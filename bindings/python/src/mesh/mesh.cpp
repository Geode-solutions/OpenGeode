/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "builder/edged_curve_builder.h"
#include "builder/graph_builder.h"
#include "builder/hybrid_solid_builder.h"
#include "builder/point_set_builder.h"
#include "builder/polygonal_surface_builder.h"
#include "builder/polyhedral_solid_builder.h"
#include "builder/solid_edges_builder.h"
#include "builder/solid_facets_builder.h"
#include "builder/solid_mesh_builder.h"
#include "builder/surface_edges_builder.h"
#include "builder/surface_mesh_builder.h"
#include "builder/tetrahedral_solid_builder.h"
#include "builder/triangulated_surface_builder.h"
#include "builder/vertex_set_builder.h"

#include "core/edged_curve.h"
#include "core/graph.h"
#include "core/hybrid_solid.h"
#include "core/point_set.h"
#include "core/polygonal_surface.h"
#include "core/polyhedral_solid.h"
#include "core/solid_edges.h"
#include "core/solid_facets.h"
#include "core/solid_mesh.h"
#include "core/surface_edges.h"
#include "core/surface_mesh.h"
#include "core/tetrahedral_solid.h"
#include "core/triangulated_surface.h"
#include "core/vertex_set.h"

#include "helpers/convert_solid_mesh.h"
#include "helpers/convert_surface_mesh.h"

#include "io/edged_curve.h"
#include "io/graph.h"
#include "io/hybrid_solid.h"
#include "io/point_set.h"
#include "io/polygonal_surface.h"
#include "io/polyhedral_solid.h"
#include "io/tetrahedral_solid.h"
#include "io/triangulated_surface.h"
#include "io/vertex_set.h"

namespace pybind11
{
    namespace detail
    {
        template < typename Type, size_t dimension >
        struct type_caster< absl::InlinedVector< Type, dimension > >
            : list_caster< absl::InlinedVector< Type, dimension >, Type >
        {
        };

        template < typename Type >
        struct type_caster< absl::Span< Type > >
            : list_caster< absl::Span< Type >, Type >
        {
            using value_conv = make_caster< Type >;

            bool load( handle src, bool convert )
            {
                cpp_.clear();
                auto s = reinterpret_borrow< sequence >( src );
                cpp_.reserve( s.size() );
                for( auto it : s )
                {
                    value_conv conv;
                    if( !conv.load( it, convert ) )
                        return false;
                    cpp_.push_back( cast_op< Type&& >( std::move( conv ) ) );
                }
                this->value = absl::MakeConstSpan( cpp_ );
                return true;
            }

            std::vector< typename std::remove_const< Type >::type > cpp_;
        };

        template <>
        struct type_caster< absl::string_view >
            : string_caster< absl::string_view, true >
        {
        };

        template < typename T >
        struct type_caster< absl::optional< T > >
            : public optional_caster< absl::optional< T > >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_py_mesh, module )
{
    module.doc() = "OpenGeode Python binding for mesh";
    geode::define_vertex_set( module );
    geode::define_graph( module );
    geode::define_edged_curve( module );
    geode::define_point_set( module );
    geode::define_surface_mesh( module );
    geode::define_surface_edges( module );
    geode::define_polygonal_surface( module );
    geode::define_triangulated_surface( module );
    geode::define_solid_mesh( module );
    geode::define_solid_edges( module );
    geode::define_solid_facets( module );
    geode::define_polyhedral_solid( module );
    geode::define_tetrahedral_solid( module );
    geode::define_hybrid_solid( module );

    geode::define_vertex_set_builder( module );
    geode::define_graph_builder( module );
    geode::define_edged_curve_builder( module );
    geode::define_point_set_builder( module );
    geode::define_surface_mesh_builder( module );
    geode::define_surface_edges_builder( module );
    geode::define_polygonal_surface_builder( module );
    geode::define_triangulated_surface_builder( module );
    geode::define_solid_mesh_builder( module );
    geode::define_solid_edges_builder( module );
    geode::define_solid_facets_builder( module );
    geode::define_polyhedral_solid_builder( module );
    geode::define_tetrahedral_solid_builder( module );
    geode::define_hybrid_solid_builder( module );

    geode::define_convert_surface_mesh( module );
    geode::define_convert_solid_mesh( module );

    geode::define_vertex_set_io( module );
    geode::define_graph_io( module );
    geode::define_edged_curve_io( module );
    geode::define_point_set_io( module );
    geode::define_polygonal_surface_io( module );
    geode::define_triangulated_surface_io( module );
    geode::define_polyhedral_solid_io( module );
    geode::define_tetrahedral_solid_io( module );
    geode::define_hybrid_solid_io( module );
}