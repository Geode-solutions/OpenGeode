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

#include <geode/basic/identifier.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

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

        absl::string_view name() const
        {
            return name_;
        }

        void set_id( uuid id )
        {
            id_ = std::move( id );
        }

        void set_name( absl::string_view name )
        {
            name_ = to_string( name );
        }

        void save( absl::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/identifier" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            register_basic_serialize_pcontext( std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[Identifier::save] Error while writing file: ", filename );
        }

        void load( absl::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/identifier" );
            std::ifstream file{ filename, std::ifstream::binary };
            if( !file )
            {
                return;
            }
            TContext context{};
            register_basic_deserialize_pcontext( std::get< 0 >( context ) );
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
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.object( impl.id_ );
                    a.text1b( impl.name_, impl.name_.max_size() );
                } );
        }

    private:
        uuid id_;
        std::string name_ = std::string{ DEFAULT_NAME };
    };

    Identifier::Identifier() {}

    Identifier::~Identifier() {}

    Identifier::Identifier( Identifier&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    const uuid& Identifier::id() const
    {
        return impl_->id();
    }

    absl::string_view Identifier::name() const
    {
        return impl_->name();
    }

    void Identifier::save_identifier( absl::string_view directory ) const
    {
        impl_->save( directory );
    }

    void Identifier::load_identifier(
        absl::string_view directory, IdentifierKey )
    {
        impl_->load( directory );
    }

    void Identifier::set_id( uuid id, IdentifierKey )
    {
        set_id( std::move( id ) );
    }

    void Identifier::set_name( absl::string_view name, IdentifierKey )
    {
        set_name( name );
    }

    void Identifier::set_id( uuid id )
    {
        impl_->set_id( std::move( id ) );
    }

    void Identifier::set_name( absl::string_view name )
    {
        impl_->set_name( name );
    }

    template < typename Archive >
    void Identifier::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, Identifier >{},
            []( Archive& a, Identifier& identifier ) {
                a.object( identifier.impl_ );
            } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, Identifier );
} // namespace geode
