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

#include "../../basic/mapping.hpp"

#include <geode/model/helpers/convert_to_mesh.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_convert_to_mesh( pybind11::module& module )
    {
        define_bijective_mapping< MeshElement, index_t >( module,
            "MeshElementToIndexMapping",
            "One-to-one mapping between the elements of the model meshes "
            "(the input, a mesh element being the uuid of a component and an "
            "element index in its mesh) and the indices of the elements of "
            "the output mesh" );
        define_bijective_mapping< index_t >( module, "IndexToIndexMapping",
            "One-to-one mapping between the unique vertices of the model "
            "(the input) and the vertices of the output mesh" );
        pybind11::class_< ModelToMeshMappings >( module, "ModelToMeshMappings",
            "Mappings between model elements and output mesh elements, filled "
            "by the convert_xxx_into_curve, _surface and _solid functions "
            "(each says which ones it fills). Model elements which are not "
            "part of the output mesh are not mapped; the unfilled mappings are "
            "empty" )
            .def_readonly( "corner_vertices_mapping",
                &ModelToMeshMappings::corner_vertices_mapping,
                "Mapping from (Corner uuid, 0), its single vertex, to the "
                "output mesh vertex; only the Corners whose vertex is in the "
                "output mesh are mapped" )
            .def_readonly( "line_edges_mapping",
                &ModelToMeshMappings::line_edges_mapping,
                "Mapping from (Line uuid, edge index) to the output mesh edge; "
                "only the Line edges which are edges of the output mesh are "
                "mapped" )
            .def_readonly( "surface_polygons_mapping",
                &ModelToMeshMappings::surface_polygons_mapping,
                "Mapping from (Surface uuid, polygon index) to the output mesh "
                "polygon, or, in the solid conversion, to the output solid "
                "facet coincident with the polygon (mesh facets are enabled by "
                "the conversion)" )
            .def_readonly( "solid_polyhedra_mapping",
                &ModelToMeshMappings::solid_polyhedra_mapping,
                "Mapping from (Block uuid, polyhedron index) to the output "
                "solid polyhedron" )
            .def_readonly( "unique_vertices_mapping",
                &ModelToMeshMappings::unique_vertices_mapping,
                "Mapping from the unique vertices of the model to the output "
                "mesh vertices; only the unique vertices used by the output "
                "mesh are mapped" );

        module
            .def( "convert_section_into_curve", &convert_section_into_curve,
                pybind11::arg( "section" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (EdgedCurve2D, ModelToMeshMappings): the curve made of "
                "the edges of all the Lines of the Section. The mappings "
                "filled are line_edges_mapping (all the Line edges), "
                "corner_vertices_mapping (the Corners on a Line) and "
                "unique_vertices_mapping; the others are empty" )
            .def( "convert_section_into_surface", &convert_section_into_surface,
                pybind11::arg( "section" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SurfaceMesh2D, ModelToMeshMappings): the surface made "
                "of the polygons of all the Surfaces of the Section, vertices "
                "being shared according to the unique vertices. The mappings "
                "filled are surface_polygons_mapping (all the polygons), "
                "line_edges_mapping (the Line edges which are edges of the "
                "surface), corner_vertices_mapping and "
                "unique_vertices_mapping; solid_polyhedra_mapping is empty" )
            .def( "convert_brep_into_curve", &convert_brep_into_curve,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (EdgedCurve3D, ModelToMeshMappings): the curve made of "
                "the edges of all the Lines of the BRep. The mappings filled "
                "are line_edges_mapping (all the Line edges), "
                "corner_vertices_mapping (the Corners on a Line) and "
                "unique_vertices_mapping; the others are empty" )
            .def( "convert_brep_into_surface", &convert_brep_into_surface,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SurfaceMesh3D, ModelToMeshMappings): the surface made "
                "of the polygons of all the Surfaces of the BRep, vertices "
                "being shared according to the unique vertices. The mappings "
                "filled are surface_polygons_mapping (all the polygons), "
                "line_edges_mapping (the Line edges which are edges of the "
                "surface), corner_vertices_mapping and "
                "unique_vertices_mapping; solid_polyhedra_mapping is empty" )
            .def( "convert_brep_into_solid", &convert_brep_into_solid,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SolidMesh3D, ModelToMeshMappings): the solid made of "
                "the polyhedra of all the Blocks of the BRep, vertices being "
                "shared according to the unique vertices; raise an error if a "
                "unique vertex is not in a Block. The mappings filled are "
                "solid_polyhedra_mapping, unique_vertices_mapping and, only if "
                "the solid has polyhedra, surface_polygons_mapping (to the "
                "solid facets), line_edges_mapping and "
                "corner_vertices_mapping; they stay empty otherwise" );
    }
} // namespace geode
