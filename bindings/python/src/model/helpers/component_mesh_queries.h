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

#include <geode/model/helpers/component_mesh_queries.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    void define_component_mesh_queries( pybind11::module& module )
    {
        pybind11::class_< BlockPolyhedronFacet >(
            module, "BlockPolyhedronFacet" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet >() )
            .def_readwrite( "facet", &BlockPolyhedronFacet::facet )
            .def_readwrite( "vertices", &BlockPolyhedronFacet::vertices );

        pybind11::class_< BlockPolyhedraFacetVertices >(
            module, "BlockPolyhedraFacetVertices" )
            .def( pybind11::init<>() )
            .def( "nb_facets", &BlockPolyhedraFacetVertices::nb_facets )
            .def_readwrite( "oriented_polyhedron_facet",
                &BlockPolyhedraFacetVertices::oriented_polyhedron_facet )
            .def_readwrite( "opposite_polyhedron_facet",
                &BlockPolyhedraFacetVertices::opposite_polyhedron_facet );

        module
            .def( "surface_polygon_unique_vertices",
                &surface_polygon_unique_vertices )
            .def( "block_mesh_polyhedra_from_surface_polygon",
                &block_mesh_polyhedra_from_surface_polygon )
            .def( "block_vertices_from_surface_polygon",
                &block_vertices_from_surface_polygon )
            .def( "oriented_block_vertices_from_surface_polygon",
                &oriented_block_vertices_from_surface_polygon );
    }
} // namespace geode