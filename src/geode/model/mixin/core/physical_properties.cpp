/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/mixin/core/physical_properties.hpp>

#include <filesystem>
#include <fstream>

#include <bitsery/ext/std_map.h>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    PhysicalProperties::Info::Info()
        : component_type( bitsery::Access::create< ComponentType >() )
    {
    }

    template < typename Archive >
    void PhysicalProperties::Info::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Info >{ { []( Archive& a, Info& info ) {
                a.object( info.component_type );
                a.object( info.attribute_id );
            } } } );
    }

    class PhysicalProperties::Impl
    {
    public:
        Impl() = default;

        Impl( BITSERY ) {}

        [[nodiscard]] bool has_property( PHYSICAL_PROPERTY_NAME name ) const
        {
            return properties_.contains( name );
        }

        [[nodiscard]] const PhysicalProperties::Info& property_info(
            PHYSICAL_PROPERTY_NAME name ) const
        {
            const auto property_it = properties_.find( name );
            OpenGeodeModelException::check_exception(
                property_it != properties_.end(), nullptr,
                OpenGeodeException::TYPE::data,
                "[PhysicalProperties::physical_property_info] Physical "
                "property not found. Use has_physical_property before "
                "calling this method." );
            return property_it->second;
        }

        void set_property( PHYSICAL_PROPERTY_NAME name,
            ComponentType component_type,
            uuid attribute_id )
        {
            properties_.insert_or_assign(
                name, PhysicalProperties::Info{ std::move( component_type ),
                          std::move( attribute_id ) } );
        }

        void copy( const Impl& other )
        {
            properties_ = other.properties_;
        }

        void save( std::string_view directory ) const
        {
            const auto filename =
                absl::StrCat( directory, "/physical_properties" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeModelException::check_exception(
                std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[PhysicalProperties::save] Error while writing file: ",
                filename );
        }

        void load( std::string_view directory )
        {
            const auto filename =
                absl::StrCat( directory, "/physical_properties" );
            if( !std::filesystem::exists( filename ) )
            {
                return;
            }
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OpenGeodeModelException::check_exception(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[PhysicalProperties::load] Error while reading file: ",
                filename );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl.properties_,
                        bitsery::ext::StdMap{ impl.properties_.max_size() },
                        []( Archive& a2, PHYSICAL_PROPERTY_NAME& name,
                            Info& info ) {
                            a2.value4b( name );
                            a2.object( info );
                        } );
                } } } );
        }

    private:
        absl::flat_hash_map< PHYSICAL_PROPERTY_NAME, PhysicalProperties::Info >
            properties_;
    };

    PhysicalProperties::PhysicalProperties() = default;

    PhysicalProperties::~PhysicalProperties() = default;

    PhysicalProperties::PhysicalProperties(
        PhysicalProperties&& other ) noexcept = default;

    PhysicalProperties& PhysicalProperties::operator=(
        PhysicalProperties&& other ) noexcept = default;

    bool PhysicalProperties::has_physical_property(
        PHYSICAL_PROPERTY_NAME name ) const
    {
        return impl_->has_property( name );
    }

    const PhysicalProperties::Info& PhysicalProperties::physical_property_info(
        PHYSICAL_PROPERTY_NAME name ) const
    {
        return impl_->property_info( name );
    }

    void PhysicalProperties::save_physical_properties(
        std::string_view directory ) const
    {
        impl_->save( directory );
    }

    void PhysicalProperties::copy_physical_properties(
        const PhysicalProperties& other, BuilderKey /*key*/ )
    {
        impl_->copy( *other.impl_ );
    }

    void PhysicalProperties::load_physical_properties(
        std::string_view directory, BuilderKey /*key*/ )
    {
        impl_->load( directory );
    }

    void PhysicalProperties::set_physical_property( PHYSICAL_PROPERTY_NAME name,
        ComponentType component_type,
        uuid attribute_id,
        BuilderKey /*key*/ )
    {
        impl_->set_property(
            name, std::move( component_type ), std::move( attribute_id ) );
    }
} // namespace geode
