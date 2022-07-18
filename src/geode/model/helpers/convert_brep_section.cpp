/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/helpers/convert_edged_curve.h>
#include <geode/mesh/helpers/convert_point_set.h>
#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/model/helpers/private/copy.h>
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
    template < typename ModelFrom,
        typename ModelTo,
        typename BuilderTo,
        geode::index_t dimension >
    geode::ModelCopyMapping copy_components(
        const ModelFrom& from, const ModelTo& to, BuilderTo& builder_to )
    {
        geode::ModelCopyMapping mappings;
        mappings.emplace( geode::Corner< dimension >::component_type_static(),
            geode::detail::copy_corner_components_without_type(
                from, to, builder_to ) );
        mappings.emplace( geode::Line< dimension >::component_type_static(),
            geode::detail::copy_line_components_without_type(
                from, to, builder_to ) );
        mappings.emplace( geode::Surface< dimension >::component_type_static(),
            geode::detail::copy_surface_components_without_type(
                from, to, builder_to ) );
        builder_to.copy_relationships( mappings, from );
        return mappings;
    }

    template < typename ModelFrom, typename BuilderTo >
    void copy_unique_vertices( const ModelFrom& from,
        BuilderTo& builder_to,
        const geode::ModelCopyMapping& mappings )
    {
        builder_to.create_unique_vertices( from.nb_unique_vertices() );
        geode::detail::copy_vertex_identifier_components(
            from, builder_to, mappings );
    }
} // namespace

namespace geode
{
    std::tuple< Section, ModelCopyMapping > convert_brep_into_section(
        const BRep& brep, index_t axis_to_remove )
    {
        Section section;
        SectionBuilder builder{ section };
        DEBUG_CONST auto mappings =
            copy_components< BRep, Section, SectionBuilder, 2 >(
                brep, section, builder );
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
        const Section& section, index_t axis_to_add )
    {
        BRep brep;
        BRepBuilder builder{ brep };
        DEBUG_CONST auto mappings =
            copy_components< Section, BRep, BRepBuilder, 3 >(
                section, brep, builder );
        for( const auto& corner : section.corners() )
        {
            builder.update_corner_mesh(
                brep.corner( mappings.at( Corner3D::component_type_static() )
                                 .in2out( corner.id() ) ),
                convert_point_set2d_into_3d( corner.mesh(), axis_to_add ) );
        }
        for( const auto& line : section.lines() )
        {
            builder.update_line_mesh(
                brep.line( mappings.at( Line3D::component_type_static() )
                               .in2out( line.id() ) ),
                convert_edged_curve2d_into_3d( line.mesh(), axis_to_add ) );
        }
        for( const auto& surface : section.surfaces() )
        {
            builder.update_surface_mesh(
                brep.surface( mappings.at( Surface3D::component_type_static() )
                                  .in2out( surface.id() ) ),
                convert_surface_mesh2d_into_3d( surface.mesh(), axis_to_add ) );
        }
        copy_unique_vertices( section, builder, mappings );
        return std::make_tuple( std::move( brep ), std::move( mappings ) );
    }
} // namespace geode
