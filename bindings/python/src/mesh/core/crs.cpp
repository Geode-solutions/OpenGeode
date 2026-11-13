/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include "../../common.hpp"

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/coordinate_reference_system.hpp>

namespace geode
{
    /*!
     * Only the CRS storing one point per attribute element knows its size.
     */
    template < index_t dimension >
    void check_crs_point( std::string_view name,
        const CoordinateReferenceSystem< dimension >& crs,
        index_t point_id )
    {
        if( const auto* attribute_crs = dynamic_cast<
                const AttributeCoordinateReferenceSystem< dimension >* >(
                &crs ) )
        {
            check_index( name, "point", point_id, attribute_crs->nb_points() );
        }
    }
} // namespace geode

#define PYTHON_CRS( dimension )                                                \
    const auto name##dimension =                                               \
        "CoordinateReferenceSystem" + std::to_string( dimension ) + "D";       \
    pybind11::class_< CoordinateReferenceSystem##dimension##D >( module,       \
        name##dimension.c_str(),                                               \
        "Coordinate reference system of a mesh: it gives the coordinates of "  \
        "the mesh vertices" )                                                  \
        .def( "type_name",                                                     \
            &CoordinateReferenceSystem##dimension##D::type_name,               \
            "Return the type of the coordinate reference system" )             \
        .def(                                                                  \
            "point",                                                           \
            []( const CoordinateReferenceSystem< dimension >& crs,             \
                index_t point_id ) -> const Point< dimension >& {              \
                check_crs_point( "point", crs, point_id );                     \
                return crs.point( point_id );                                  \
            },                                                                 \
            "Return the coordinates of the point of the given index; raise "   \
            "IndexError if it is out of range for a coordinate reference "     \
            "system storing one point per vertex",                             \
            pybind11::arg( "point_id" ) )                                      \
        .def(                                                                  \
            "set_point",                                                       \
            []( CoordinateReferenceSystem< dimension >& crs, index_t point_id, \
                Point< dimension > point ) {                                   \
                check_crs_point( "set_point", crs, point_id );                 \
                crs.set_point( point_id, std::move( point ) );                 \
            },                                                                 \
            "Set the coordinates of the point of the given index; raise "      \
            "IndexError if it is out of range for a coordinate reference "     \
            "system storing one point per vertex",                             \
            pybind11::arg( "point_id" ), pybind11::arg( "point" ) )

namespace geode
{
    void define_crs( pybind11::module& module )
    {
        pybind11::class_< CRSType >( module, "CRSType",
            "Name identifying the type of a coordinate reference system" )
            .def( pybind11::init<>(), "Build a type with an empty name" )
            .def( pybind11::init< std::string >(),
                "Build a type with the given name", pybind11::arg( "value" ) )
            .def( "get", &CRSType::get, "Return the name as a string" )
            .def( "matches", &CRSType::operator==,
                "Return True if the other type has the same name",
                pybind11::arg( "other" ) );

        PYTHON_CRS( 1 );
        PYTHON_CRS( 2 );
        PYTHON_CRS( 3 );
    }
} // namespace geode
