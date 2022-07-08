#include <geode/mesh/helpers/hausdorff_distance.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/mesh/helpers/aabb_surface_helpers.h>

namespace HAUSDORFF_DISTANCE
{

    double compute_one_sided_hausdorff_distance(
        const geode::TriangulatedSurface< 3 >& mesh_A,
        const geode::TriangulatedSurface< 3 >& mesh_B )
    {
        const auto mesh_B_tree = geode::create_aabb_tree( mesh_B );
        const geode::DistanceToTriangle< 3 > distance_action( mesh_B );
        double min_dist;
        for( const geode::index_t v : geode::Range( mesh_A.nb_vertices() ) )
        {
            const geode::Point3D& query = mesh_A.point( v );
            const auto toto =
                mesh_B_tree.closest_element_box( query, distance_action );
            const auto distance = std::get< 2 >( toto );
            if( v == 0 || distance > min_dist )
            {
                min_dist = distance;
            }
        }
        return min_dist;
    }

    double compute_two_sided_hausdorff_distance(
        const geode::TriangulatedSurface< 3 >& mesh_A,
        const geode::TriangulatedSurface< 3 >& mesh_B )
    {
        return (
            std::max( compute_one_sided_hausdorff_distance( mesh_A, mesh_B ),
                compute_one_sided_hausdorff_distance( mesh_B, mesh_A ) ) );
    }
} // namespace HAUSDORFF_DISTANCE
