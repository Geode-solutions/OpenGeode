/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/core/geode_vertex_set.h>

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
                  geode::TriangulatedSurface2D >
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
        struct PolymorphicBaseClass< geode::SolidMesh3D >
            : PolymorphicDerivedClasses< geode::PolyhedralSolid3D,
                  geode::TetrahedralSolid3D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::TetrahedralSolid3D >
            : PolymorphicDerivedClasses< geode::OpenGeodeTetrahedralSolid3D >
        {
        };
    } // namespace ext
} // namespace bitsery

namespace
{
    template < typename Serializer >
    void register_pcontext( geode::PContext& context )
    {
        geode::AttributeManager::register_attribute_type<
            geode::EdgesAroundVertex, Serializer >( context );
        geode::AttributeManager::register_attribute_type< geode::PolygonVertex,
            Serializer >( context );
        geode::AttributeManager::register_attribute_type< geode::PolygonEdge,
            Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronFacet, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronFacetVertex, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            geode::PolyhedronVertex, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 4 >, Serializer >( context );
        context.registerBasesList< Serializer >(
            bitsery::ext::PolymorphicClassesList< geode::VertexSet >{} );
    }
} // namespace

namespace geode
{
    void register_mesh_serialize_pcontext( PContext& context )
    {
        register_pcontext< Serializer >( context );
    }

    void register_mesh_deserialize_pcontext( PContext& context )
    {
        register_pcontext< Deserializer >( context );
    }
} // namespace geode
