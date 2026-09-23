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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    class PhysicalProperties::Impl
    {
    public:
        [[nodiscard]] bool has_property( PHYSICAL_PROPERTY_NAME name ) const
        {
            return properties_.contains( name );
        }

        [[nodiscard]] const PhysicalProperties::PhysicalPropertyInfo&
            property_attribute( PHYSICAL_PROPERTY_NAME name ) const
        {
            return properties_.at( name );
        }

        void set_property( PHYSICAL_PROPERTY_NAME name,
            ComponentType component_type,
            uuid attribute_id )
        {
            properties_.insert_or_assign( name,
                PhysicalProperties::PhysicalPropertyInfo{
                    std::move( component_type ), std::move( attribute_id ) } );
        }

    private:
        absl::flat_hash_map< PHYSICAL_PROPERTY_NAME,
            PhysicalProperties::PhysicalPropertyInfo >
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

    const PhysicalProperties::PhysicalPropertyInfo&
        PhysicalProperties::physical_property_attribute(
            PHYSICAL_PROPERTY_NAME name ) const
    {
        return impl_->property_attribute( name );
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
