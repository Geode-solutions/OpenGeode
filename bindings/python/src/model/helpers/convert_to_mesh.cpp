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
    void define_model_mesh_mapping( pybind11::module& module, const char* name )
    {
        pybind11::class_< Mapping >( module, name )
            .def( "has_mapping_input", &Mapping::has_mapping_input )
            .def( "has_mapping_output", &Mapping::has_mapping_output )
            .def( "in2out", &Mapping::in2out )
            .def( "out2in", &Mapping::out2in )
            .def( "size", &Mapping::size );
    }
} // namespace

namespace geode
{
    void define_convert_to_mesh( pybind11::module& module )
    {
        using ElementMapping = ModelToMeshMappings::MeshElementToIndexMapping;
        using IndexMapping = BijectiveMapping< index_t >;
        define_model_mesh_mapping< ElementMapping >(
            module, "MeshElementToIndexMapping" );
        define_model_mesh_mapping< IndexMapping >(
            module, "IndexToIndexMapping" );
        pybind11::class_< ModelToMeshMappings >( module, "ModelToMeshMappings" )
            .def_readonly( "corner_vertices_mapping",
                &ModelToMeshMappings::corner_vertices_mapping )
            .def_readonly(
                "line_edges_mapping", &ModelToMeshMappings::line_edges_mapping )
            .def_readonly( "surface_polygons_mapping",
                &ModelToMeshMappings::surface_polygons_mapping )
            .def_readonly( "solid_polyhedra_mapping",
                &ModelToMeshMappings::solid_polyhedra_mapping )
            .def_readonly( "unique_vertices_mapping",
                &ModelToMeshMappings::unique_vertices_mapping );

        module.def( "convert_section_into_curve", &convert_section_into_curve )
            .def(
                "convert_section_into_surface", &convert_section_into_surface )
            .def( "convert_brep_into_curve", &convert_brep_into_curve )
            .def( "convert_brep_into_surface", &convert_brep_into_surface )
            .def( "convert_brep_into_solid", &convert_brep_into_solid );
    }
} // namespace geode
