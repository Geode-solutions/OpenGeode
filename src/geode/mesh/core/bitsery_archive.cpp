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

#include <geode/mesh/core/bitsery_archive.h>

#include <bitsery/brief_syntax/vector.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/cached_value.h>

#include <geode/mesh/core/attribute_coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system.h>
#include <geode/mesh/core/geode/geode_edged_curve.h>
#include <geode/mesh/core/geode/geode_graph.h>
#include <geode/mesh/core/geode/geode_hybrid_solid.h>
#include <geode/mesh/core/geode/geode_point_set.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/core/geode/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode/geode_regular_grid_solid.h>
#include <geode/mesh/core/geode/geode_regular_grid_surface.h>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode/geode_triangulated_surface.h>
#include <geode/mesh/core/geode/geode_vertex_set.h>
#include <geode/mesh/core/mesh_element.h>
#include <geode/mesh/core/private/solid_mesh_impl.h>
#include <geode/mesh/core/private/surface_mesh_impl.h>

namespace bitsery
{
    namespace ext
    {
        template <>
        struct PolymorphicBaseClass< geode::VertexSet >
            : PolymorphicDerivedClasses< geode::Graph,
                  geode::PointSet2D,
                  geode::PointSet3D,
                  geode::SurfaceMesh2D,
                  geode::SurfaceMesh3D,
                  geode::SolidMesh3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::Graph >
            : PolymorphicDerivedClasses< geode::OpenGeodeGraph,
                  geode::EdgedCurve2D,
                  geode::EdgedCurve3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::PointSet2D >
            : PolymorphicDerivedClasses< geode::OpenGeodePointSet2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::PointSet3D >
            : PolymorphicDerivedClasses< geode::OpenGeodePointSet3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::EdgedCurve2D >
            : PolymorphicDerivedClasses< geode::OpenGeodeEdgedCurve2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::EdgedCurve3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeEdgedCurve3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::PolygonalSurface2D >
            : PolymorphicDerivedClasses< geode::OpenGeodePolygonalSurface2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::SurfaceMesh2D >
            : PolymorphicDerivedClasses< geode::PolygonalSurface2D,
                  geode::TriangulatedSurface2D,
                  geode::RegularGrid2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::SurfaceMesh3D >
            : PolymorphicDerivedClasses< geode::PolygonalSurface3D,
                  geode::TriangulatedSurface3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::PolygonalSurface3D >
            : PolymorphicDerivedClasses< geode::OpenGeodePolygonalSurface3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::TriangulatedSurface2D >
            : PolymorphicDerivedClasses< geode::OpenGeodeTriangulatedSurface2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::TriangulatedSurface3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeTriangulatedSurface3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::PolyhedralSolid3D >
            : PolymorphicDerivedClasses< geode::OpenGeodePolyhedralSolid3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::RegularGrid2D >
            : PolymorphicDerivedClasses< geode::OpenGeodeRegularGrid2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::SolidMesh3D >
            : PolymorphicDerivedClasses< geode::PolyhedralSolid3D,
                  geode::TetrahedralSolid3D,
                  geode::HybridSolid3D,
                  geode::RegularGrid3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::HybridSolid3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeHybridSolid3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::TetrahedralSolid3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeTetrahedralSolid3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::RegularGrid3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeRegularGrid3D >
        {
        };

        BITSERY_CLASS_NAME( geode::VertexSet, "VertexSet" );
        BITSERY_CLASS_NAME( geode::Graph, "Graph" );
        BITSERY_CLASS_NAME( geode::OpenGeodeGraph, "OpenGeodeGraph" );
        BITSERY_CLASS_NAME( geode::EdgedCurve2D, "EdgedCurve2D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeEdgedCurve2D, "OpenGeodeEdgedCurve2D" );
        BITSERY_CLASS_NAME( geode::EdgedCurve3D, "EdgedCurve3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeEdgedCurve3D, "OpenGeodeEdgedCurve3D" );
        BITSERY_CLASS_NAME( geode::PointSet2D, "PointSet2D" );
        BITSERY_CLASS_NAME( geode::OpenGeodePointSet2D, "OpenGeodePointSet2D" );
        BITSERY_CLASS_NAME( geode::PointSet3D, "PointSet3D" );
        BITSERY_CLASS_NAME( geode::OpenGeodePointSet3D, "OpenGeodePointSet3D" );
        BITSERY_CLASS_NAME( geode::SurfaceMesh2D, "SurfaceMesh2D" );
        BITSERY_CLASS_NAME( geode::SurfaceMesh3D, "SurfaceMesh3D" );
        BITSERY_CLASS_NAME( geode::PolygonalSurface2D, "PolygonalSurface2D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodePolygonalSurface2D, "OpenGeodePolygonalSurface2D" );
        BITSERY_CLASS_NAME( geode::PolygonalSurface3D, "PolygonalSurface3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodePolygonalSurface3D, "OpenGeodePolygonalSurface3D" );
        BITSERY_CLASS_NAME(
            geode::TriangulatedSurface2D, "TriangulatedSurface2D" );
        BITSERY_CLASS_NAME( geode::OpenGeodeTriangulatedSurface2D,
            "OpenGeodeTriangulatedSurface2D" );
        BITSERY_CLASS_NAME(
            geode::TriangulatedSurface3D, "TriangulatedSurface3D" );
        BITSERY_CLASS_NAME( geode::OpenGeodeTriangulatedSurface3D,
            "OpenGeodeTriangulatedSurface3D" );
        BITSERY_CLASS_NAME( geode::RegularGrid2D, "RegularGrid2D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeRegularGrid2D, "OpenGeodeRegularGrid2D" );
        BITSERY_CLASS_NAME( geode::SolidMesh3D, "SolidMesh3D" );
        BITSERY_CLASS_NAME( geode::PolyhedralSolid3D, "PolyhedralSolid3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodePolyhedralSolid3D, "OpenGeodePolyhedralSolid3D" );
        BITSERY_CLASS_NAME( geode::TetrahedralSolid3D, "TetrahedralSolid3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeTetrahedralSolid3D, "OpenGeodeTetrahedralSolid3D" );
        BITSERY_CLASS_NAME( geode::HybridSolid3D, "HybridSolid3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeHybridSolid3D, "OpenGeodeHybridSolid3D" );
        BITSERY_CLASS_NAME( geode::RegularGrid3D, "RegularGrid3D" );
        BITSERY_CLASS_NAME(
            geode::OpenGeodeRegularGrid3D, "OpenGeodeRegularGrid3D" );

        BITSERY_CLASS_NAME(
            geode::CoordinateReferenceSystem1D, "CoordinateReferenceSystem1D" );
        BITSERY_CLASS_NAME(
            geode::CoordinateReferenceSystem2D, "CoordinateReferenceSystem2D" );
        BITSERY_CLASS_NAME(
            geode::CoordinateReferenceSystem3D, "CoordinateReferenceSystem3D" );
        BITSERY_CLASS_NAME( geode::AttributeCoordinateReferenceSystem1D,
            "AttributeCoordinateReferenceSystem1D" );
        BITSERY_CLASS_NAME( geode::AttributeCoordinateReferenceSystem2D,
            "AttributeCoordinateReferenceSystem2D" );
        BITSERY_CLASS_NAME( geode::AttributeCoordinateReferenceSystem3D,
            "AttributeCoordinateReferenceSystem3D" );
    } // namespace ext
} // namespace bitsery

namespace
{
    template < typename Serializer >
    void register_mesh_pcontext( geode::PContext& context )
    {
        geode::AttributeManager::register_attribute_type<
            geode::EdgesAroundVertex, Serializer >(
            context, "EdgesAroundVertex" );
        geode::AttributeManager::register_attribute_type< geode::PolygonVertex,
            Serializer >( context, "PolygonVertex" );
        geode::AttributeManager::register_attribute_type< geode::PolygonEdge,
            Serializer >( context, "PolygonEdge" );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronFacet, Serializer >( context, "PolyhedronFacet" );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronFacetVertex, Serializer >(
            context, "PolyhedronFacetVertex" );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronVertex, Serializer >(
            context, "PolyhedronVertex" );
        geode::AttributeManager::register_attribute_type<
            geode::CachedValue< geode::detail::PolygonsAroundVertexImpl >,
            Serializer >( context, "CachedPolygonsAroundVertexImpl" );
        geode::AttributeManager::register_attribute_type<
            geode::CachedValue< geode::detail::PolyhedraAroundVertexImpl >,
            Serializer >( context, "CachedPolyhedraAroundVertexImpl" );
        geode::AttributeManager::register_attribute_type<
            geode::HybridSolid3D::Type, Serializer >(
            context, "HybridSolidType" );
        geode::AttributeManager::register_attribute_type< geode::MeshElement,
            Serializer >( context, "SingleMeshElement" );
        geode::AttributeManager::register_attribute_type<
            std::vector< geode::MeshElement >, Serializer >(
            context, "MeshElement" );
        geode::CoordinateReferenceSystem1D::
            register_coordinate_reference_system_type<
                geode::AttributeCoordinateReferenceSystem1D, Serializer >(
                context, "AttributeCoordinateReferenceSystem1D" );
        geode::CoordinateReferenceSystem2D::
            register_coordinate_reference_system_type<
                geode::AttributeCoordinateReferenceSystem2D, Serializer >(
                context, "AttributeCoordinateReferenceSystem2D" );
        geode::CoordinateReferenceSystem3D::
            register_coordinate_reference_system_type<
                geode::AttributeCoordinateReferenceSystem3D, Serializer >(
                context, "AttributeCoordinateReferenceSystem3D" );
        context.registerBasesList< Serializer >(
            bitsery::ext::PolymorphicClassesList< geode::VertexSet >{} );
    }
} // namespace

namespace geode
{
    void register_mesh_deserialize_pcontext( PContext& context )
    {
        register_mesh_pcontext< Deserializer >( context );
    }

    void register_mesh_serialize_pcontext( PContext& context )
    {
        register_mesh_pcontext< Serializer >( context );
    }
} // namespace geode
