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

#pragma once

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollections );
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollectionsBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Collection component describing a Boundary.
     */
    template < index_t dimension >
    class CornerCollection : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( CornerCollection );
        PASSKEY( CornerCollections< dimension >, CornerCollectionsKey );
        PASSKEY( CornerCollectionsBuilder< dimension >,
            CornerCollectionsBuilderKey );
        friend class bitsery::Access;

    public:
        CornerCollection( CornerCollection&& other ) noexcept = default;
        ~CornerCollection() = default;

        [[nodiscard]] static ComponentType component_type_static()
        {
            return ComponentType{ "CornerCollection" };
        }

        [[nodiscard]] ComponentType component_type() const override
        {
            return component_type_static();
        }

    public:
        CornerCollection( CornerCollectionsKey ) : CornerCollection() {}

        void set_corner_collection_name(
            std::string_view name, CornerCollectionsBuilderKey )
        {
            this->set_name( name );
        }

    private:
        CornerCollection() = default;

        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_2D_AND_3D( CornerCollection );
} // namespace geode
