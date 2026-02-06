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

#include <geode/model/helpers/simplicial_brep_creator.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#include <geode/model/helpers/internal/simplicial_model_creator.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    std::vector< geode::Point3D > create_bbox_points(
        const geode::BoundingBox3D& bbox )
    {
        std::vector< geode::Point3D > points;
        points.reserve( 8 );
        points.emplace_back( bbox.min() );
        points.emplace_back( geode::Point3D{ { bbox.max().value( 0 ),
            bbox.min().value( 1 ), bbox.min().value( 2 ) } } );
        points.emplace_back( geode::Point3D{ { bbox.max().value( 0 ),
            bbox.max().value( 1 ), bbox.min().value( 2 ) } } );
        points.emplace_back( geode::Point3D{ { bbox.min().value( 0 ),
            bbox.max().value( 1 ), bbox.min().value( 2 ) } } );
        points.emplace_back( geode::Point3D{ { bbox.min().value( 0 ),
            bbox.min().value( 1 ), bbox.max().value( 2 ) } } );
        points.emplace_back( geode::Point3D{ { bbox.max().value( 0 ),
            bbox.min().value( 1 ), bbox.max().value( 2 ) } } );
        points.emplace_back( bbox.max() );
        points.emplace_back( geode::Point3D{ { bbox.min().value( 0 ),
            bbox.max().value( 1 ), bbox.max().value( 2 ) } } );
        return points;
    }

    std::vector< geode::CornerDefinition > create_corner_definitions()
    {
        return std::vector< geode::CornerDefinition >{ { 0 }, { 1 }, { 2 },
            { 3 }, { 4 }, { 5 }, { 6 }, { 7 } };
    }

    std::vector< geode::LineDefinition > create_line_definitions()
    {
        return std::vector< geode::LineDefinition >{
            { { 0, 1 } },
            { { 1, 2 } },
            { { 2, 3 } },
            { { 3, 0 } },
            { { 4, 5 } },
            { { 5, 6 } },
            { { 6, 7 } },
            { { 7, 4 } },
            { { 0, 4 } },
            { { 1, 5 } },
            { { 2, 6 } },
            { { 3, 7 } },
        };
    }

    std::vector< geode::SurfaceDefinition > create_surface_definitions()
    {
        return std::vector< geode::SurfaceDefinition >{
            { { 0, 1, 2, 3 }, { 0, 1, 2, 0, 2, 3 }, { 0, 1, 2, 3 }, {}, {} },
            { { 4, 5, 6, 7 }, { 0, 1, 2, 0, 2, 3 }, { 4, 5, 6, 7 }, {}, {} },
            { { 0, 1, 5, 4 }, { 0, 1, 2, 0, 2, 3 }, { 0, 9, 4, 8 }, {}, {} },
            { { 1, 2, 6, 5 }, { 0, 1, 2, 0, 2, 3 }, { 1, 10, 5, 9 }, {}, {} },
            { { 2, 3, 7, 6 }, { 0, 1, 2, 0, 2, 3 }, { 2, 11, 6, 10 }, {}, {} },
            { { 3, 0, 4, 7 }, { 0, 1, 2, 0, 2, 3 }, { 3, 8, 7, 11 }, {}, {} },
        };
    }

    std::vector< geode::BlockDefinition > create_block_definitions()
    {
        return std::vector< geode::BlockDefinition >{ { {}, {},
            { 0, 1, 2, 3, 4, 5 }, {}, {}, {} } };
    }
} // namespace

namespace geode
{
    class SimplicialBRepCreator::Impl
        : public internal::SimplicialModelCreator< BRep >
    {
    public:
        Impl( BRep& brep, std::vector< Point3D > unique_points )
            : internal::SimplicialModelCreator< BRep >(
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

        std::vector< uuid > create_blocks( absl::Span< const uuid > corners,
            absl::Span< const uuid > lines,
            absl::Span< const uuid > surfaces,
            absl::Span< const BlockDefinition > definitions )
        {
            const auto blocks = create_blocks( surfaces, definitions );
            for( const auto d : Indices{ definitions } )
            {
                const auto& definition = definitions[d];
                for( const auto internal : definition.internal_lines )
                {
                    builder().add_line_block_internal_relationship(
                        model().line( lines[internal] ),
                        model().block( blocks[d] ) );
                }
                for( const auto internal : definition.internal_corners )
                {
                    builder().add_corner_block_internal_relationship(
                        model().corner( corners[internal] ),
                        model().block( blocks[d] ) );
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

    std::vector< uuid > SimplicialBRepCreator::create_surfaces(
        absl::Span< const uuid > corners,
        absl::Span< const uuid > lines,
        absl::Span< const SurfaceDefinition > definitions )
    {
        return impl_->create_surfaces( corners, lines, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_blocks(
        absl::Span< const uuid > surfaces,
        absl::Span< const BlockDefinition > definitions )
    {
        return impl_->create_blocks( surfaces, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_blocks(
        absl::Span< const uuid > corners,
        absl::Span< const uuid > lines,
        absl::Span< const uuid > surfaces,
        absl::Span< const BlockDefinition > definitions )
    {
        return impl_->create_blocks( corners, lines, surfaces, definitions );
    }

    std::vector< uuid > SimplicialBRepCreator::create_model_boundaries(
        absl::Span< const uuid > surfaces,
        absl::Span< const BoundaryDefinition > definitions )
    {
        return impl_->create_model_boundaries( surfaces, definitions );
    }

    BRep create_model_from_bounding_box( const BoundingBox3D& box )
    {
        BRep brep;
        const auto points = create_bbox_points( box );
        SimplicialBRepCreator creator{ brep, points };
        const auto corners =
            creator.create_corners( create_corner_definitions() );
        const auto lines =
            creator.create_lines( corners, create_line_definitions() );
        const auto surfaces =
            creator.create_surfaces( lines, create_surface_definitions() );
        const auto blocks =
            creator.create_blocks( surfaces, create_block_definitions() );
        return brep;
    }
} // namespace geode