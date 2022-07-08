#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/helpers/hausdorff_distance.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/tests/common.h>

void test()
{
    const std::string filename( "../tests/data/iter1.og_tsf3d" );
    std::unique_ptr< geode::TriangulatedSurface3D > mesh_A =
        geode::load_triangulated_surface< 3 >( filename );
    std::unique_ptr< geode::TriangulatedSurface3D > mesh_B =
        geode::load_triangulated_surface< 3 >(
            "../tests/data/modified_iter1.og_tsf3d" );

    const auto hausdorff_distance =
        HAUSDORFF_DISTANCE::compute_two_sided_hausdorff_distance(
            *mesh_A, *mesh_B );
    DEBUG( hausdorff_distance );
}

OPENGEODE_TEST( "hausdorff_distance" )