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

#include "../../common.hpp"

#include <geode/model/helpers/convert_to_mesh.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Mapping >
    void define_model_mesh_mapping(
        pybind11::module& module, const char* name, const char* doc )
    {
        pybind11::class_< Mapping >( module, name, doc )
            .def( "has_mapping_input", &Mapping::has_mapping_input,
                pybind11::arg( "input" ),
                "Return True if the input is mapped to an output" )
            .def( "has_mapping_output", &Mapping::has_mapping_output,
                pybind11::arg( "output" ),
                "Return True if the output is mapped to an input" )
            .def( "in2out", &Mapping::in2out, pybind11::arg( "input" ),
                "Return the output mapped to the input; raise an error if "
                "there is none" )
            .def( "out2in", &Mapping::out2in, pybind11::arg( "output" ),
                "Return the input mapped to the output; raise an error if "
                "there is none" )
            .def( "size", &Mapping::size,
                "Return the number of mapped input and output pairs" );
    }
} // namespace

namespace geode
{
    void define_convert_to_mesh( pybind11::module& module )
    {
        using ElementMapping = ModelToMeshMappings::MeshElementToIndexMapping;
        using IndexMapping = BijectiveMapping< index_t >;
        define_model_mesh_mapping< ElementMapping >( module,
            "MeshElementToIndexMapping",
            "One-to-one mapping between the elements of the model meshes "
            "(the input, a mesh element being the uuid of a component and an "
            "element index in its mesh) and the indices of the elements of "
            "the output mesh" );
        define_model_mesh_mapping< IndexMapping >( module,
            "IndexToIndexMapping",
            "One-to-one mapping between the unique vertices of the model "
            "(the input) and the vertices of the output mesh" );
        pybind11::class_< ModelToMeshMappings >( module, "ModelToMeshMappings",
            "Mappings between model elements and output mesh elements. Model "
            "elements which are not part of the output mesh are not mapped "
            "(e.g. Corners not on any Line in the curve conversion, Lines not "
            "on any Surface edge in the surface conversion)" )
            .def_readonly( "corner_vertices_mapping",
                &ModelToMeshMappings::corner_vertices_mapping,
                "Mapping between the Corners (their single vertex) and the "
                "output mesh vertices" )
            .def_readonly( "line_edges_mapping",
                &ModelToMeshMappings::line_edges_mapping,
                "Mapping between the Line edges and the output mesh edges" )
            .def_readonly( "surface_polygons_mapping",
                &ModelToMeshMappings::surface_polygons_mapping,
                "Mapping between the Surface polygons and the output mesh "
                "polygons" )
            .def_readonly( "solid_polyhedra_mapping",
                &ModelToMeshMappings::solid_polyhedra_mapping,
                "Mapping between the Block polyhedra and the output mesh "
                "polyhedra" )
            .def_readonly( "unique_vertices_mapping",
                &ModelToMeshMappings::unique_vertices_mapping,
                "Mapping between the unique vertices of the model and the "
                "output mesh vertices" );

        module
            .def( "convert_section_into_curve", &convert_section_into_curve,
                pybind11::arg( "section" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (EdgedCurve2D, ModelToMeshMappings): the curve made of "
                "the edges of all the Lines of the Section. Corners not on any "
                "Line are not mapped" )
            .def( "convert_section_into_surface", &convert_section_into_surface,
                pybind11::arg( "section" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SurfaceMesh2D, ModelToMeshMappings): the surface made "
                "of the polygons of all the Surfaces of the Section, vertices "
                "being shared according to the unique vertices" )
            .def( "convert_brep_into_curve", &convert_brep_into_curve,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (EdgedCurve3D, ModelToMeshMappings): the curve made of "
                "the edges of all the Lines of the BRep. Corners not on any "
                "Line are not mapped" )
            .def( "convert_brep_into_surface", &convert_brep_into_surface,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SurfaceMesh3D, ModelToMeshMappings): the surface made "
                "of the polygons of all the Surfaces of the BRep, vertices "
                "being shared according to the unique vertices" )
            .def( "convert_brep_into_solid", &convert_brep_into_solid,
                pybind11::arg( "brep" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return (SolidMesh3D, ModelToMeshMappings): the solid made of "
                "the polyhedra of all the Blocks of the BRep, vertices being "
                "shared according to the unique vertices; raise an error if a "
                "unique "
                "vertex is not in a Block" );
    }
} // namespace geode
