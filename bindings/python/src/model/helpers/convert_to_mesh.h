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

#include <geode/model/helpers/convert_to_mesh.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    void define_convert_to_mesh( pybind11::module& module )
    {
        module.def( "convert_section_into_curve", &convert_section_into_curve )
            .def( "convert_section_into_triangulated_surface",
                &convert_section_into_surface< TriangulatedSurface2D > )
            .def( "convert_section_into_polygonal_surface",
                &convert_section_into_surface< PolygonalSurface2D > )
            .def( "convert_section_into_curve_and_triangulated_surface",
                &convert_section_into_curve_and_surface<
                    TriangulatedSurface2D > )
            .def( "convert_section_into_curve_and_polygonal_surface",
                &convert_section_into_curve_and_surface< PolygonalSurface2D > )
            .def( "convert_brep_into_curve", &convert_brep_into_curve )
            .def( "convert_brep_into_triangulated_surface",
                &convert_brep_into_surface< TriangulatedSurface3D > )
            .def( "convert_brep_into_polygonal_surface",
                &convert_brep_into_surface< PolygonalSurface3D > )
            .def( "convert_brep_into_tetrahedral_solid",
                &convert_brep_into_solid< TetrahedralSolid3D > )
            .def( "convert_brep_into_polyhedral_solid",
                &convert_brep_into_solid< PolyhedralSolid3D > )
            .def( "convert_brep_into_hybrid_solid",
                &convert_brep_into_solid< HybridSolid3D > )
            .def( "convert_brep_into_curve_and_triangulated_surface",
                &convert_brep_into_curve_and_surface< TriangulatedSurface3D > )
            .def( "convert_brep_into_curve_and_polygonal_surface",
                &convert_brep_into_curve_and_surface< PolygonalSurface3D > )
            .def(
                "convert_brep_into_triangulated_surface_and_tetrahedral_solid",
                &convert_brep_into_surface_and_solid< TriangulatedSurface3D,
                    TetrahedralSolid3D > )
            .def( "convert_brep_into_triangulated_surface_and_polyhedral_solid",
                &convert_brep_into_surface_and_solid< TriangulatedSurface3D,
                    PolyhedralSolid3D > )
            .def(
                "convert_brep_into_triangulated_surface_and_hybridhedral_solid",
                &convert_brep_into_surface_and_solid< TriangulatedSurface3D,
                    HybridSolid3D > )
            .def( "convert_brep_into_polygonal_surface_and_tetrahedral_solid",
                &convert_brep_into_surface_and_solid< PolygonalSurface3D,
                    TetrahedralSolid3D > )
            .def( "convert_brep_into_polygonal_surface_and_polyhedral_solid",
                &convert_brep_into_surface_and_solid< PolygonalSurface3D,
                    PolyhedralSolid3D > )
            .def( "convert_brep_into_polygonal_surface_and_hybridhedral_solid",
                &convert_brep_into_surface_and_solid< PolygonalSurface3D,
                    HybridSolid3D > )
            .def( "convert_brep_into_curve_and_triangulated_surface_and_"
                  "tetrahedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    TriangulatedSurface3D, TetrahedralSolid3D > )
            .def( "convert_brep_into_curve_and_triangulated_surface_and_"
                  "polyhedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    TriangulatedSurface3D, PolyhedralSolid3D > )
            .def( "convert_brep_into_curve_and_triangulated_surface_and_"
                  "hybridhedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    TriangulatedSurface3D, HybridSolid3D > )
            .def( "convert_brep_into_curve_and_polygonal_surface_and_"
                  "tetrahedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    PolygonalSurface3D, TetrahedralSolid3D > )
            .def( "convert_brep_into_curve_and_polygonal_surface_and_"
                  "polyhedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    PolygonalSurface3D, PolyhedralSolid3D > )
            .def( "convert_brep_into_curve_and_polygonal_surface_and_"
                  "hybridhedral_solid",
                &convert_brep_into_curve_and_surface_and_solid<
                    PolygonalSurface3D, HybridSolid3D > );
    }
} // namespace geode