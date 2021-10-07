/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "helpers/convert_model_meshes.h"

#include "mixin/builder/blocks_builder.h"
#include "mixin/builder/corners_builder.h"
#include "mixin/builder/lines_builder.h"
#include "mixin/builder/model_boundaries_builder.h"
#include "mixin/builder/relationships_builder.h"
#include "mixin/builder/surfaces_builder.h"
#include "mixin/builder/topology_builder.h"
#include "mixin/builder/vertex_identifier_builder.h"

#include "mixin/core/block.h"
#include "mixin/core/blocks.h"
#include "mixin/core/component.h"
#include "mixin/core/component_type.h"
#include "mixin/core/corner.h"
#include "mixin/core/corners.h"
#include "mixin/core/line.h"
#include "mixin/core/lines.h"
#include "mixin/core/model_boundaries.h"
#include "mixin/core/model_boundary.h"
#include "mixin/core/relationships.h"
#include "mixin/core/surface.h"
#include "mixin/core/surfaces.h"
#include "mixin/core/topology.h"
#include "mixin/core/vertex_identifier.h"

#include "representation/builder/brep_builder.h"
#include "representation/builder/copy.h"
#include "representation/builder/section_builder.h"

#include "representation/core/brep.h"
#include "representation/core/section.h"

#include "representation/io/brep.h"
#include "representation/io/section.h"

namespace pybind11
{
    namespace detail
    {
        template <>
        struct type_caster< absl::string_view >
            : string_caster< absl::string_view, true >
        {
        };

        template < typename Type >
        struct type_caster< absl::FixedArray< Type > >
            : list_caster< absl::FixedArray< Type >, Type >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_py_model, module )
{
    try
    {
        pybind11::module::import( "opengeode_py_basic" );
    }
    catch( ... )
    {
        pybind11::module::import( "opengeode" );
    }
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for model";
    geode::define_component_type( module );
    geode::define_component( module );
    geode::define_block( module );
    geode::define_blocks( module );
    geode::define_corner( module );
    geode::define_corners( module );
    geode::define_line( module );
    geode::define_lines( module );
    geode::define_model_boundary( module );
    geode::define_model_boundaries( module );
    geode::define_surface( module );
    geode::define_surfaces( module );
    geode::define_relationships( module );
    geode::define_vertex_identifier( module );
    geode::define_topology( module );

    geode::define_blocks_builder( module );
    geode::define_corners_builder( module );
    geode::define_lines_builder( module );
    geode::define_model_boundaries_builder( module );
    geode::define_surfaces_builder( module );
    geode::define_relationships_builder( module );
    geode::define_vertex_identifier_builder( module );
    geode::define_topology_builder( module );

    geode::define_brep( module );
    geode::define_section( module );

    geode::define_copy( module );
    geode::define_brep_builder( module );
    geode::define_section_builder( module );

    geode::define_brep_io( module );
    geode::define_section_io( module );

    geode::define_convert_model_meshes( module );
}