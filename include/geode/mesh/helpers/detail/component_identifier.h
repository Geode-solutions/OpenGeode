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
        class opengeode_mesh_api GraphIdentifier
        {
            OPENGEODE_DISABLE_COPY_AND_MOVE( GraphIdentifier );

        public:
            explicit GraphIdentifier( const Graph& graph );

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
            OPENGEODE_DISABLE_COPY_AND_MOVE( EdgedCurveIdentifier );

        public:
            explicit EdgedCurveIdentifier(
                const EdgedCurve< dimension >& curve );

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
            OPENGEODE_DISABLE_COPY_AND_MOVE( SurfaceIdentifier< dimension > );

        public:
            explicit SurfaceIdentifier(
                const SurfaceMesh< dimension >& surface );

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
            OPENGEODE_DISABLE_COPY_AND_MOVE( SolidIdentifier );

        public:
            explicit SolidIdentifier( const SolidMesh3D& solid );

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
