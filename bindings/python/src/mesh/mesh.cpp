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

#include "../common.h"
#include <pybind11/iostream.h>

#include <geode/mesh/common.h>

namespace geode
{
    void define_crs( pybind11::module& );
    void define_crs_manager( pybind11::module& );
    void define_crs_managers( pybind11::module& );
    void define_crs_manager_builder( pybind11::module& );
    void define_crs_managers_builder( pybind11::module& );

    void define_vertex_set( pybind11::module& );
    void define_graph( pybind11::module& );
    void define_edged_curve( pybind11::module& );
    void define_grid( pybind11::module& );
    void define_point_set( pybind11::module& );
    void define_surface_mesh( pybind11::module& );
    void define_surface_edges( pybind11::module& );
    void define_polygonal_surface( pybind11::module& );
    void define_triangulated_surface( pybind11::module& );
    void define_solid_mesh( pybind11::module& );
    void define_solid_edges( pybind11::module& );
    void define_solid_facets( pybind11::module& );
    void define_light_regular_grid( pybind11::module& );
    void define_regular_grid( pybind11::module& );
    void define_polyhedral_solid( pybind11::module& );
    void define_tetrahedral_solid( pybind11::module& );
    void define_hybrid_solid( pybind11::module& );

    void define_texture( pybind11::module& );
    void define_texture_manager( pybind11::module& );

    void define_vertex_set_builder( pybind11::module& );
    void define_graph_builder( pybind11::module& );
    void define_edged_curve_builder( pybind11::module& );
    void define_point_set_builder( pybind11::module& );
    void define_surface_mesh_builder( pybind11::module& );
    void define_surface_edges_builder( pybind11::module& );
    void define_polygonal_surface_builder( pybind11::module& );
    void define_triangulated_surface_builder( pybind11::module& );
    void define_solid_mesh_builder( pybind11::module& );
    void define_solid_edges_builder( pybind11::module& );
    void define_solid_facets_builder( pybind11::module& );
    void define_regular_grid_builder( pybind11::module& );
    void define_polyhedral_solid_builder( pybind11::module& );
    void define_tetrahedral_solid_builder( pybind11::module& );
    void define_hybrid_solid_builder( pybind11::module& );

    void define_convert_edged_curve( pybind11::module& );
    void define_convert_point_set( pybind11::module& );
    void define_convert_surface_mesh( pybind11::module& );
    void define_convert_solid_mesh( pybind11::module& );
    void define_euclidean_distance_transform( pybind11::module& );
    void define_repair_polygon_orientations( pybind11::module& );
    void define_geometrical_operations_on_mesh( pybind11::module& );
    void define_gradient_computation( pybind11::module& module );
    void define_mesh_crs_helper( pybind11::module& );

    void define_vertex_set_io( pybind11::module& );
    void define_graph_io( pybind11::module& );
    void define_edged_curve_io( pybind11::module& );
    void define_light_regular_grid_io( pybind11::module& );
    void define_point_set_io( pybind11::module& );
    void define_polygonal_surface_io( pybind11::module& );
    void define_triangulated_surface_io( pybind11::module& );
    void define_polyhedral_solid_io( pybind11::module& );
    void define_tetrahedral_solid_io( pybind11::module& );
    void define_hybrid_solid_io( pybind11::module& );
    void define_regular_grid_io( pybind11::module& );
} // namespace geode

PYBIND11_MODULE( opengeode_py_mesh, module )
{
    try
    {
        pybind11::module::import( "opengeode_py_basic" );
    }
    catch( ... )
    {
        pybind11::module::import( "opengeode" );
    }
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for mesh";
    pybind11::class_< geode::OpenGeodeMeshLibrary >(
        module, "OpenGeodeMeshLibrary" )
        .def( "initialize", &geode::OpenGeodeMeshLibrary::initialize );

    geode::define_crs( module );
    geode::define_crs_manager( module );
    geode::define_crs_managers( module );
    geode::define_crs_manager_builder( module );
    geode::define_crs_managers_builder( module );

    geode::define_vertex_set( module );
    geode::define_graph( module );
    geode::define_edged_curve( module );
    geode::define_grid( module );
    geode::define_point_set( module );
    geode::define_surface_mesh( module );
    geode::define_surface_edges( module );
    geode::define_polygonal_surface( module );
    geode::define_triangulated_surface( module );
    geode::define_solid_mesh( module );
    geode::define_solid_edges( module );
    geode::define_solid_facets( module );
    geode::define_light_regular_grid( module );
    geode::define_regular_grid( module );
    geode::define_polyhedral_solid( module );
    geode::define_tetrahedral_solid( module );
    geode::define_hybrid_solid( module );

    geode::define_texture( module );
    geode::define_texture_manager( module );

    geode::define_vertex_set_builder( module );
    geode::define_graph_builder( module );
    geode::define_edged_curve_builder( module );
    geode::define_point_set_builder( module );
    geode::define_surface_mesh_builder( module );
    geode::define_surface_edges_builder( module );
    geode::define_polygonal_surface_builder( module );
    geode::define_triangulated_surface_builder( module );
    geode::define_solid_mesh_builder( module );
    geode::define_solid_edges_builder( module );
    geode::define_solid_facets_builder( module );
    geode::define_regular_grid_builder( module );
    geode::define_polyhedral_solid_builder( module );
    geode::define_tetrahedral_solid_builder( module );
    geode::define_hybrid_solid_builder( module );

    geode::define_convert_edged_curve( module );
    geode::define_convert_point_set( module );
    geode::define_convert_surface_mesh( module );
    geode::define_convert_solid_mesh( module );
    geode::define_euclidean_distance_transform( module );
    geode::define_repair_polygon_orientations( module );
    geode::define_geometrical_operations_on_mesh( module );
    geode::define_gradient_computation( module );
    geode::define_mesh_crs_helper( module );

    geode::define_vertex_set_io( module );
    geode::define_graph_io( module );
    geode::define_edged_curve_io( module );
    geode::define_light_regular_grid_io( module );
    geode::define_point_set_io( module );
    geode::define_polygonal_surface_io( module );
    geode::define_triangulated_surface_io( module );
    geode::define_polyhedral_solid_io( module );
    geode::define_tetrahedral_solid_io( module );
    geode::define_hybrid_solid_io( module );
    geode::define_regular_grid_io( module );
}