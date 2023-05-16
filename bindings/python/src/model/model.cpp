/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include "../common.h"
#include <pybind11/iostream.h>

#include <geode/model/common.h>

namespace geode
{
    void define_component_type( pybind11::module& );
    void define_component( pybind11::module& );
    void define_component_mesh_element( pybind11::module& );
    void define_block( pybind11::module& );
    void define_blocks( pybind11::module& );
    void define_corner( pybind11::module& );
    void define_corners( pybind11::module& );
    void define_line( pybind11::module& );
    void define_lines( pybind11::module& );
    void define_model_boundary( pybind11::module& );
    void define_model_boundaries( pybind11::module& );
    void define_surface( pybind11::module& );
    void define_surfaces( pybind11::module& );
    void define_relationships( pybind11::module& );
    void define_vertex_identifier( pybind11::module& );
    void define_topology( pybind11::module& );

    void define_blocks_builder( pybind11::module& );
    void define_corners_builder( pybind11::module& );
    void define_lines_builder( pybind11::module& );
    void define_model_boundaries_builder( pybind11::module& );
    void define_surfaces_builder( pybind11::module& );
    void define_relationships_builder( pybind11::module& );
    void define_vertex_identifier_builder( pybind11::module& );
    void define_topology_builder( pybind11::module& );

    void define_brep( pybind11::module& );
    void define_section( pybind11::module& );
    void define_copy_mapping( pybind11::module& );
    void define_generic_mapping( pybind11::module& );

    void define_brep_builder( pybind11::module& );
    void define_section_builder( pybind11::module& );

    void define_brep_io( pybind11::module& );
    void define_section_io( pybind11::module& );

    void define_convert_brep_section( pybind11::module& );
    void define_convert_model_meshes( pybind11::module& );
    void define_convert_to_mesh( pybind11::module& );
    void define_component_mesh_polygons( pybind11::module& );
    void define_model_component_filter( pybind11::module& );
    void define_model_concatener( pybind11::module& );
    void define_model_coordinate_reference_system( pybind11::module& );
} // namespace geode

PYBIND11_MODULE( opengeode_py_model, module )
{
    try
    {
        pybind11::module::import( "opengeode_py_basic" );
        pybind11::module::import( "opengeode_py_mesh" );
    }
    catch( ... )
    {
        pybind11::module::import( "opengeode" );
    }
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for model";
    pybind11::class_< geode::OpenGeodeModelLibrary >(
        module, "OpenGeodeModelLibrary" )
        .def( "initialize", &geode::OpenGeodeModelLibrary::initialize );
    geode::define_component_type( module );
    geode::define_component( module );
    geode::define_component_mesh_element( module );
    geode::define_block( module );
    geode::define_blocks( module );
    geode::define_corner( module );
    geode::define_corners( module );
    geode::define_line( module );
    geode::define_lines( module );
    geode::define_model_boundary( module );
    geode::define_model_boundaries( module );
    geode::define_surface( module );
    geode::define_surfaces( module );
    geode::define_relationships( module );
    geode::define_vertex_identifier( module );
    geode::define_topology( module );

    geode::define_blocks_builder( module );
    geode::define_corners_builder( module );
    geode::define_lines_builder( module );
    geode::define_model_boundaries_builder( module );
    geode::define_surfaces_builder( module );
    geode::define_relationships_builder( module );
    geode::define_vertex_identifier_builder( module );
    geode::define_topology_builder( module );

    geode::define_brep( module );
    geode::define_section( module );
    geode::define_copy_mapping( module );
    geode::define_generic_mapping( module );

    geode::define_brep_builder( module );
    geode::define_section_builder( module );

    geode::define_brep_io( module );
    geode::define_section_io( module );

    geode::define_convert_brep_section( module );
    geode::define_convert_model_meshes( module );
    geode::define_convert_to_mesh( module );
    geode::define_component_mesh_polygons( module );
    geode::define_model_component_filter( module );
    geode::define_model_concatener( module );
    geode::define_model_coordinate_reference_system( module );
}