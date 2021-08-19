/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/model/helpers/simplicial_brep_creator.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>

#include <geode/model/helpers/private/simplicial_model_creator.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    class SimplicialBRepCreator::Impl
        : public detail::SimplicialModelCreator< BRep, BRepBuilder, 3 >
    {
    public:
        Impl( BRep& brep, std::vector< Point3D > unique_points )
            : detail::SimplicialModelCreator< BRep, BRepBuilder, 3 >(
                brep, std::move( unique_points ) )
        {
        }

        std::vector< uuid > create_blocks( absl::Span< const uuid > surfaces,
            absl::Span< const BlockDefinition > definitions )
        {
            std::vector< uuid > blocks;
            blocks.reserve( definitions.size() );
            for( const auto& definition : definitions )
            {
                const auto& block_id =
                    builder().add_block( MeshFactory::default_impl(
                        TetrahedralSolid3D::type_name_static() ) );
                const auto& block = model().block( block_id );
                auto mesh_builder =
                    builder().block_mesh_builder< TetrahedralSolid3D >(
                        block_id );
                for( const auto vertex : definition.vertices )
                {
                    create_point( *mesh_builder, block, vertex );
                }
                for( index_t v = 0; v < definition.tetrahedra.size(); v += 4 )
                {
                    mesh_builder->create_tetrahedron(
                        { definition.tetrahedra[v],
                            definition.tetrahedra[v + 1],
                            definition.tetrahedra[v + 2],
                            definition.tetrahedra[v + 3] } );
                }
                mesh_builder->compute_polyhedron_adjacencies();
                blocks.push_back( block_id );
                for( const auto boundary : definition.boundaries )
                {
                    builder().add_surface_block_boundary_relationship(
                        model().surface( surfaces[boundary] ), block );
                }
                for( const auto internal : definition.internals )
                {
                    builder().add_surface_block_internal_relationship(
                        model().surface( surfaces[internal] ), block );
                }
            }
            return blocks;
        }

        std::vector< uuid > create_model_boundaries(
            absl::Span< const uuid > surfaces,
            absl::Span< const BoundaryDefinition > definitions )
        {
            std::vector< geode::uuid > boundaries;
            boundaries.reserve( definitions.size() );
            for( const auto& definition : definitions )
            {
                const auto& bonudary_id = builder().add_model_boundary();
                const auto& boundary = model().model_boundary( bonudary_id );
                boundaries.push_back( bonudary_id );
                for( const auto surface : definition.boundaries )
                {
                    builder().add_surface_in_model_boundary(
                        model().surface( surfaces[surface] ), boundary );
                }
            }
            return boundaries;
        }
    };

    SimplicialBRepCreator::SimplicialBRepCreator(
        BRep& brep, std::vector< Point3D > unique_points )
        : impl_{ brep, std::move( unique_points ) }
    {
    }

    SimplicialBRepCreator::~SimplicialBRepCreator() {}

    std::vector< uuid > SimplicialBRepCreator::create_corners(
        absl::Span< const CornerDefinition > definitions )
    {
        return impl_->create_corners( definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_lines(
        absl::Span< const uuid > corners,
        absl::Span< const LineDefinition > definitions )
    {
        return impl_->create_lines( corners, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_surfaces(
        absl::Span< const uuid > lines,
        absl::Span< const SurfaceDefinition > definitions )
    {
        return impl_->create_surfaces( lines, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_blocks(
        absl::Span< const uuid > surfaces,
        absl::Span< const BlockDefinition > definitions )
    {
        return impl_->create_blocks( surfaces, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_model_boundaries(
        absl::Span< const uuid > surfaces,
        absl::Span< const BoundaryDefinition > definitions )
    {
        return impl_->create_model_boundaries( surfaces, definitions );
    }
} // namespace geode