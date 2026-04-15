/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/helpers/mesh_statistics.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
} // namespace

namespace geode
{
    template < index_t dimension >
    [[nodiscard]] MeshMetric compute_surface_metrics(
        const SurfaceMesh< dimension >& mesh )
    {
        MeshMetric result;
        index_t nb_edges{ 0 };
        for( const auto polygon_id : Range{ mesh.nb_polygons() } )
        {
            for( const auto edge_id :
                LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                const PolygonEdge polygon_edge{ polygon_id, edge_id };
                if( const auto adjacent =
                        mesh.polygon_adjacent( polygon_edge ) )
                {
                    if( adjacent.value() < polygon_id )
                    {
                        continue;
                    }
                }
                const auto length = mesh.edge_length( polygon_edge );
                result.max_mesh_size = std::max( result.max_mesh_size, length );
                result.min_mesh_size = std::min( result.min_mesh_size, length );
                result.mean_mesh_size += length;
                nb_edges++;
            }
        }
        if( nb_edges != 0 )
        {
            result.mean_mesh_size /= nb_edges;
        }
        return result;
    }

    template MeshMetric compute_surface_metrics< 2 >( const SurfaceMesh< 2 >& );
    template MeshMetric compute_surface_metrics< 3 >( const SurfaceMesh< 3 >& );
} // namespace geode
