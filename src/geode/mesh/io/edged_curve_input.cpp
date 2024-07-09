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

#include <geode/mesh/io/edged_curve_input.h>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > load_edged_curve(
        const MeshImpl& impl, absl::string_view filename )
    {
        const auto type = absl::StrCat( "EdgedCurve", dimension, "D" );
        try
        {
            auto edged_curve = detail::geode_object_input_impl(
                edged_curve_input_factory< dimension >(), type, filename,
                impl );
            Logger::info( type, " has: ", edged_curve->nb_vertices(),
                " vertices, ", edged_curve->nb_edges(), " edges" );
            return edged_curve;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions(
                edged_curve_input_factory< dimension >(), type );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions( vertex_set_input_factory, "VertexSet" );
            throw OpenGeodeException{ "Cannot load EdgedCurve from file: ",
                filename };
        }
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > load_edged_curve(
        absl::string_view filename )
    {
        return load_edged_curve< dimension >(
            mesh_factory.default_impl(
                EdgedCurve< dimension >::type_name_static() ),
            filename );
    }

    template < index_t dimension >
    typename EdgedCurveInput< dimension >::MissingFiles
        check_edged_curve_missing_files( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader(
            edged_curve_input_factory< dimension >(), filename );
        return input->check_missing_files();
    }

    template < index_t dimension >
    bool is_edged_curve_loadable( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader(
            edged_curve_input_factory< dimension >(), filename );
        return input->is_loadable();
    }

    template std::unique_ptr< EdgedCurve< 2 > > opengeode_mesh_api
        load_edged_curve( const MeshImpl&, absl::string_view );
    template std::unique_ptr< EdgedCurve< 3 > > opengeode_mesh_api
        load_edged_curve( const MeshImpl&, absl::string_view );

    template std::unique_ptr< EdgedCurve< 2 > >
        opengeode_mesh_api load_edged_curve( absl::string_view );
    template std::unique_ptr< EdgedCurve< 3 > >
        opengeode_mesh_api load_edged_curve( absl::string_view );

    template EdgedCurveInput< 2 >::MissingFiles opengeode_mesh_api
        check_edged_curve_missing_files< 2 >( absl::string_view );
    template EdgedCurveInput< 3 >::MissingFiles opengeode_mesh_api
        check_edged_curve_missing_files< 3 >( absl::string_view );

    template bool opengeode_mesh_api is_edged_curve_loadable< 2 >(
        absl::string_view );
    template bool opengeode_mesh_api is_edged_curve_loadable< 3 >(
        absl::string_view );
} // namespace geode
