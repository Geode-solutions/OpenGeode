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
            SurfaceCollections2D, Identifier, pybind11::smart_holder >( module,
            "Section",
            "A 2D model made of Corners, Lines and Surfaces linked by boundary "
            "relationships (a Line is bounded by Corners, a Surface by Lines), "
            "internal relationships, and organized by ModelBoundaries and "
            "collections. A Section is modified through SectionBuilder. "
            "Besides its own methods it inherits those of the Topology and of "
            "the component containers" )
            .def( pybind11::init<>(), "Build an empty Section" )
            .def(
                "__repr__",
                []( const Section& model ) {
                    return identifier_repr( model, python_type_name( model ),
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) } } );
                },
                "Return an unambiguous description of the model: its type, its "
                "id, its name if it has one and its numbers of components" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners bounding the Line (its end points)" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines bounding the Surface" )
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
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines bounded by the Corner" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces bounded by the Line" )
            .def( "nb_internal_corners_of_line",
                static_cast< index_t ( Section::* )( const Line2D& ) const >(
                    &Section::nb_internal_corners ),
                pybind11::arg( "line" ),
                "Return the number of Corners internal to the Line, i.e. lying "
                "on it without being one of its boundaries" )
            .def(
                "internal_corners_of_line",
                []( const Section& section, const Line2D& line ) {
                    std::vector< const Corner2D* > components;
                    for( const auto& component :
                        section.internal_corners( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners internal to the Line, i.e. lying on it "
                "without being one of its boundaries" )
            .def( "nb_internal_corners_of_surface",
                static_cast< index_t ( Section::* )( const Surface2D& ) const >(
                    &Section::nb_internal_corners ),
                pybind11::arg( "surface" ),
                "Return the number of Corners internal to the Surface, i.e. "
                "lying on it without bounding any of its Lines" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners internal to the Surface, i.e. lying on it "
                "without bounding any of its Lines" )
            .def( "nb_internal_lines_of_surface", &Section::nb_internal_lines,
                pybind11::arg( "surface" ),
                "Return the number of Lines internal to the Surface, i.e. "
                "lying on it without being one of its boundaries" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines internal to the Surface, i.e. lying on it "
                "without being one of its boundaries" )
            .def( "nb_embedding_lines_of_corner",
                static_cast< index_t ( Section::* )( const Corner2D& ) const >(
                    &Section::nb_embedding_lines ),
                pybind11::arg( "corner" ),
                "Return the number of Lines in which the Corner is internal" )
            .def(
                "embedding_lines_of_corner",
                []( const Section& section, const Corner2D& corner ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        section.embedding_lines( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines in which the Corner is internal" )
            .def( "nb_embedding_surfaces_of_corner",
                static_cast< index_t ( Section::* )( const Corner2D& ) const >(
                    &Section::nb_embedding_surfaces ),
                pybind11::arg( "corner" ),
                "Return the number of Surfaces in which the Corner is "
                "internal" )
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
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces in which the Corner is internal" )
            .def( "nb_embedding_surfaces_of_line",
                static_cast< index_t ( Section::* )( const Line2D& ) const >(
                    &Section::nb_embedding_surfaces ),
                pybind11::arg( "line" ),
                "Return the number of Surfaces in which the Line is internal" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces in which the Line is internal" )
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
                pybind11::arg( "boundary" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines belonging to the ModelBoundary" )
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
                pybind11::arg( "collection" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners belonging to the CornerCollection" )
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
                pybind11::arg( "collection" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines belonging to the LineCollection" )
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
                pybind11::arg( "collection" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces belonging to the SurfaceCollection" )
            .def( "is_line_closed", &Section::is_closed,
                pybind11::arg( "line" ),
                "Return True if the Line has fewer than two boundary Corners "
                "(it is a loop or has none), False otherwise" )
            .def( "is_line_boundary",
                static_cast< bool ( Section::* )( const Corner2D&,
                    const Line2D& ) const >( &Section::is_boundary ),
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Return True if the Corner is a boundary of the Line" )
            .def( "is_surface_boundary",
                static_cast< bool ( Section::* )( const Line2D&,
                    const Surface2D& ) const >( &Section::is_boundary ),
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Return True if the Line is a boundary of the Surface" )
            .def( "is_corner_in_line_internals",
                static_cast< bool ( Section::* )( const Corner2D&,
                    const Line2D& ) const >( &Section::is_internal ),
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Return True if the Corner is internal to the Line" )
            .def( "is_corner_in_surface_internals",
                static_cast< bool ( Section::* )( const Corner2D&,
                    const Surface2D& ) const >( &Section::is_internal ),
                pybind11::arg( "corner" ), pybind11::arg( "surface" ),
                "Return True if the Corner is internal to the Surface" )
            .def( "is_line_in_surface_internals",
                static_cast< bool ( Section::* )( const Line2D&,
                    const Surface2D& ) const >( &Section::is_internal ),
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Return True if the Line is internal to the Surface" )
            .def( "is_model_boundary_item", &Section::is_model_boundary_item,
                pybind11::arg( "line" ), pybind11::arg( "boundary" ),
                "Return True if the Line belongs to the ModelBoundary" )
            .def( "is_corner_collection_item",
                &Section::is_corner_collection_item, pybind11::arg( "corner" ),
                pybind11::arg( "corner_collection" ),
                "Return True if the Corner belongs to the CornerCollection" )
            .def( "is_line_collection_item", &Section::is_line_collection_item,
                pybind11::arg( "line" ), pybind11::arg( "line_collection" ),
                "Return True if the Line belongs to the LineCollection" )
            .def( "is_surface_collection_item",
                &Section::is_surface_collection_item,
                pybind11::arg( "surface" ),
                pybind11::arg( "surface_collection" ),
                "Return True if the Surface belongs to the SurfaceCollection" )
            .def( "bounding_box", &Section::bounding_box,
                "Return the bounding box of the meshes of all the components, "
                "active or not; raise an error if the model has no vertex" )
            .def( "native_extension", &Section::native_extension,
                "Return the extension of the native file format of the class" )
            .def( "section_component", &Section::component,
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component of the Section with the given uuid, "
                "whatever its type (an instance of its own class: Corner2D, "
                "Surface2D, ...); raise an error if the Section has none" );
    }
} // namespace geode
