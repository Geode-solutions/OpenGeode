/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/coordinate_reference_system.h>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class AttributeCoordinateReferenceSystem
        : public CoordinateReferenceSystem< dimension >
    {
        friend class bitsery::Access;

    public:
        AttributeCoordinateReferenceSystem( AttributeManager& manager );
        AttributeCoordinateReferenceSystem(
            AttributeManager& manager, absl::string_view attribute_name );
        ~AttributeCoordinateReferenceSystem();

        static CRSType type_name_static()
        {
            return CRSType{ "AttributeCoordinateReferenceSystem" };
        }

        CRSType type_name() const override
        {
            return type_name_static();
        }

        const Point< dimension >& point( index_t point_id ) const override;

        void set_point( index_t point_id, Point< dimension > point ) override;

    private:
        AttributeCoordinateReferenceSystem();

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( AttributeCoordinateReferenceSystem );
} // namespace geode