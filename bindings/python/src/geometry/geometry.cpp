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

#include <geode/geometry/common.h>

namespace geode
{
    void define_barycentric( pybind11::module& );
    void define_basic_objects( pybind11::module& );
    void define_bounding_box( pybind11::module& );
    void define_distance( pybind11::module& );
    void define_intersection( pybind11::module& );
    void define_intersection_detection( pybind11::module& );
    void define_mensuration( pybind11::module& );
    void define_nn_search( pybind11::module& );
    void define_perpendicular( pybind11::module& );
    void define_point( pybind11::module& );
    void define_points_sort( pybind11::module& );
    void define_position( pybind11::module& );
    void define_projection( pybind11::module& );
    void define_rotation( pybind11::module& );
    void define_sign( pybind11::module& );
    void define_vector( pybind11::module& );
    void define_frame( pybind11::module& );
    void define_coordinate_system( pybind11::module& module );
} // namespace geode

PYBIND11_MODULE( opengeode_py_geometry, module )
{
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for geometry";
    pybind11::class_< geode::OpenGeodeGeometryLibrary >(
        module, "OpenGeodeGeometryLibrary" )
        .def( "initialize", &geode::OpenGeodeGeometryLibrary::initialize );
    geode::define_barycentric( module );
    geode::define_basic_objects( module );
    geode::define_bounding_box( module );
    geode::define_distance( module );
    geode::define_intersection( module );
    geode::define_intersection_detection( module );
    geode::define_mensuration( module );
    geode::define_nn_search( module );
    geode::define_perpendicular( module );
    geode::define_point( module );
    geode::define_points_sort( module );
    geode::define_position( module );
    geode::define_projection( module );
    geode::define_rotation( module );
    geode::define_sign( module );
    geode::define_vector( module );
    geode::define_frame( module );
    geode::define_coordinate_system( module );
}