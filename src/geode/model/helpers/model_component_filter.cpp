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

#include <geode/model/helpers/model_component_filter.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model, typename ComponentRange >
    std::vector< geode::uuid > find_components_to_delete( const Model& model,
        const ComponentRange& components,
        geode::index_t nb_components )
    {
        std::vector< geode::uuid > to_delete;
        to_delete.reserve( nb_components );
        for( const auto& component : components )
        {
            if( model.nb_incidences( component.id() ) == 0
                && model.nb_embeddings( component.id() ) == 0 )
            {
                to_delete.push_back( component.id() );
            }
        }
        return to_delete;
    }

    template < typename Model >
    void filter_surfaces( const Model& model,
        typename Model::Builder& filter,
        std::vector< geode::ComponentID >& removed_components )
    {
        for( const auto& surface_uuid : find_components_to_delete(
                 model, model.surfaces(), model.nb_surfaces() ) )
        {
            filter.remove_surface( model.surface( surface_uuid ) );
            removed_components.emplace_back(
                geode::Surface3D::component_type_static(), surface_uuid );
        }
    }

    template < typename Model >
    void filter_lines( const Model& model,
        typename Model::Builder& filter,
        std::vector< geode::ComponentID >& removed_components )
    {
        for( const auto& line_uuid : find_components_to_delete(
                 model, model.lines(), model.nb_lines() ) )
        {
            filter.remove_line( model.line( line_uuid ) );
            removed_components.emplace_back(
                geode::Line3D::component_type_static(), line_uuid );
        }
    }

    template < typename Model >
    void filter_corners( const Model& model,
        typename Model::Builder& filter,
        std::vector< geode::ComponentID >& removed_components )
    {
        for( const auto& corner_uuid : find_components_to_delete(
                 model, model.corners(), model.nb_corners() ) )
        {
            filter.remove_corner( model.corner( corner_uuid ) );
            removed_components.emplace_back(
                geode::Corner3D::component_type_static(), corner_uuid );
        }
    }
} // namespace

namespace geode
{
    std::vector< ComponentID > filter_brep_components_with_regards_to_blocks(
        BRep& brep )
    {
        std::vector< ComponentID > removed_components;
        BRepBuilder filter{ brep };
        filter_surfaces( brep, filter, removed_components );
        filter_lines( brep, filter, removed_components );
        filter_corners( brep, filter, removed_components );
        filter.delete_isolated_vertices();
        return removed_components;
    }

    std::vector< ComponentID >
        filter_section_components_with_regards_to_surfaces( Section& section )
    {
        std::vector< ComponentID > removed_components;
        SectionBuilder filter{ section };
        filter_lines( section, filter, removed_components );
        filter_corners( section, filter, removed_components );
        filter.delete_isolated_vertices();
        return removed_components;
    }
} // namespace geode