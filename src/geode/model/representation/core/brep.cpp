/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/representation/core/brep.h>

#include <geode/basic/bounding_box.h>
#include <geode/basic/vector.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    BRep::LineBoundaryRange BRep::boundaries( const Line3D& line ) const
    {
        return { *this, relationships(), line };
    }

    BRep::LineBoundaryRange::LineBoundaryRange( const BRep& brep,
        const RelationshipManager& manager,
        const Line3D& line )
        : RelationshipManager::BoundaryRange( manager, line.id() ),
          brep_( brep )
    {
    }

    const Corner3D& BRep::LineBoundaryRange::operator*() const
    {
        return brep_.corner( RelationshipManager::BoundaryRange::operator*() );
    }

    BRep::SurfaceBoundaryRange BRep::boundaries(
        const Surface3D& surface ) const
    {
        return { *this, relationships(), surface };
    }

    BRep::SurfaceBoundaryRange::SurfaceBoundaryRange( const BRep& brep,
        const RelationshipManager& manager,
        const Surface3D& surface )
        : RelationshipManager::BoundaryRange( manager, surface.id() ),
          brep_( brep )
    {
    }

    const Line3D& BRep::SurfaceBoundaryRange::operator*() const
    {
        return brep_.line( RelationshipManager::BoundaryRange::operator*() );
    }

    BRep::BlockBoundaryRange BRep::boundaries( const Block3D& block ) const
    {
        return { *this, relationships(), block };
    }

    BRep::BlockBoundaryRange::BlockBoundaryRange( const BRep& brep,
        const RelationshipManager& manager,
        const Block3D& block )
        : RelationshipManager::BoundaryRange( manager, block.id() ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::BlockBoundaryRange::operator*() const
    {
        return brep_.surface( RelationshipManager::BoundaryRange::operator*() );
    }

    BRep::CornerIncidenceRange BRep::incidences( const Corner3D& corner ) const
    {
        return { *this, relationships(), corner };
    }

    BRep::CornerIncidenceRange::CornerIncidenceRange( const BRep& brep,
        const RelationshipManager& manager,
        const Corner3D& corner )
        : RelationshipManager::IncidenceRange( manager, corner.id() ),
          brep_( brep )
    {
    }

    const Line3D& BRep::CornerIncidenceRange::operator*() const
    {
        return brep_.line( RelationshipManager::IncidenceRange::operator*() );
    }

    BRep::LineIncidenceRange BRep::incidences( const Line3D& line ) const
    {
        return { *this, relationships(), line };
    }

    BRep::LineIncidenceRange::LineIncidenceRange( const BRep& brep,
        const RelationshipManager& manager,
        const Line3D& line )
        : RelationshipManager::IncidenceRange( manager, line.id() ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::LineIncidenceRange::operator*() const
    {
        return brep_.surface(
            RelationshipManager::IncidenceRange::operator*() );
    }

    BRep::SurfaceIncidenceRange BRep::incidences(
        const Surface3D& surface ) const
    {
        return { *this, relationships(), surface };
    }

    BRep::SurfaceIncidenceRange::SurfaceIncidenceRange( const BRep& brep,
        const RelationshipManager& manager,
        const Surface3D& surface )
        : RelationshipManager::IncidenceRange( manager, surface.id() ),
          brep_( brep )
    {
    }

    const Block3D& BRep::SurfaceIncidenceRange::operator*() const
    {
        return brep_.block( RelationshipManager::IncidenceRange::operator*() );
    }

    void BRep::compute_epsilon( double& epsilon ) const
    {
        BoundingBox3D box;
        for( const auto& surface : surfaces() )
        {
            const auto& mesh = surface.mesh();
            for( auto v : Range{ mesh.nb_vertices() } )
            {
                box.add_point( mesh.point( v ) );
            }
        }
        Vector3D diagonal{ box.min(), box.max() };
        epsilon = epsilon_scaling
                  * std::min( diagonal.value( 0 ),
                        std::min( diagonal.value( 1 ), diagonal.value( 2 ) ) );
    }
} // namespace geode
