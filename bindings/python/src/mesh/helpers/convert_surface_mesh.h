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

#include <pybind11/operators.h>

#include <geode/mesh/helpers/convert_surface_mesh.h>

namespace geode
{
    void define_convert_surface_mesh( pybind11::module& module )
    {
        module
            .def( "convert_surface_mesh_into_triangulated_surface2D",
                &convert_surface_mesh_into_triangulated_surface< 2 > )
            .def( "convert_surface_mesh_into_triangulated_surface3D",
                &convert_surface_mesh_into_triangulated_surface< 3 > )
            .def( "triangulate_surface_mesh2D",
                ( void ( * )( SurfaceMesh< 2 >& ) )
                    & triangulate_surface_mesh< 2 > )
            .def( "triangulate_surface_mesh3D",
                ( void ( * )( SurfaceMesh< 3 >& ) )
                    & triangulate_surface_mesh< 3 > )
            .def( "convert_surface_mesh2d_into_3d",
                &convert_surface_mesh2d_into_3d )
            .def( "convert_surface_mesh3d_into_2d",
                &convert_surface_mesh3d_into_2d )
            .def( "convert_polygonal_surface2d_into_3d",
                &convert_polygonal_surface2d_into_3d )
            .def( "convert_polygonal_surface3d_into_2d",
                &convert_polygonal_surface3d_into_2d )
            .def( "convert_triangulated_surface2d_into_3d",
                &convert_triangulated_surface2d_into_3d )
            .def( "convert_triangulated_surface3d_into_2d",
                &convert_triangulated_surface3d_into_2d )
            .def( "merge_surface_mesh2D", &merge_surface_mesh< 2 > )
            .def( "merge_surface_mesh3D", &merge_surface_mesh< 3 > )
            .def( "merge_surface_meshes2D", &merge_surface_meshes< 2 > )
            .def( "merge_surface_meshes3D", &merge_surface_meshes< 3 > );
    }
} // namespace geode