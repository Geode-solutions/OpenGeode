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

#pragma once

#include <cstdint>
#include <string_view>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    class PhysicalPropertiesBuilder;
} // namespace geode

namespace geode
{

    enum struct PHYSICAL_PROPERTY_NAME
    {
        porosity,
        permeability
    };

    class opengeode_model_api PhysicalProperties
    {
    public:
        PASSKEY( PhysicalPropertiesBuilder, BuilderKey );

        struct PhysicalPropertyInfo
        {
            PhysicalPropertyInfo(
                ComponentType component_type_in, uuid attribute_id_in )
                : component_type( std::move( component_type_in ) ),
                  attribute_id( std::move( attribute_id_in ) )
            {
            }

            ComponentType component_type;
            uuid attribute_id;

        private:
            friend class bitsery::Access;
            PhysicalPropertyInfo();
            template < typename Archive >
            void serialize( Archive& archive );
        };

        PhysicalProperties();
        PhysicalProperties( BITSERY );
        ~PhysicalProperties();

        [[nodiscard]] bool has_physical_property(
            PHYSICAL_PROPERTY_NAME name ) const;

        [[nodiscard]] const PhysicalPropertyInfo& physical_property_attribute(
            PHYSICAL_PROPERTY_NAME name ) const;

        void save_physical_properties( std::string_view directory ) const;

    public:
        void set_physical_property( PHYSICAL_PROPERTY_NAME name,
            ComponentType component_type,
            uuid attribute_id,
            BuilderKey /*key*/ );

        void copy_physical_properties(
            const PhysicalProperties& other, BuilderKey /*key*/ );

        void load_physical_properties(
            std::string_view directory, BuilderKey /*key*/ );

    protected:
        PhysicalProperties( PhysicalProperties&& other ) noexcept;
        PhysicalProperties& operator=( PhysicalProperties&& other ) noexcept;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
