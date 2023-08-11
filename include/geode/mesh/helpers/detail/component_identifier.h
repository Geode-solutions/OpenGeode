/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <absl/container/fixed_array.h>

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    ALIAS_3D( SolidMesh );
    class Graph;
} // namespace geode

namespace geode
{
    namespace detail
    {
        class GraphIdentifier
        {
        public:
            GraphIdentifier( const Graph& graph );

            ~GraphIdentifier();

            index_t identify_vertices();

            index_t vertex_identifier( index_t vertex_id ) const;

            absl::FixedArray< std::vector< index_t > >
                identified_connected_vertices() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        template < index_t dimension >
        class EdgedCurveIdentifier
        {
        public:
            EdgedCurveIdentifier( const EdgedCurve< dimension >& curve );

            ~EdgedCurveIdentifier();

            index_t identify_edges();

            index_t edge_identifier( index_t edge_id ) const;

            absl::FixedArray< std::vector< index_t > >
                identified_connected_edges() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_1D_AND_2D_AND_3D( EdgedCurveIdentifier );

        template < index_t dimension >
        class SurfaceIdentifier
        {
        public:
            SurfaceIdentifier( const SurfaceMesh< dimension >& surface );

            ~SurfaceIdentifier();

            index_t identify_polygons();

            index_t polygon_identifier( index_t polygon ) const;

            absl::FixedArray< std::vector< index_t > >
                identified_connected_polygons() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_2D_AND_3D( SurfaceIdentifier );

        class opengeode_mesh_api SolidIdentifier
        {
        public:
            SolidIdentifier( const SolidMesh3D& solid );

            ~SolidIdentifier();

            index_t identify_polyhedra();

            index_t polyhedron_identifier( index_t polyhedron ) const;

            absl::FixedArray< std::vector< index_t > >
                identified_connected_polyhedra() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
    } // namespace detail
} // namespace geode
