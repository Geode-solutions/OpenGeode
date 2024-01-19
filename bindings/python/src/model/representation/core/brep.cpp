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

#include "../../../common.h"

#include <geode/geometry/bounding_box.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    void define_brep( pybind11::module& module )
    {
        pybind11::class_< BRep, Topology, Corners3D, Lines3D, Surfaces3D,
            Blocks3D, ModelBoundaries3D, Identifier >( module, "BRep" )
            .def( pybind11::init<>() )
            .def(
                "boundary_corners",
                []( const BRep& brep, const Line3D& line ) {
                    std::vector< const Corner3D* > components;
                    for( const auto& component : brep.boundaries( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "boundary_lines",
                []( const BRep& brep, const Surface3D& surface ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component : brep.boundaries( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "boundary_surfaces",
                []( const BRep& brep, const Block3D& block ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component : brep.boundaries( block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "incident_lines",
                []( const BRep& brep, const Corner3D& corner ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component : brep.incidences( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "incident_surfaces",
                []( const BRep& brep, const Line3D& line ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component : brep.incidences( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "incident_blocks",
                []( const BRep& brep, const Surface3D& surface ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component : brep.incidences( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_corners_of_surface",
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_internal_corners ) )
            .def(
                "internal_corners_of_surface",
                []( const BRep& brep, const Surface3D& surface ) {
                    std::vector< const Corner3D* > components;
                    for( const auto& component :
                        brep.internal_corners( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_lines_of_surface",
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_internal_lines ) )
            .def(
                "internal_lines_of_surface",
                []( const BRep& brep, const Surface3D& surface ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component :
                        brep.internal_lines( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_corners_of_block",
                static_cast< index_t ( BRep::* )( const Block3D& ) const >(
                    &BRep::nb_internal_corners ) )
            .def(
                "internal_corners_of_block",
                []( const BRep& brep, const Block3D& block ) {
                    std::vector< const Corner3D* > components;
                    for( const auto& component :
                        brep.internal_corners( block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_lines_of_block",
                static_cast< index_t ( BRep::* )( const Block3D& ) const >(
                    &BRep::nb_internal_lines ) )
            .def(
                "internal_lines_of_block",
                []( const BRep& brep, const Block3D& block ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component : brep.internal_lines( block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internal_surfaces_of_block", &BRep::nb_internal_surfaces )
            .def(
                "internal_surfaces_of_block",
                []( const BRep& brep, const Block3D& block ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        brep.internal_surfaces( block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_surfaces_of_corner",
                static_cast< index_t ( BRep::* )( const Corner3D& ) const >(
                    &BRep::nb_embedding_surfaces ) )
            .def(
                "embedding_surfaces_of_corner",
                []( const BRep& brep, const Corner3D& corner ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        brep.embedding_surfaces( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_surfaces_of_line",
                static_cast< index_t ( BRep::* )( const Line3D& ) const >(
                    &BRep::nb_embedding_surfaces ) )
            .def(
                "embedding_surfaces_of_line",
                []( const BRep& brep, const Line3D& line ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        brep.embedding_surfaces( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_blocks_of_corner",
                static_cast< index_t ( BRep::* )( const Corner3D& ) const >(
                    &BRep::nb_embedding_blocks ) )
            .def(
                "embedding_blocks_of_corner",
                []( const BRep& brep, const Corner3D& corner ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component :
                        brep.embedding_blocks( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_blocks_of_line",
                static_cast< index_t ( BRep::* )( const Line3D& ) const >(
                    &BRep::nb_embedding_blocks ) )
            .def(
                "embedding_blocks_of_line",
                []( const BRep& brep, const Line3D& line ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component : brep.embedding_blocks( line ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embedding_blocks_of_surface",
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_embedding_blocks ) )
            .def(
                "embedding_blocks_of_surface",
                []( const BRep& brep, const Surface3D& surface ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component :
                        brep.embedding_blocks( surface ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def(
                "model_boundary_items",
                []( const BRep& brep, const ModelBoundary3D& boundary ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        brep.model_boundary_items( boundary ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "is_line_closed",
                static_cast< bool ( BRep::* )( const Line3D& ) const >(
                    &BRep::is_closed ) )
            .def( "is_surface_closed",
                static_cast< bool ( BRep::* )( const Surface3D& ) const >(
                    &BRep::is_closed ) )
            .def( "is_line_boundary",
                static_cast< bool ( BRep::* )( const Corner3D&, const Line3D& )
                        const >( &BRep::is_boundary ) )
            .def( "is_surface_boundary",
                static_cast< bool ( BRep::* )( const Line3D&, const Surface3D& )
                        const >( &BRep::is_boundary ) )
            .def( "is_block_boundary",
                static_cast< bool ( BRep::* )( const Surface3D&,
                    const Block3D& ) const >( &BRep::is_boundary ) )
            .def( "is_corner_in_surface_internals",
                static_cast< bool ( BRep::* )( const Corner3D&,
                    const Surface3D& ) const >( &BRep::is_internal ) )
            .def( "is_line_in_surface_internals",
                static_cast< bool ( BRep::* )( const Line3D&, const Surface3D& )
                        const >( &BRep::is_internal ) )
            .def( "is_corner_in_block_internals",
                static_cast< bool ( BRep::* )( const Corner3D&, const Block3D& )
                        const >( &BRep::is_internal ) )
            .def( "is_line_in_block_internals",
                static_cast< bool ( BRep::* )( const Line3D&, const Block3D& )
                        const >( &BRep::is_internal ) )
            .def( "is_surface_in_block_internals",
                static_cast< bool ( BRep::* )( const Surface3D&,
                    const Block3D& ) const >( &BRep::is_internal ) )
            .def( "is_model_boundary_item", &BRep::is_model_boundary_item )
            .def( "bounding_box", &BRep::bounding_box )
            .def( "native_extension", &BRep::native_extension );
    }
} // namespace geode
