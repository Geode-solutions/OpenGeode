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

#include <geode/model/mixin/builder/lines_builder.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/edged_curve.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/lines.h>

namespace geode
{
    template < index_t dimension >
    const uuid& LinesBuilder< dimension >::create_line()
    {
        return lines_.create_line();
    }

    template < index_t dimension >
    const uuid& LinesBuilder< dimension >::create_line( const MeshImpl& impl )
    {
        return lines_.create_line( impl );
    }

    template < index_t dimension >
    void LinesBuilder< dimension >::delete_line( const Line< dimension >& line )
    {
        lines_.delete_line( line );
    }

    template < index_t dimension >
    void LinesBuilder< dimension >::load_lines( absl::string_view directory )
    {
        return lines_.load_lines( directory );
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurveBuilder< dimension > >
        LinesBuilder< dimension >::line_mesh_builder( const uuid& id )
    {
        return EdgedCurveBuilder< dimension >::create(
            lines_.modifiable_line( id ).modifiable_mesh(
                typename Line< dimension >::LinesBuilderKey{} ) );
    }

    template < index_t dimension >
    void LinesBuilder< dimension >::set_line_name(
        const uuid& id, absl::string_view name )
    {
        lines_.modifiable_line( id ).set_line_name( name, {} );
        line_mesh_builder( id )->set_name( name );
    }

    template < index_t dimension >
    void LinesBuilder< dimension >::set_line_mesh(
        const uuid& id, std::unique_ptr< EdgedCurve< dimension > > mesh )
    {
        lines_.modifiable_line( id ).set_mesh(
            std::move( mesh ), typename Line< dimension >::LinesBuilderKey{} );
    }

    template < index_t dimension >
    EdgedCurve< dimension >& LinesBuilder< dimension >::modifiable_line_mesh(
        const uuid& id )
    {
        return lines_.modifiable_line( id ).modifiable_mesh(
            typename Line< dimension >::LinesBuilderKey{} );
    }

    template class opengeode_model_api LinesBuilder< 2 >;
    template class opengeode_model_api LinesBuilder< 3 >;
} // namespace geode
