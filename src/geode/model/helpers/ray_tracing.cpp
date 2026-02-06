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

#include <geode/model/helpers/ray_tracing.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/aabb.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_edged_curve_helpers.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    const std::array< geode::Vector2D, 8 > directions_2D = {
        { geode::Vector2D{ { 1., 0. } }, geode::Vector2D{ { 0., 1. } },
            geode::Vector2D{ { 0.1, 1. } }, geode::Vector2D{ { 0.3, 1. } },
            geode::Vector2D{ { 0.5, 1. } }, geode::Vector2D{ { 0., 0.1 } },
            geode::Vector2D{ { 0., 0.3 } }, geode::Vector2D{ { 0., 0.5 } } }
    };

    const std::array< geode::Vector3D, 12 > directions_3D = { { geode::Vector3D{
                                                                    { 1., 0.,
                                                                        0. } },
        geode::Vector3D{ { 1., 0., 0.1 } }, geode::Vector3D{ { 1., 0., 0.3 } },
        geode::Vector3D{ { 1., 0., 0.5 } }, geode::Vector3D{ { 0., 1., 0. } },
        geode::Vector3D{ { 0.1, 1., 0. } }, geode::Vector3D{ { 0.3, 1., 0. } },
        geode::Vector3D{ { 0.5, 1., 0. } }, geode::Vector3D{ { 0., 0., 1. } },
        geode::Vector3D{ { 0., 0.1, 1. } }, geode::Vector3D{ { 0., 0.3, 1. } },
        geode::Vector3D{ { 0., 0.5, 1. } } } };

    std::vector< geode::RayTracing2D::EdgeDistance >
        find_intersections_with_boundary( const geode::Ray2D& ray,
            const geode::EdgedCurve2D& curve,
            const geode::AABBTree2D& aabb )
    {
        geode::RayTracing2D ray_tracing{ curve, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, ray_tracing );
        return ray_tracing.all_intersections();
    }

    std::optional< geode::index_t > count_real_intersections_with_boundary(
        const geode::Ray2D& ray,
        const geode::EdgedCurve2D& curve,
        const geode::AABBTree2D& aabb )
    {
        geode::index_t nb_intersections{ 0 };
        const auto tracing =
            find_intersections_with_boundary( ray, curve, aabb );
        for( const auto& intersection : tracing )
        {
            if( intersection.position != geode::POSITION::inside )
            {
                return std::nullopt;
            }
            if( std::fabs( intersection.distance ) <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            else
            {
                nb_intersections += 1;
            }
        }
        return nb_intersections;
    }

    std::vector< geode::RayTracing3D::PolygonDistance >
        find_intersections_with_boundaries( const geode::Ray3D& ray,
            const geode::SurfaceMesh3D& surface,
            const geode::AABBTree3D& aabb )
    {
        geode::RayTracing3D ray_tracing{ surface, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, ray_tracing );
        return ray_tracing.all_intersections();
    }

    std::optional< geode::index_t > count_real_intersections_with_boundaries(
        const geode::Ray3D& ray,
        const geode::SurfaceMesh3D& surface,
        const geode::AABBTree3D& aabb )
    {
        geode::index_t nb_intersections{ 0 };
        const auto tracing =
            find_intersections_with_boundaries( ray, surface, aabb );
        for( const auto& intersection : tracing )
        {
            if( intersection.position != geode::POSITION::inside )
            {
                return std::nullopt;
            }
            if( std::fabs( intersection.distance ) <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            else
            {
                nb_intersections += 1;
            }
        }
        return nb_intersections;
    }
} // namespace

namespace geode
{
    class SectionRayTracing::Impl
    {
    public:
        Impl( const Section& section ) : section_( section ) {}

        bool is_point_inside_surface(
            const Point2D& point, const Surface2D& surface )
        {
            for( const auto& direction : directions_2D )
            {
                const Ray2D ray{ direction, point };
                index_t nb_intersections{ 0 };
                bool could_determine{ true };
                for( const auto& line : section_.boundaries( surface ) )
                {
                    auto intersections = count_real_intersections_with_boundary(
                        ray, line.mesh(), line_aabb( line ) );
                    if( !intersections.has_value() )
                    {
                        could_determine = false;
                        break;
                    }
                    nb_intersections += intersections.value();
                }
                if( could_determine )
                {
                    return ( nb_intersections % 2 == 1 );
                }
            }
            throw OpenGeodeException{
                "Cannot determine the point is inside the surface 2D or not "
                "(ambiguous intersection with rays)."
            };
        }

        std::optional< uuid > surface_containing_point( const Point2D& point )
        {
            for( const auto& surface : section_.surfaces() )
            {
                if( is_point_inside_surface( point, surface ) )
                {
                    return surface.id();
                }
            }
            return std::nullopt;
        }

    private:
        const AABBTree2D& line_aabb( const Line2D& line )
        {
            if( aabb_trees_.contains( line.id() ) )
            {
                return aabb_trees_.at( line.id() );
            }
            return aabb_trees_
                .emplace( line.id(), create_aabb_tree( line.mesh() ) )
                .first->second;
        }

    private:
        const Section& section_;
        absl::flat_hash_map< uuid, AABBTree2D > aabb_trees_;
    };

    SectionRayTracing::SectionRayTracing( const Section& section )
        : impl_{ section }
    {
    }

    SectionRayTracing::~SectionRayTracing() = default;

    bool SectionRayTracing::is_point_inside_surface(
        const Point2D& point, const Surface2D& surface )
    {
        return impl_->is_point_inside_surface( point, surface );
    }

    std::optional< uuid > SectionRayTracing::surface_containing_point(
        const Point2D& point )
    {
        return impl_->surface_containing_point( point );
    }

    class BRepRayTracing::Impl
    {
    public:
        Impl( const BRep& brep ) : brep_( brep ) {}

        BoundarySurfaceIntersections find_intersections_with_boundaries(
            const InfiniteLine3D& infinite_line, const Block3D& block )
        {
            BoundarySurfaceIntersections result;
            for( const auto& surface : brep_.boundaries( block ) )
            {
                const auto& aabb = surface_aabb( surface );
                RayTracing3D ray_tracing{ surface.mesh(), aabb.bounding_box(),
                    infinite_line };
                aabb.compute_line_element_bbox_intersections(
                    infinite_line, ray_tracing );
                result[surface.id()] = ray_tracing.all_intersections();
            }
            return result;
        }

        bool is_point_inside_block( const Point3D& point, const Block3D& block )
        {
            for( const auto& direction : directions_3D )
            {
                const Ray3D ray{ direction, point };
                index_t nb_intersections{ 0 };
                bool could_determine{ true };
                for( const auto& surface : brep_.boundaries( block ) )
                {
                    auto intersections =
                        count_real_intersections_with_boundaries(
                            ray, surface.mesh(), surface_aabb( surface ) );
                    if( !intersections.has_value() )
                    {
                        could_determine = false;
                        break;
                    }
                    nb_intersections += intersections.value();
                }
                if( could_determine )
                {
                    return ( nb_intersections % 2 == 1 );
                }
            }
            throw OpenGeodeException{
                "Cannot determine the point is inside the block or not "
                "(ambiguous intersection with rays)."
            };
        }

        std::optional< uuid > block_containing_point( const Point3D& point )
        {
            for( const auto& block : brep_.blocks() )
            {
                if( is_point_inside_block( point, block ) )
                {
                    return block.id();
                }
            }
            return std::nullopt;
        }

    private:
        const AABBTree3D& surface_aabb( const Surface3D& surface )
        {
            if( aabb_trees_.contains( surface.id() ) )
            {
                return aabb_trees_.at( surface.id() );
            }
            return aabb_trees_
                .emplace( surface.id(), create_aabb_tree( surface.mesh() ) )
                .first->second;
        }

    private:
        const BRep& brep_;
        absl::flat_hash_map< uuid, AABBTree3D > aabb_trees_;
    };

    BRepRayTracing::BRepRayTracing( const BRep& brep ) : impl_{ brep } {}

    BRepRayTracing::~BRepRayTracing() = default;

    auto BRepRayTracing::find_intersections_with_boundaries(
        const InfiniteLine3D& infinite_line, const Block3D& block )
        -> BoundarySurfaceIntersections
    {
        return impl_->find_intersections_with_boundaries(
            infinite_line, block );
    }

    bool BRepRayTracing::is_point_inside_block(
        const Point3D& point, const Block3D& block )
    {
        return impl_->is_point_inside_block( point, block );
    }

    std::optional< uuid > BRepRayTracing::block_containing_point(
        const Point3D& point )
    {
        return impl_->block_containing_point( point );
    }

    bool is_point_inside_closed_surface( const Point3D& point,
        const SurfaceMesh3D& surface,
        const AABBTree3D& surface_aabb )
    {
        if( !surface_aabb.bounding_box().contains( point ) )
        {
            return false;
        }
        for( const auto& direction : directions_3D )
        {
            const Ray3D ray{ direction, point };
            auto nb_intersections = count_real_intersections_with_boundaries(
                ray, surface, surface_aabb );
            if( nb_intersections.has_value() )
            {
                return ( nb_intersections.value() % 2 == 1 );
            }
        }
        throw OpenGeodeException{ "Cannot determine the point is inside "
                                  "the closed surface 3D or not "
                                  "(ambiguous intersection with rays)." };
    }
} // namespace geode
