/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/identifier.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    class Identifier::Impl
    {
    public:
        Impl() = default;

        const uuid& id() const
        {
            return id_;
        }

        std::string_view name() const
        {
            if( name_ )
            {
                return name_.value();
            }
            return id_.string();
        }

        void set_id( const uuid& unique_id )
        {
            id_ = unique_id;
        }

        void set_name( std::string_view name )
        {
            name_ = to_string( name );
        }

        void save( std::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/identifier" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[Identifier::save] Error while writing file: ", filename );
        }

        void load( std::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/identifier" );
            std::ifstream file{ filename, std::ifstream::binary };
            if( !file )
            {
                return;
            }
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Identifier::load] Error while reading file: ", filename );
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{
                                    { []( Archive& local_archive, Impl& impl ) {
                                        local_archive.object( impl.id_ );
                                        local_archive.text1b(
                                            impl.name_, impl.name_.max_size() );
                                    } } } );
        }

    private:
        uuid id_;
        std::optional< std::string > name_ = std::nullopt;
    };

    Identifier::Identifier() = default;

    Identifier::~Identifier() = default;

    Identifier::Identifier( Identifier&& ) noexcept = default;

    Identifier& Identifier::operator=( Identifier&& ) noexcept = default;

    const uuid& Identifier::id() const
    {
        return impl_->id();
    }

    std::string_view Identifier::name() const
    {
        return impl_->name();
    }

    void Identifier::save_identifier( std::string_view directory ) const
    {
        impl_->save( directory );
    }

    void Identifier::load_identifier(
        std::string_view directory, IdentifierKey /*unused*/ )
    {
        impl_->load( directory );
    }

    void Identifier::set_id( const uuid& unique_id, IdentifierKey /*unused*/ )
    {
        set_id( unique_id );
    }

    void Identifier::set_name( std::string_view name, IdentifierKey /*unused*/ )
    {
        set_name( name );
    }

    void Identifier::copy_identifier(
        const Identifier& other, IdentifierKey /*unused*/ )
    {
        set_name( other.name() );
        set_id( other.id() );
    }

    void Identifier::set_id( const uuid& unique_id )
    {
        impl_->set_id( unique_id );
    }

    void Identifier::set_name( std::string_view name )
    {
        impl_->set_name( name );
    }

    template < typename Archive >
    void Identifier::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Identifier >{
                       { []( Archive& local_archive, Identifier& identifier ) {
                           local_archive.object( identifier.impl_ );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, Identifier );
} // namespace geode
