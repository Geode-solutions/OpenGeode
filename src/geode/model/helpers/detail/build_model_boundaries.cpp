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

#include <geode/model/helpers/detail/build_model_boundaries.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < class Model >
    bool is_part_of_a_model_boundary(
        const Model& model, const geode::uuid& component_id )
    {
        for( const auto& collection : model.collections( component_id ) )
        {
            if( collection.type()
                == geode::ModelBoundary< Model::dim >::component_type_static() )
            {
                return true;
            }
        }
        return false;
    }

    template < class Model >
    const geode::ModelBoundary< Model::dim >& find_or_create_boundary(
        const Model& model,
        typename Model::Builder& builder,
        std::string_view name )
    {
        for( const auto& boundary : model.model_boundaries() )
        {
            if( boundary.name() == name )
            {
                return boundary;
            }
        }
        const auto& model_boundary_id = builder.add_model_boundary();
        builder.set_model_boundary_name( model_boundary_id, name );
        const auto& boundary = model.model_boundary( model_boundary_id );
        return boundary;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void build_model_boundaries(
            const Section& model, SectionBuilder& builder )
        {
            for( const auto& line : model.lines() )
            {
                if( model.nb_incidences( line.id() ) != 1
                    || ::is_part_of_a_model_boundary( model, line.id() ) )
                {
                    continue;
                }
                const auto& boundary =
                    ::find_or_create_boundary( model, builder, line.name() );
                builder.add_line_in_model_boundary( line, boundary );
            }
        }

        void build_model_boundaries( const BRep& model, BRepBuilder& builder )
        {
            for( const auto& surface : model.surfaces() )
            {
                if( model.nb_incidences( surface.id() ) != 1
                    || ::is_part_of_a_model_boundary( model, surface.id() ) )
                {
                    continue;
                }
                const auto& boundary =
                    ::find_or_create_boundary( model, builder, surface.name() );
                builder.add_surface_in_model_boundary( surface, boundary );
            }
        }
    } // namespace detail
} // namespace geode