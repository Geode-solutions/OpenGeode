/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/builder/geode/geode_point_set_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode/geode_point_set.h>

namespace geode
{
    template < index_t dimension >
    OpenGeodePointSetBuilder< dimension >::OpenGeodePointSetBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodePointSetBuilder< dimension >(
            dynamic_cast< OpenGeodePointSet< dimension >& >( vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodePointSetBuilder< dimension >::OpenGeodePointSetBuilder(
        OpenGeodePointSet< dimension >& mesh )
        : PointSetBuilder< dimension >( mesh ), geode_point_set_( mesh )
    {
    }

    template < index_t dimension >
    void OpenGeodePointSetBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePointSetBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePointSetBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePointSetBuilder< dimension >::do_permute_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePointSetBuilder< dimension >::do_copy_points(
        const PointSet< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template class opengeode_mesh_api OpenGeodePointSetBuilder< 2 >;
    template class opengeode_mesh_api OpenGeodePointSetBuilder< 3 >;
} // namespace geode
