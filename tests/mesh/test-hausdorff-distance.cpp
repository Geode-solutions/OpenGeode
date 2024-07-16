#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>
#include <geode/mesh/helpers/hausdorff_distance.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/tests/common.hpp>

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    const auto initial_mesh_filename =
        absl::StrCat( geode::DATA_PATH, "Armadillo.og_tsf3d" );
    const auto mesh_A =
        geode::load_triangulated_surface< 3 >( initial_mesh_filename );
    const auto simplified_mesh_filename =
        absl::StrCat( geode::DATA_PATH, "modified_Armadillo.og_tsf3d" );
    const auto mesh_B =
        geode::load_triangulated_surface< 3 >( simplified_mesh_filename );

    const auto hausdorff_distance =
        geode::hausdorff_distance( *mesh_A, *mesh_B );
    DEBUG( hausdorff_distance );
}

OPENGEODE_TEST( "hausdorff-distance" )