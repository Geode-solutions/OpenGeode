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
    geode::OpenGeodeMesh::initialize();
    const auto initial_mesh_filename =
        absl::StrCat( geode::data_path, "Armadillo.og_tsf3d" );
    auto mesh_A =
        geode::load_triangulated_surface< 3 >( initial_mesh_filename );
    const auto simplified_mesh_filename =
        absl::StrCat( geode::data_path, "modified_Armadillo.og_tsf3d" );
    auto mesh_B =
        geode::load_triangulated_surface< 3 >( simplified_mesh_filename );

    const auto hausdorff_distance =
        geode::hausdorff_distance( *mesh_A, *mesh_B );
    DEBUG( hausdorff_distance );
}

OPENGEODE_TEST( "hausdorff_distance" )