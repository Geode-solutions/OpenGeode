/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <iostream>

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/point_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodePointSetBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodePointSet : public PointSet< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( OpenGeodePointSet );
        friend class OpenGeodePointSetBuilder< dimension >;

    public:
        OpenGeodePointSet();
        ~OpenGeodePointSet();

        static MeshType type_name_static()
        {
            return MeshType{ absl::StrCat(
                "OpenGeodePointSet", dimension, "D" ) };
        }

        MeshType type_name() const override
        {
            return type_name_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_pts", dimension, "d" );
            return extension;
        }

        absl::string_view native_extension() const override
        {
            return native_extension_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        const Point< dimension >& get_point( index_t vertex_id ) const override;

        void set_vertex( index_t vertex_id, const Point< dimension >& point );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( OpenGeodePointSet );
} // namespace geode
