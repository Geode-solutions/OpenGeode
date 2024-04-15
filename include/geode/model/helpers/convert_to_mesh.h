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

    using MeshElementToIndexMapping = GenericMapping< MeshElement, index_t >;

    struct ModelToMeshMappings
    {
        MeshElementToIndexMapping mesh_element_mapping;
        BijectiveMapping< geode::index_t > unique_vertex_mapping;
    };

    static constexpr auto uuid_from_conversion_attribute_name =
        "uuid_from_conversion";
    using uuid_from_conversion_attribute_type = uuid;
    static constexpr auto unique_vertex_from_conversion_attribute_name =
        "unique_vertex_from_conversion";
    using unique_vertex_from_conversion_attribute_type = index_t;
    static constexpr auto MESH_ELEMENT_ATTRIBUTE_NAME =
        "mesh_elements_from_conversion";
    using mesh_elements_attribute_type = MeshElement;

    std::unique_ptr< EdgedCurve2D >
        opengeode_model_api OPENGEODE_MODEL_DEPRECATED
        convert_section_into_curve( const Section& section );

    std::tuple< std::unique_ptr< EdgedCurve2D >, ModelToMeshMappings >
        opengeode_model_api new_convert_section_into_curve(
            const Section& section );

    template < typename SurfaceType = SurfaceMesh2D >
    OPENGEODE_MODEL_DEPRECATED std::unique_ptr< SurfaceType >
        convert_section_into_surface( const Section& section );

    std::tuple< std::unique_ptr< SurfaceMesh2D >, ModelToMeshMappings >
        opengeode_model_api new_convert_section_into_surface(
            const Section& section );

    template < typename SurfaceType = SurfaceMesh2D >
    std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< SurfaceType > >
        convert_section_into_curve_and_surface( const Section& section );

    std::unique_ptr< EdgedCurve3D >
        opengeode_model_api OPENGEODE_MODEL_DEPRECATED convert_brep_into_curve(
            const BRep& brep );

    std::tuple< std::unique_ptr< EdgedCurve3D >, ModelToMeshMappings >
        opengeode_model_api new_convert_brep_into_curve( const BRep& brep );

    template < typename SurfaceType = SurfaceMesh3D >
    std::unique_ptr< SurfaceType > OPENGEODE_MODEL_DEPRECATED
        convert_brep_into_surface( const BRep& brep );

    std::tuple< std::unique_ptr< SurfaceMesh3D >, ModelToMeshMappings >
        new_convert_brep_into_surface( const BRep& brep );

    template < typename SolidType = SolidMesh3D >
    std::unique_ptr< SolidType >
        OPENGEODE_MODEL_DEPRECATED convert_brep_into_solid( const BRep& brep );

    std::tuple< std::unique_ptr< SolidMesh3D >, ModelToMeshMappings >
        new_convert_brep_into_solid( const BRep& brep );

    template < typename SurfaceType = SurfaceMesh3D >
    std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceType > >
        convert_brep_into_curve_and_surface( const BRep& brep );

    template < typename SurfaceType = SurfaceMesh3D,
        typename SolidType = SolidMesh3D >
    std::tuple< std::unique_ptr< SurfaceType >, std::unique_ptr< SolidType > >
        convert_brep_into_surface_and_solid( const BRep& brep );

    template < typename SurfaceType = SurfaceMesh3D,
        typename SolidType = SolidMesh3D >
    std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceType >,
        std::unique_ptr< SolidType > >
        convert_brep_into_curve_and_surface_and_solid( const BRep& brep );
} // namespace geode