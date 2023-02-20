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

#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    namespace detail
    {
        using Mapping = ModelCopyMapping::Mapping;

        template < typename ModelFrom, typename ModelTo >
        Mapping copy_corner_components_without_type( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_corners() );
            for( const auto& corner : from.corners() )
            {
                auto id = builder_to.add_corner();
                builder_to.set_corner_name( id, corner.name() );
                mapping.map( corner.id(), id );
                builder_to.register_mesh_component( to.corner( id ) );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo >
        Mapping copy_line_components_without_type( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_lines() );
            for( const auto& line : from.lines() )
            {
                auto id = builder_to.add_line();
                builder_to.set_line_name( id, line.name() );
                mapping.map( line.id(), id );
                builder_to.register_mesh_component( to.line( id ) );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo >
        Mapping copy_surface_components_without_type( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_surfaces() );
            for( const auto& surface : from.surfaces() )
            {
                auto id = builder_to.add_surface();
                builder_to.set_surface_name( id, surface.name() );
                mapping.map( surface.id(), id );
                builder_to.register_mesh_component( to.surface( id ) );
            }
            return mapping;
        }
    } // namespace detail
} // namespace geode
