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
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
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
    geode::OpenGeodeMeshException::test(
        std::fabs( hausdorff_distance - 2.45089 ) < 1e-4,
        "Wrong Hausdorff distance: ", hausdorff_distance );

    const auto empty_mesh = geode::TriangulatedSurface3D::create();
    const auto throws_with_empty_mesh =
        []( const geode::TriangulatedSurface3D& lhs,
            const geode::TriangulatedSurface3D& rhs ) {
            try
            {
                const auto distance = geode::hausdorff_distance( lhs, rhs );
                geode::Logger::error(
                    "Hausdorff distance with empty mesh: ", distance );
            }
            catch( const geode::OpenGeodeException& )
            {
                return true;
            }
            return false;
        };
    geode::OpenGeodeMeshException::test(
        throws_with_empty_mesh( *mesh_A, *empty_mesh ),
        "Hausdorff distance to an empty mesh should throw" );
    geode::OpenGeodeMeshException::test(
        throws_with_empty_mesh( *empty_mesh, *mesh_A ),
        "Hausdorff distance from an empty mesh should throw" );
}

OPENGEODE_TEST( "hausdorff-distance" )