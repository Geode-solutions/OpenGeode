/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/model/helpers/convert_brep_section.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/geode/geode_edged_curve_builder.h>
#include <geode/mesh/builder/geode/geode_hybrid_solid_builder.h>
#include <geode/mesh/builder/geode/geode_point_set_builder.h>
#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.h>
#include <geode/mesh/core/geode/geode_hybrid_solid.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/helpers/convert_edged_curve.h>
#include <geode/mesh/helpers/convert_point_set.h>
#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/model/helpers/model_concatener.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/mixin/core/vertex_identifier.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename ModelFrom, typename BuilderTo >
    geode::ModelCopyMapping copy_components(
        const ModelFrom& from, BuilderTo& builder_to )
    {
        geode::ModelCopyMapping mappings;
        const auto dimension = BuilderTo::dim;
        geode::detail::copy_corner_components( from, builder_to,
            mappings[geode::Corner< dimension >::component_type_static()] );
        geode::detail::copy_line_components( from, builder_to,
            mappings[geode::Line< dimension >::component_type_static()] );
        geode::detail::copy_surface_components( from, builder_to,
            mappings[geode::Surface< dimension >::component_type_static()] );
        geode::detail::copy_model_boundary_components( from, builder_to,
            mappings
                [geode::ModelBoundary< dimension >::component_type_static()] );
        builder_to.copy_relationships( mappings, from );
        return mappings;
    }

    template < typename ModelFrom, typename BuilderTo >
    void copy_unique_vertices( const ModelFrom& from,
        BuilderTo& builder_to,
        const geode::ModelCopyMapping& mappings )
    {
        const auto first_new_unique_vertex_id =
            builder_to.create_unique_vertices( from.nb_unique_vertices() );
        geode::detail::copy_vertex_identifier_components(
            from, builder_to, first_new_unique_vertex_id, mappings );
    }

    class SectionExtruder
    {
    public:
        SectionExtruder( const geode::Section& section )
            : section_( section ), brep_builder_{ brep_ }
        {
        }

        geode::BRep extrude( const geode::SectionExtruderOptions& options )
        {
            initialize_extrusion( options );
            create_lines();
            create_surfaces();
            create_blocks();
            return std::move( brep_ );
        }

    private:
        void initialize_extrusion(
            const geode::SectionExtruderOptions& options )
        {
            auto slice0_conversion_output = convert_section_into_brep(
                section_, options.axis_to_extrude, options.min_coordinate );
            section_slice0_mapping_ =
                std::move( std::get< 1 >( slice0_conversion_output ) );
            brep_ = std::move( std::get< 0 >( slice0_conversion_output ) );
            add_surfaces_in_model_boundaries();

            auto slice1_conversion_output = convert_section_into_brep(
                section_, options.axis_to_extrude, options.max_coordinate );
            section_slice1_mapping_ =
                std::move( std::get< 1 >( slice1_conversion_output ) );
            geode::BRepConcatener brep_concatener{ brep_ };
            slice1_brep_mapping_ = brep_concatener.concatenate(
                std::get< 0 >( slice1_conversion_output ) );
            add_surfaces_in_model_boundaries();
        }

        void add_surfaces_in_model_boundaries()
        {
            const auto model_boundary_id = brep_builder_.add_model_boundary();
            bool surface_added{ false };
            const auto& model_boundary =
                brep_.model_boundary( model_boundary_id );
            for( const auto& surface : brep_.surfaces() )
            {
                if( brep_.nb_collections( surface.id() ) > 0 )
                {
                    continue;
                }
                brep_builder_.add_surface_in_model_boundary(
                    surface, model_boundary );
                surface_added = true;
            }
            if( !surface_added )
            {
                brep_builder_.remove_model_boundary( model_boundary );
            }
        }

        geode::uuid section_slice0_brep_mapping(
            const geode::ComponentType& brep_component_type,
            geode::uuid section_component )
        {
            return section_slice0_mapping_.at( brep_component_type )
                .in2out( section_component );
        }

        geode::uuid section_slice1_brep_mapping(
            const geode::ComponentType& brep_component_type,
            geode::uuid section_component )
        {
            return slice1_brep_mapping_.at( brep_component_type )
                .in2out( section_slice1_mapping_.at( brep_component_type )
                             .in2out( section_component ) );
        }

        void create_lines()
        {
            for( const auto& corner : section_.corners() )
            {
                extrude_corner( corner );
            }
        }

        void extrude_corner( const geode::Corner2D& section_corner )
        {
            const auto& corner_slice0 =
                brep_.corner( section_slice0_brep_mapping(
                    geode::Corner3D::component_type_static(),
                    section_corner.id() ) );
            const auto& corner_slice1 =
                brep_.corner( section_slice1_brep_mapping(
                    geode::Corner3D::component_type_static(),
                    section_corner.id() ) );
            const auto& line = brep_.line( brep_builder_.add_line() );
            brep_builder_.add_corner_line_boundary_relationship(
                corner_slice0, line );
            brep_builder_.add_corner_line_boundary_relationship(
                corner_slice1, line );
            auto line_builder = brep_builder_.line_mesh_builder( line.id() );
            const auto corner_slice0_vid =
                line_builder->create_point( corner_slice0.mesh().point( 0 ) );
            brep_builder_.set_unique_vertex(
                { line.component_id(), corner_slice0_vid },
                brep_.unique_vertex( { corner_slice0.component_id(), 0 } ) );
            const auto corner_slice1_vid =
                line_builder->create_point( corner_slice1.mesh().point( 0 ) );
            brep_builder_.set_unique_vertex(
                { line.component_id(), corner_slice1_vid },
                brep_.unique_vertex( { corner_slice1.component_id(), 0 } ) );
            line_builder->create_edge( corner_slice0_vid, corner_slice1_vid );
        }

        void create_surfaces()
        {
            absl::flat_hash_map< geode::uuid, geode::uuid >
                lines_to_model_boundaries;
            for( const auto& model_boundary : section_.model_boundaries() )
            {
                const auto new_id = brep_builder_.add_model_boundary();
                brep_builder_.set_model_boundary_name(
                    new_id, model_boundary.name() );
                for( const auto& item :
                    section_.model_boundary_items( model_boundary ) )
                {
                    lines_to_model_boundaries[item.id()] = new_id;
                }
            }
            for( const auto& line : section_.lines() )
            {
                const auto surface_id = extrude_line( line );
                if( lines_to_model_boundaries.contains( line.id() ) )
                {
                    brep_builder_.add_surface_in_model_boundary(
                        brep_.surface( surface_id ),
                        brep_.model_boundary(
                            lines_to_model_boundaries.at( line.id() ) ) );
                }
            }
        }

        geode::uuid extrude_line( const geode::Line2D& section_line )
        {
            const auto& line_slice0 = brep_.line( section_slice0_brep_mapping(
                geode::Line3D::component_type_static(), section_line.id() ) );
            const auto& line_slice1 = brep_.line( section_slice1_brep_mapping(
                geode::Line3D::component_type_static(), section_line.id() ) );
            const auto& surface = brep_.surface( brep_builder_.add_surface() );
            brep_builder_.add_line_surface_boundary_relationship(
                line_slice0, surface );
            brep_builder_.add_line_surface_boundary_relationship(
                line_slice1, surface );
            add_extruded_line_surface_boundary_relationship(
                section_line, surface );

            auto surface_builder =
                brep_builder_.surface_mesh_builder( surface.id() );
            for( const auto edge_id :
                geode::Range{ line_slice0.mesh().nb_edges() } )
            {
                std::array< geode::index_t, 4 > pointids;
                pointids[0] = find_or_create_surface_vertex(
                    line_slice0, { edge_id, 0 }, surface );
                pointids[1] = find_or_create_surface_vertex(
                    line_slice1, { edge_id, 0 }, surface );
                pointids[2] = find_or_create_surface_vertex(
                    line_slice1, { edge_id, 1 }, surface );
                pointids[3] = find_or_create_surface_vertex(
                    line_slice0, { edge_id, 1 }, surface );
                surface_builder->create_polygon( pointids );
            }
            surface_builder->compute_polygon_adjacencies();
            return surface.id();
        }

        geode::index_t find_or_create_surface_vertex( const geode::Line3D& line,
            const geode::EdgeVertex& e_vertex,
            const geode::Surface3D& surface )
        {
            const auto line_pointid = line.mesh().edge_vertex( e_vertex );
            const auto uvertex_id =
                brep_.unique_vertex( { line.component_id(), line_pointid } );
            for( const auto& cmv : brep_.component_mesh_vertices( uvertex_id ) )
            {
                if( cmv.component_id.id() == surface.id() )
                {
                    return cmv.vertex;
                }
            }
            auto surface_builder =
                brep_builder_.surface_mesh_builder( surface.id() );
            auto pt_id = surface_builder->create_point(
                line.mesh().point( line_pointid ) );
            brep_builder_.set_unique_vertex(
                { surface.component_id(), pt_id }, uvertex_id );
            return pt_id;
        }

        void add_extruded_line_surface_boundary_relationship(
            const geode::Line2D& section_line,
            const geode::Surface3D& brep_surface )
        {
            for( const auto& boundary : section_.boundaries( section_line ) )
            {
                for( const auto& incidence0 :
                    brep_.incidences( brep_.corner( section_slice0_brep_mapping(
                        geode::Corner3D::component_type_static(),
                        boundary.id() ) ) ) )
                {
                    for( const auto& incidence1 : brep_.incidences(
                             brep_.corner( section_slice1_brep_mapping(
                                 geode::Corner3D::component_type_static(),
                                 boundary.id() ) ) ) )
                    {
                        if( incidence0.id() == incidence1.id() )
                        {
                            brep_builder_
                                .add_line_surface_boundary_relationship(
                                    incidence0, brep_surface );
                            break;
                        }
                    }
                }
            }
        }

        void create_blocks()
        {
            for( const auto& surface : section_.surfaces() )
            {
                extrude_surface( surface );
            }
        }

        void extrude_surface( const geode::Surface2D& section_surface )
        {
            const auto& surface_slice0 =
                brep_.surface( section_slice0_brep_mapping(
                    geode::Surface3D::component_type_static(),
                    section_surface.id() ) );
            const auto& surface_slice1 =
                brep_.surface( section_slice1_brep_mapping(
                    geode::Surface3D::component_type_static(),
                    section_surface.id() ) );
            const auto& block = brep_.block( brep_builder_.add_block(
                geode::OpenGeodeHybridSolid3D::impl_name_static() ) );
            brep_builder_.add_surface_block_boundary_relationship(
                surface_slice0, block );
            brep_builder_.add_surface_block_boundary_relationship(
                surface_slice1, block );
            add_extruded_surface_block_boundary_relationship(
                section_surface, block );
            add_extruded_surface_block_internal_relationship(
                section_surface, block );
            add_extruded_line_block_internal_relationship(
                section_surface, block );
            auto block_builder =
                brep_builder_.block_mesh_builder< geode::HybridSolid3D >(
                    block.id() );
            for( const auto tgl_id :
                geode::Range{ surface_slice0.mesh().nb_polygons() } )
            {
                std::array< geode::index_t, 6 > pointids;
                for( const auto p0 : geode::LRange{ 3 } )
                {
                    pointids[p0] = find_or_create_block_vertex(
                        surface_slice0, { tgl_id, p0 }, block );
                    pointids[p0 + 3] = find_or_create_block_vertex(
                        surface_slice1, { tgl_id, p0 }, block );
                }
                block_builder->create_prism( pointids );
            }
            block_builder->compute_polyhedron_adjacencies();
        }

        geode::index_t find_or_create_block_vertex(
            const geode::Surface3D& surface,
            const geode::PolygonVertex& p_vertex,
            const geode::Block3D& block )
        {
            const auto surf_pointid = surface.mesh().polygon_vertex( p_vertex );
            const auto uvertex_id =
                brep_.unique_vertex( { surface.component_id(), surf_pointid } );
            for( const auto& cmv : brep_.component_mesh_vertices( uvertex_id ) )
            {
                if( cmv.component_id.id() == block.id() )
                {
                    return cmv.vertex;
                }
            }
            auto block_builder = brep_builder_.block_mesh_builder( block.id() );
            auto pt_id = block_builder->create_point(
                surface.mesh().point( surf_pointid ) );
            brep_builder_.set_unique_vertex(
                { block.component_id(), pt_id }, uvertex_id );
            return pt_id;
        }

        void add_extruded_surface_block_boundary_relationship(
            const geode::Surface2D& section_surface,
            const geode::Block3D& brep_block )
        {
            for( const auto& boundary : section_.boundaries( section_surface ) )
            {
                for( const auto& incidence0 :
                    brep_.incidences( brep_.line( section_slice0_brep_mapping(
                        geode::Line3D::component_type_static(),
                        boundary.id() ) ) ) )
                {
                    for( const auto& incidence1 : brep_.incidences(
                             brep_.line( section_slice1_brep_mapping(
                                 geode::Line3D::component_type_static(),
                                 boundary.id() ) ) ) )
                    {
                        if( incidence0.id() == incidence1.id() )
                        {
                            brep_builder_
                                .add_surface_block_boundary_relationship(
                                    incidence0, brep_block );
                            break;
                        }
                    }
                }
            }
        }

        void add_extruded_surface_block_internal_relationship(
            const geode::Surface2D& section_surface,
            const geode::Block3D& brep_block )
        {
            for( const auto& internal_line :
                section_.internal_lines( section_surface ) )
            {
                for( const auto& incidence0 :
                    brep_.incidences( brep_.line( section_slice0_brep_mapping(
                        geode::Line3D::component_type_static(),
                        internal_line.id() ) ) ) )
                {
                    for( const auto& incidence1 : brep_.incidences(
                             brep_.line( section_slice1_brep_mapping(
                                 geode::Line3D::component_type_static(),
                                 internal_line.id() ) ) ) )
                    {
                        if( incidence0.id() == incidence1.id() )
                        {
                            brep_builder_
                                .add_surface_block_internal_relationship(
                                    incidence0, brep_block );
                            break;
                        }
                    }
                }
            }
        }

        void add_extruded_line_block_internal_relationship(
            const geode::Surface2D& section_surface,
            const geode::Block3D& brep_block )
        {
            for( const auto& internal_corner :
                section_.internal_corners( section_surface ) )
            {
                for( const auto& incidence0 :
                    brep_.incidences( brep_.corner( section_slice0_brep_mapping(
                        geode::Corner3D::component_type_static(),
                        internal_corner.id() ) ) ) )
                {
                    for( const auto& incidence1 : brep_.incidences(
                             brep_.corner( section_slice1_brep_mapping(
                                 geode::Corner3D::component_type_static(),
                                 internal_corner.id() ) ) ) )
                    {
                        if( incidence0.id() == incidence1.id() )
                        {
                            brep_builder_.add_line_block_internal_relationship(
                                incidence0, brep_block );
                            break;
                        }
                    }
                }
            }
        }

    private:
        const geode::Section& section_;
        geode::BRep brep_;
        geode::BRepBuilder brep_builder_;
        geode::ModelCopyMapping section_slice0_mapping_;
        geode::ModelCopyMapping section_slice1_mapping_;
        geode::ModelCopyMapping slice1_brep_mapping_;
    };
} // namespace

namespace geode
{
    std::tuple< Section, ModelCopyMapping > convert_brep_into_section(
        const BRep& brep, index_t axis_to_remove )
    {
        Section section;
        SectionBuilder builder{ section };
        auto mappings =
            copy_components< BRep, SectionBuilder >( brep, builder );
        for( const auto& corner : brep.corners() )
        {
            builder.update_corner_mesh(
                section.corner( mappings.at( Corner2D::component_type_static() )
                                    .in2out( corner.id() ) ),
                convert_point_set3d_into_2d( corner.mesh(), axis_to_remove ) );
        }
        for( const auto& line : brep.lines() )
        {
            builder.update_line_mesh(
                section.line( mappings.at( Line2D::component_type_static() )
                                  .in2out( line.id() ) ),
                convert_edged_curve3d_into_2d( line.mesh(), axis_to_remove ) );
        }
        for( const auto& surface : brep.surfaces() )
        {
            builder.update_surface_mesh(
                section.surface(
                    mappings.at( Surface2D::component_type_static() )
                        .in2out( surface.id() ) ),
                convert_surface_mesh3d_into_2d(
                    surface.mesh(), axis_to_remove ) );
        }
        copy_unique_vertices( brep, builder, mappings );
        return std::make_tuple( std::move( section ), std::move( mappings ) );
    }

    std::tuple< BRep, ModelCopyMapping > convert_section_into_brep(
        const Section& section, index_t axis_to_add, double axis_coordinate )
    {
        BRep brep;
        BRepBuilder builder{ brep };
        auto mappings =
            copy_components< Section, BRepBuilder >( section, builder );
        // Remove wrong transfer of Model Boundaries
        mappings.remove( ModelBoundary3D::component_type_static() );
        for( const auto& model_boundary : brep.model_boundaries() )
        {
            builder.remove_model_boundary( model_boundary );
        }
        for( const auto& corner : section.corners() )
        {
            builder.update_corner_mesh(
                brep.corner( mappings.at( Corner3D::component_type_static() )
                                 .in2out( corner.id() ) ),
                convert_point_set2d_into_3d(
                    corner.mesh(), axis_to_add, axis_coordinate ) );
        }
        for( const auto& line : section.lines() )
        {
            builder.update_line_mesh(
                brep.line( mappings.at( Line3D::component_type_static() )
                               .in2out( line.id() ) ),
                convert_edged_curve2d_into_3d(
                    line.mesh(), axis_to_add, axis_coordinate ) );
        }
        for( const auto& surface : section.surfaces() )
        {
            builder.update_surface_mesh(
                brep.surface( mappings.at( Surface3D::component_type_static() )
                                  .in2out( surface.id() ) ),
                convert_surface_mesh2d_into_3d(
                    surface.mesh(), axis_to_add, axis_coordinate ) );
        }
        copy_unique_vertices( section, builder, mappings );
        return std::make_tuple( std::move( brep ), std::move( mappings ) );
    }

    BRep extrude_section_to_brep(
        const Section& section, const geode::SectionExtruderOptions& options )
    {
        SectionExtruder extruder{ section };
        return extruder.extrude( options );
    }
} // namespace geode
