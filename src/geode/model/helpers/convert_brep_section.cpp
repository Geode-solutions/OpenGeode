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

namespace geode
{
    Section convert_brep_into_section(
        const BRep& brep, index_t axis_to_remove )
    {
        Section section;
        SectionBuilder builder{ section };
        ModelCopyMapping mappings;
        mappings.emplace( Corner2D::component_type_static(),
            detail::copy_corner_components_without_type(
                brep, section, builder ) );
        mappings.emplace( Line2D::component_type_static(),
            detail::copy_line_components_without_type(
                brep, section, builder ) );
        mappings.emplace( Surface2D::component_type_static(),
            detail::copy_surface_components_without_type(
                brep, section, builder ) );
        builder.copy_relationships( mappings, brep );

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

        builder.create_unique_vertices( brep.nb_unique_vertices() );
        detail::copy_vertex_identifier_components( brep, builder, mappings );
        return section;
    }

    BRep convert_section_into_brep(
        const Section& section, index_t axis_to_add )
    {
        BRep brep;
        BRepBuilder builder{ brep };
        ModelCopyMapping mappings;
        mappings.emplace( Corner3D::component_type_static(),
            detail::copy_corner_components_without_type(
                section, brep, builder ) );
        mappings.emplace( Line3D::component_type_static(),
            detail::copy_line_components_without_type(
                section, brep, builder ) );
        mappings.emplace( Surface3D::component_type_static(),
            detail::copy_surface_components_without_type(
                section, brep, builder ) );

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

        builder.create_unique_vertices( section.nb_unique_vertices() );
        detail::copy_vertex_identifier_components( section, builder, mappings );
        return brep;
    }
} // namespace geode
