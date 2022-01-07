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

#include <geode/mesh/builder/polyhedral_solid_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace geode
{
    template < index_t dimension >
    PolyhedralSolidBuilder< dimension >::PolyhedralSolidBuilder(
        VertexSet& vertex_set )
        : SolidMeshBuilder< dimension >( vertex_set )
    {
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolidBuilder< dimension > >
        PolyhedralSolidBuilder< dimension >::create(
            PolyhedralSolid< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            PolyhedralSolidBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::set_mesh(
        PolyhedralSolid< dimension >& mesh, MeshBuilderFactoryKey key )
    {
        polyhedral_solid_ = &mesh;
        SolidMeshBuilder< dimension >::set_mesh( mesh, key );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::copy(
        const PolyhedralSolid< dimension >& polyhedral_solid, BuilderKey )
    {
        SolidMeshBuilder< dimension >::copy( polyhedral_solid );
    }

    template class opengeode_mesh_api PolyhedralSolidBuilder< 3 >;
} // namespace geode
