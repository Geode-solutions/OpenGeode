/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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
#include <geode/mesh/builder/geode/geode_point_set_builder.h>
#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/helpers/convert_edged_curve.h>
#include <geode/mesh/helpers/convert_point_set.h>
#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/model/helpers/model_concatener.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
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
        mappings.emplace( geode::Corner< dimension >::component_type_static(),
            geode::detail::copy_corner_components( from, builder_to ) );
        mappings.emplace( geode::Line< dimension >::component_type_static(),
            geode::detail::copy_line_components( from, builder_to ) );
        mappings.emplace( geode::Surface< dimension >::component_type_static(),
            geode::detail::copy_surface_components( from, builder_to ) );
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

        geode::BRep extrude( geode::index_t axis_to_extrude,
            double min_coordinate,
            double max_coordinate )
        {
            initialize_extrusion(
                axis_to_extrude, min_coordinate, max_coordinate );
            create_lines();
            create_surfaces();
            create_blocks();
            return std::move( brep_ );
        }

    private:
        void initialize_extrusion( geode::index_t axis_to_extrude,
            double min_coordinate,
            double max_coordinate )
        {
            auto slice0_conversion_output = convert_section_into_brep(
                section_, axis_to_extrude, min_coordinate );
            section_slice0_mapping_ = std::get< 1 >( slice0_conversion_output );
            slice0_brep_mapping_ =
                brep_builder_.copy( std::get< 0 >( slice0_conversion_output ) );

            auto slice1_conversion_output = convert_section_into_brep(
                section_, axis_to_extrude, max_coordinate );
            section_slice1_mapping_ = std::get< 1 >( slice1_conversion_output );
            geode::BRepConcatener brep_concatener{ brep_ };
            slice1_brep_mapping_ = brep_concatener.concatenate(
                std::get< 0 >( slice1_conversion_output ) );
        }

        geode::uuid section_slice0_brep_mapping(
            const geode::ComponentType& brep_component_type,
            geode::uuid section_component )
        {
            return slice0_brep_mapping_.at( brep_component_type )
                .in2out( section_slice0_mapping_.at( brep_component_type )
                             .in2out( section_component ) );
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
            for( const auto& line : section_.lines() )
            {
                extrude_line( line );
            }
        }

        void extrude_line( const geode::Line2D& section_line )
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
            const auto prev_slide0_pointid =
                line_slice0.mesh().edge_vertex( { 0, 0 } );
            auto prev_slice0_pt = surface_builder->create_point(
                line_slice0.mesh().point( prev_slide0_pointid ) );
            brep_builder_.set_unique_vertex(
                { surface.component_id(), prev_slice0_pt },
                brep_.unique_vertex(
                    { line_slice0.component_id(), prev_slide0_pointid } ) );
            const auto prev_slide1_pointid =
                line_slice1.mesh().edge_vertex( { 0, 0 } );
            auto prev_slice1_pt = surface_builder->create_point(
                line_slice1.mesh().point( prev_slide1_pointid ) );
            brep_builder_.set_unique_vertex(
                { surface.component_id(), prev_slice1_pt },
                brep_.unique_vertex(
                    { line_slice1.component_id(), prev_slide1_pointid } ) );
            for( const auto edge_id :
                geode::Range{ line_slice0.mesh().nb_edges() } )
            {
                const auto slide0_pointid =
                    line_slice0.mesh().edge_vertex( { edge_id, 1 } );
                const auto slide0_pt = surface_builder->create_point(
                    line_slice0.mesh().point( slide0_pointid ) );
                brep_builder_.set_unique_vertex(
                    { surface.component_id(), slide0_pt },
                    brep_.unique_vertex(
                        { line_slice0.component_id(), slide0_pointid } ) );
                const auto slide1_pointid =
                    line_slice1.mesh().edge_vertex( { edge_id, 1 } );
                const auto slice1_pt = surface_builder->create_point(
                    line_slice1.mesh().point( slide1_pointid ) );
                brep_builder_.set_unique_vertex(
                    { surface.component_id(), slice1_pt },
                    brep_.unique_vertex(
                        { line_slice1.component_id(), slide1_pointid } ) );
                surface_builder->create_polygon(
                    { prev_slice0_pt, prev_slice1_pt, slice1_pt, slide0_pt } );
                prev_slice0_pt = slide0_pt;
                prev_slice1_pt = slice1_pt;
            }
            surface_builder->compute_polygon_adjacencies();
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
            const auto& block = brep_.block( brep_builder_.add_block() );
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
        geode::ModelCopyMapping slice0_brep_mapping_;
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
        DEBUG_CONST auto mappings =
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
        DEBUG_CONST auto mappings =
            copy_components< Section, BRepBuilder >( section, builder );
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

    BRep extrude_section_to_brep( const Section& section,
        index_t axis_to_extrude,
        double min_coordinate,
        double max_coordinate )
    {
        SectionExtruder extruder{ section };
        return extruder.extrude(
            axis_to_extrude, min_coordinate, max_coordinate );
    }
} // namespace geode
