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

#include <geode/model/helpers/model_component_filter.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Model, typename Builder >
    void filter_surfaces( const Model& model, Builder& filter )
    {
        std::vector< geode::uuid > surfaces_to_delete;
        surfaces_to_delete.reserve( model.nb_surfaces() );
        for( const auto& surface : model.surfaces() )
        {
            if( model.nb_incidences( surface.id() ) == 0
                && model.nb_embeddings( surface.id() ) == 0 )
            {
                surfaces_to_delete.push_back( surface.id() );
            }
        }
        for( const auto& surface_uuid : surfaces_to_delete )
        {
            filter.remove_surface( model.surface( surface_uuid ) );
        }
    }

    template < typename Model, typename Builder >
    void filter_lines( const Model& model, Builder& filter )
    {
        std::vector< geode::uuid > lines_to_delete;
        lines_to_delete.reserve( model.nb_lines() );
        for( const auto& line : model.lines() )
        {
            if( model.nb_incidences( line.id() ) == 0
                && model.nb_embeddings( line.id() ) == 0 )
            {
                lines_to_delete.push_back( line.id() );
            }
        }
        for( const auto& line_uuid : lines_to_delete )
        {
            filter.remove_line( model.line( line_uuid ) );
        }
    }

    template < typename Model, typename Builder >
    void filter_corners( const Model& model, Builder& filter )
    {
        std::vector< geode::uuid > corners_to_delete;
        corners_to_delete.reserve( model.nb_corners() );
        for( const auto& corner : model.corners() )
        {
            if( model.nb_incidences( corner.id() ) == 0
                && model.nb_embeddings( corner.id() ) == 0 )
            {
                corners_to_delete.push_back( corner.id() );
            }
        }
        for( const auto& corner_uuid : corners_to_delete )
        {
            filter.remove_corner( model.corner( corner_uuid ) );
        }
    }
} // namespace

namespace geode
{
    void filter_brep_components_with_regards_to_blocks( BRep& brep )
    {
        geode::BRepBuilder filter{ brep };
        filter_surfaces( brep, filter );
        filter_lines( brep, filter );
        filter_corners( brep, filter );
    }

    void filter_section_components_with_regards_to_surfaces( Section& section )
    {
        geode::SectionBuilder filter{ section };
        filter_lines( section, filter );
        filter_corners( section, filter );
    }
} // namespace geode