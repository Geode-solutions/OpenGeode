/*
 * Copyright (c) 2019 - 2022 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include <geode/basic/database.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

#include <async++.h>

#include <ghc/filesystem.hpp>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/identifier.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

namespace
{
    constexpr auto DATA_EXPIRATION = std::chrono::minutes( 3 );
} // namespace

namespace geode
{
    class Database::Storage
    {
    public:
        Storage( std::unique_ptr< geode::Identifier >&& data )
            : data_{ std::move( data ) }
        {
        }

        ~Storage()
        {
            terminate_storage();
            std::unique_lock< std::mutex > locking{ lock_ };
            condition_.wait( locking, [this] {
                clean_queue();
                return queue_.empty();
            } );
        }

        bool expired() const
        {
            return !data_;
        }

        bool unused() const
        {
            return counter_ == 0;
        }

        void new_data_reference()
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            counter_++;
            last_++;
        }

        void delete_data_reference()
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            OPENGEODE_ASSERT(
                counter_ > 0, "[Database::Storage] Cannot decrement" );
            counter_--;
            if( unused() )
            {
                clean_queue();
                wait_for_memory_release();
            }
        }

        void set_data( std::unique_ptr< geode::Identifier >&& data )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            counter_ = 0;
            data_ = std::move( data );
        }

        const std::unique_ptr< geode::Identifier >& data() const
        {
            return data_;
        }

        geode::Identifier* steal_data()
        {
            return data_.release();
        }

    private:
        void terminate_storage()
        {
            terminate_ = true;
            condition_.notify_all();
        }

        void clean_queue()
        {
            while( !queue_.empty() )
            {
                if( !queue_.front().ready() )
                {
                    break;
                }
                queue_.pop();
            }
            condition_.notify_all();
        }

        void wait_for_memory_release()
        {
            const auto last = last_;
            queue_.emplace( async::spawn( [this, last] {
                std::unique_lock< std::mutex > locking{ lock_ };
                if( !condition_.wait_for(
                        locking, DATA_EXPIRATION, [this, last] {
                            return terminate_.load();
                        } ) )
                {
                    if( last == last_ )
                    {
                        data_.reset();
                    }
                }
                locking.unlock();
                condition_.notify_all();
            } ) );
        }

    private:
        std::unique_ptr< Identifier > data_;
        std::atomic< bool > terminate_{ false };
        index_t counter_{ 0 };
        std::mutex lock_;
        std::condition_variable condition_;
        index_t last_{ 0 };
        std::queue< async::task< void > > queue_;
    };

    class Database::Impl
    {
    public:
        Impl( absl::string_view directory )
            : directory_{ to_string( directory ) }
        {
            if( !ghc::filesystem::exists( directory_ ) )
            {
                ghc::filesystem::create_directory( directory_ );
            }
        }

        index_t nb_data() const
        {
            return storage_.size();
        }

        void register_unique_data(
            const uuid& id, std::unique_ptr< Identifier >&& data )
        {
            save_data( id, data );
            register_data( id, std::move( data ) );
        }

        std::shared_ptr< Storage > data( const uuid& id ) const
        {
            auto& storage = storage_.at( id );
            if( !storage || storage->expired() )
            {
                storage = std::make_shared< Storage >( load_data( id ) );
            }
            return storage;
        }

        std::unique_ptr< Identifier > steal_data( const uuid& id )
        {
            auto& storage = storage_.at( id );
            if( storage && storage->unused() && !storage->expired() )
            {
                auto* data = storage->steal_data();
                storage.reset();
                return std::unique_ptr< Identifier >{ data };
            }
            return load_data( id );
        }

        void delete_data( const uuid& id )
        {
            storage_.erase( id );
        }

        void register_serializer_functions(
            serializer_function serializer, serializer_function deserializer )
        {
            serializers_.push_back( serializer );
            deserializers_.push_back( deserializer );
        }

    private:
        const std::unique_ptr< Identifier >& register_data(
            const uuid& id, std::unique_ptr< Identifier >&& data )
        {
            const auto it = storage_.find( id );
            if( it != storage_.end() )
            {
                if( it->second->unused() )
                {
                    it->second->set_data( std::move( data ) );
                    return it->second->data();
                }
                delete_data( id );
                return do_register_data( id, std::move( data ) );
            }
            return do_register_data( id, std::move( data ) );
        }

        const std::unique_ptr< Identifier >& do_register_data(
            const uuid& id, std::unique_ptr< Identifier >&& data )
        {
            auto new_storage = std::make_shared< Storage >( std::move( data ) );
            return storage_.emplace( id, std::move( new_storage ) )
                .first->second->data();
        }

        void save_data(
            const uuid& id, const std::unique_ptr< Identifier >& data ) const
        {
            const auto filename = absl::StrCat( directory_, "/", id.string() );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context;
            for( const auto& serializer : serializers_ )
            {
                serializer( std::get< 0 >( context ) );
            }
            Serializer archive{ context, file };
            archive.ext( data, bitsery::ext::StdSmartPtr{} );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[Database::save_data] Error while writing file: ", filename );
        }

        std::unique_ptr< Identifier > load_data( const uuid& id ) const
        {
            const auto filename = absl::StrCat( directory_, "/", id.string() );
            std::ifstream file{ filename, std::ifstream::binary };
            OPENGEODE_EXCEPTION(
                file, "[Database::load_data] Failed to open file: ", filename );
            TContext context{};
            for( const auto& deserializer : deserializers_ )
            {
                deserializer( std::get< 0 >( context ) );
            }
            Deserializer archive{ context, file };
            std::unique_ptr< Identifier > data;
            archive.ext( data, bitsery::ext::StdSmartPtr{} );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Database::load_data] Error while reading file: ", filename );
            return data;
        }

    private:
        std::string directory_;
        mutable absl::flat_hash_map< uuid, std::shared_ptr< Storage > >
            storage_;
        std::vector< serializer_function > serializers_;
        std::vector< serializer_function > deserializers_;
    };

    class Database::Data::Impl
    {
    public:
        Impl( std::shared_ptr< Storage > storage )
            : storage_{ std::move( storage ) }
        {
            storage_->new_data_reference();
        }

        ~Impl()
        {
            storage_->delete_data_reference();
        }

        const Identifier& data() const
        {
            return *storage_->data();
        }

    private:
        std::shared_ptr< Storage > storage_;
    };

    Database::Data::Data( std::shared_ptr< Storage > storage )
        : impl_{ std::move( storage ) }
    {
    }

    Database::Data::~Data() {} // NOLINT

    Database::Data::Data( Data&& other ) : impl_{ std::move( other.impl_ ) } {}

    const Identifier& Database::Data::data() const
    {
        return impl_->data();
    }

    Database::Database( absl::string_view directory ) : impl_{ directory } {}

    Database::~Database() {} // NOLINT

    index_t Database::nb_data() const
    {
        return impl_->nb_data();
    }

    void Database::register_unique_data(
        const uuid& id, std::unique_ptr< Identifier >&& data )
    {
        impl_->register_unique_data( id, std::move( data ) );
    }

    Database::Data Database::get_data( const uuid& id ) const
    {
        return { impl_->data( id ) };
    }

    std::unique_ptr< Identifier > Database::steal_data( const uuid& id )
    {
        return impl_->steal_data( id );
    }

    void Database::delete_data( const uuid& id )
    {
        impl_->delete_data( id );
    }

    void Database::register_serializer_functions(
        serializer_function serializer, serializer_function deserializer )
    {
        impl_->register_serializer_functions( serializer, deserializer );
    }
} // namespace geode