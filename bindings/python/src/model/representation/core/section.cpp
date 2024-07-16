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

#include "../../../common.hpp"

#include <geode/geometry/bounding_box.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_section( pybind11::module& module )
    {
        pybind11::class_< Section, Topology, Corners2D, Lines2D, Surfaces2D,
            ModelBoundaries2D, CornerCollections2D, LineCollections2D,
            SurfaceCollections2D, Identifier >( module, "Section" )
            .def( pybind11::init<>() )
            .def(
                "boundary_corners",
                []( const Section& section, const Line2D& line ) {
                    std::vector< const Corner2D* > components;
                    for( const auto& component : section.boundaries( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "boundary_lines",
                []( const Section& section, const Surface2D& surface ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component : section.boundaries( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "incident_lines",
                []( const Section& section, const Corner2D& corner ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component : section.incidences( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "incident_surfaces",
                []( const Section& section, const Line2D& line ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component : section.incidences( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_corners_of_surface",
                &Section::nb_internal_corners )
            .def(
                "internal_corners_of_surface",
                []( const Section& section, const Surface2D& surface ) {
                    std::vector< const Corner2D* > components;
                    for( const auto& component :
                        section.internal_corners( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_lines_of_surface", &Section::nb_internal_lines )
            .def(
                "internal_lines_of_surface",
                []( const Section& section, const Surface2D& surface ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        section.internal_lines( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_surfaces_of_corner",
                static_cast< index_t ( Section::* )( const Corner2D& ) const >(
                    &Section::nb_embedding_surfaces ) )
            .def(
                "embedding_surfaces_of_corner",
                []( const Section& section, const Corner2D& corner ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component :
                        section.embedding_surfaces( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_surfaces_of_line",
                static_cast< index_t ( Section::* )( const Line2D& ) const >(
                    &Section::nb_embedding_surfaces ) )
            .def(
                "embedding_surfaces_of_line",
                []( const Section& section, const Line2D& line ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component :
                        section.embedding_surfaces( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "model_boundary_items",
                []( const Section& section, const ModelBoundary2D& boundary ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        section.model_boundary_items( boundary ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "corner_collection_items",
                []( const Section& section,
                    const CornerCollection2D& collection ) {
                    std::vector< const Corner2D* > components;
                    for( const auto& component :
                        section.corner_collection_items( collection ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "line_collection_items",
                []( const Section& section,
                    const LineCollection2D& collection ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        section.line_collection_items( collection ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "surface_collection_items",
                []( const Section& section,
                    const SurfaceCollection2D& collection ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component :
                        section.surface_collection_items( collection ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "is_line_closed", &Section::is_closed )
            .def( "is_line_boundary",
                static_cast< bool ( Section::* )( const Corner2D&,
                    const Line2D& ) const >( &Section::is_boundary ) )
            .def( "is_surface_boundary",
                static_cast< bool ( Section::* )( const Line2D&,
                    const Surface2D& ) const >( &Section::is_boundary ) )
            .def( "is_corner_in_surface_internals",
                static_cast< bool ( Section::* )( const Corner2D&,
                    const Surface2D& ) const >( &Section::is_internal ) )
            .def( "is_line_in_surface_internals",
                static_cast< bool ( Section::* )( const Line2D&,
                    const Surface2D& ) const >( &Section::is_internal ) )
            .def( "is_model_boundary_item", &Section::is_model_boundary_item )
            .def( "is_corner_collection_item",
                &Section::is_corner_collection_item )
            .def( "is_line_collection_item", &Section::is_line_collection_item )
            .def( "is_surface_collection_item",
                &Section::is_surface_collection_item )
            .def( "bounding_box", &Section::bounding_box )
            .def( "native_extension", &Section::native_extension );
    }
} // namespace geode
