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

#include <bitsery/ext/inheritance.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    class opengeode_mesh_api OpenGeodeVertexSet : public VertexSet
    {
    public:
        static MeshImpl impl_name_static()
        {
            return MeshImpl{ "OpenGeodeVertexSet" };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return VertexSet::type_name_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto extension = "og_vts";
            return extension;
        }

        absl::string_view native_extension() const override
        {
            return native_extension_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, OpenGeodeVertexSet >{},
                []( Archive &archive, OpenGeodeVertexSet &vertex_set ) {
                    archive.ext(
                        vertex_set, bitsery::ext::BaseClass< VertexSet >{} );
                } );
        }
    };
} // namespace geode
