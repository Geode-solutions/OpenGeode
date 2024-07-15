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

#pragma once

#include <geode/basic/mapping.h>

#include <geode/mesh/core/mesh_element.h>

#include <geode/model/common.h>

#include <memory>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    ALIAS_2D_AND_3D( EdgedCurve );
    ALIAS_2D_AND_3D( SurfaceMesh );
    ALIAS_3D( SolidMesh );
    class BRep;
    class Section;
    struct uuid;
} // namespace geode

namespace geode
{
    struct ModelToMeshMappings
    {
        using MeshElementToIndexMapping =
            GenericMapping< MeshElement, index_t >;

        MeshElementToIndexMapping corner_vertices_mapping;
        MeshElementToIndexMapping line_edges_mapping;
        MeshElementToIndexMapping surface_polygons_mapping;
        MeshElementToIndexMapping solid_polyhedra_mapping;
        BijectiveMapping< geode::index_t > unique_vertices_mapping;
    };

    std::tuple< std::unique_ptr< EdgedCurve2D >, ModelToMeshMappings >
        opengeode_model_api convert_section_into_curve(
            const Section& section );

    std::tuple< std::unique_ptr< SurfaceMesh2D >, ModelToMeshMappings >
        opengeode_model_api convert_section_into_surface(
            const Section& section );

    std::tuple< std::unique_ptr< EdgedCurve3D >, ModelToMeshMappings >
        opengeode_model_api convert_brep_into_curve( const BRep& brep );

    std::tuple< std::unique_ptr< SurfaceMesh3D >, ModelToMeshMappings >
        opengeode_model_api convert_brep_into_surface( const BRep& brep );

    std::tuple< std::unique_ptr< SolidMesh3D >, ModelToMeshMappings >
        opengeode_model_api convert_brep_into_solid( const BRep& brep );
} // namespace geode