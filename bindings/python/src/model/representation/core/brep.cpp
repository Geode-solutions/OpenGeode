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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    void define_brep( pybind11::module& module )
    {
        pybind11::class_< BRep, Topology, Corners3D, Lines3D, Surfaces3D,
            Blocks3D, ModelBoundaries3D, CornerCollections3D, LineCollections3D,
            SurfaceCollections3D, BlockCollections3D, PhysicalProperties,
            Identifier, pybind11::smart_holder >( module, "BRep",
            "Boundary representation: a 3D model made of Corners, Lines, "
            "Surfaces and Blocks linked by boundary relationships (a Line is "
            "bounded by Corners, a Surface by Lines, a Block by Surfaces), "
            "internal relationships, and organized by ModelBoundaries and "
            "collections. A BRep is modified through BRepBuilder. Besides its "
            "own methods it inherits those of the Topology, of the component "
            "containers and of PhysicalProperties" )
            .def( pybind11::init<>(), "Build an empty BRep" )
            .def(
                "__repr__",
                []( const BRep& model ) {
                    return identifier_repr( model, python_type_name( model ),
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) },
                            { "nb_blocks",
                                python_repr( model.nb_blocks() ) } } );
                },
                "Return an unambiguous description of the model: its type, its "
                "id, its name if it has one and its numbers of components" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners bounding the Line (its end points)" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines bounding the Surface" )
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
                pybind11::arg( "block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces bounding the Block" )
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
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines bounded by the Corner" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces bounded by the Line" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks bounded by the Surface" )
            .def( "nb_internal_corners_of_line",
                static_cast< index_t ( BRep::* )( const Line3D& ) const >(
                    &BRep::nb_internal_corners ),
                pybind11::arg( "line" ),
                "Return the number of Corners internal to the Line, i.e. lying "
                "on it without being one of its boundaries" )
            .def(
                "internal_corners_of_line",
                []( const BRep& brep, const Line3D& line ) {
                    std::vector< const Corner3D* > components;
                    for( const auto& component : brep.internal_corners( line ) )
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
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_internal_corners ),
                pybind11::arg( "surface" ),
                "Return the number of Corners internal to the Surface, i.e. "
                "lying on it without bounding any of its Lines" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners internal to the Surface, i.e. lying on it "
                "without bounding any of its Lines" )
            .def( "nb_internal_lines_of_surface",
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_internal_lines ),
                pybind11::arg( "surface" ),
                "Return the number of Lines internal to the Surface, i.e. "
                "lying on it without being one of its boundaries" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines internal to the Surface, i.e. lying on it "
                "without being one of its boundaries" )
            .def( "nb_internal_corners_of_block",
                static_cast< index_t ( BRep::* )( const Block3D& ) const >(
                    &BRep::nb_internal_corners ),
                pybind11::arg( "block" ),
                "Return the number of Corners internal to the Block, i.e. "
                "inside it" )
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
                pybind11::arg( "block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Corners internal to the Block, i.e. inside it" )
            .def( "nb_internal_lines_of_block",
                static_cast< index_t ( BRep::* )( const Block3D& ) const >(
                    &BRep::nb_internal_lines ),
                pybind11::arg( "block" ),
                "Return the number of Lines internal to the Block, i.e. inside "
                "it" )
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
                pybind11::arg( "block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines internal to the Block, i.e. inside it" )
            .def( "nb_internal_surfaces_of_block", &BRep::nb_internal_surfaces,
                pybind11::arg( "block" ),
                "Return the number of Surfaces internal to the Block, i.e. "
                "inside it without being one of its boundaries" )
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
                pybind11::arg( "block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces internal to the Block, i.e. inside it "
                "without being one of its boundaries" )
            .def( "nb_embedding_lines_of_corner",
                static_cast< index_t ( BRep::* )( const Corner3D& ) const >(
                    &BRep::nb_embedding_lines ),
                pybind11::arg( "corner" ),
                "Return the number of Lines in which the Corner is internal" )
            .def(
                "embedding_lines_of_corner",
                []( const BRep& brep, const Corner3D& corner ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component :
                        brep.embedding_lines( corner ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines in which the Corner is internal" )
            .def( "nb_embedding_surfaces_of_corner",
                static_cast< index_t ( BRep::* )( const Corner3D& ) const >(
                    &BRep::nb_embedding_surfaces ),
                pybind11::arg( "corner" ),
                "Return the number of Surfaces in which the Corner is "
                "internal" )
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
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces in which the Corner is internal" )
            .def( "nb_embedding_surfaces_of_line",
                static_cast< index_t ( BRep::* )( const Line3D& ) const >(
                    &BRep::nb_embedding_surfaces ),
                pybind11::arg( "line" ),
                "Return the number of Surfaces in which the Line is internal" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces in which the Line is internal" )
            .def( "nb_embedding_blocks_of_corner",
                static_cast< index_t ( BRep::* )( const Corner3D& ) const >(
                    &BRep::nb_embedding_blocks ),
                pybind11::arg( "corner" ),
                "Return the number of Blocks in which the Corner is internal" )
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
                pybind11::arg( "corner" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks in which the Corner is internal" )
            .def( "nb_embedding_blocks_of_line",
                static_cast< index_t ( BRep::* )( const Line3D& ) const >(
                    &BRep::nb_embedding_blocks ),
                pybind11::arg( "line" ),
                "Return the number of Blocks in which the Line is internal" )
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
                pybind11::arg( "line" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks in which the Line is internal" )
            .def( "nb_embedding_blocks_of_surface",
                static_cast< index_t ( BRep::* )( const Surface3D& ) const >(
                    &BRep::nb_embedding_blocks ),
                pybind11::arg( "surface" ),
                "Return the number of Blocks in which the Surface is internal" )
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
                pybind11::arg( "surface" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks in which the Surface is internal" )
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
                pybind11::arg( "boundary" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces belonging to the ModelBoundary" )
            .def(
                "corner_collection_items",
                []( const BRep& brep, const CornerCollection3D& collection ) {
                    std::vector< const Corner3D* > components;
                    for( const auto& component :
                        brep.corner_collection_items( collection ) )
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
                []( const BRep& brep, const LineCollection3D& collection ) {
                    std::vector< const Line3D* > components;
                    for( const auto& component :
                        brep.line_collection_items( collection ) )
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
                []( const BRep& brep, const SurfaceCollection3D& collection ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        brep.surface_collection_items( collection ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "collection" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces belonging to the SurfaceCollection" )
            .def(
                "block_collection_items",
                []( const BRep& brep, const BlockCollection3D& collection ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component :
                        brep.block_collection_items( collection ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "collection" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks belonging to the BlockCollection" )
            .def( "is_line_closed",
                static_cast< bool ( BRep::* )( const Line3D& ) const >(
                    &BRep::is_closed ),
                pybind11::arg( "line" ),
                "Return True if the Line has fewer than two boundary Corners "
                "(it is a loop or has none), False otherwise" )
            .def( "is_surface_closed",
                static_cast< bool ( BRep::* )( const Surface3D& ) const >(
                    &BRep::is_closed ),
                pybind11::arg( "surface" ),
                "Return True if the Surface has no boundary Line, False "
                "otherwise" )
            .def( "is_line_boundary",
                static_cast< bool ( BRep::* )( const Corner3D&, const Line3D& )
                        const >( &BRep::is_boundary ),
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Return True if the Corner is a boundary of the Line" )
            .def( "is_surface_boundary",
                static_cast< bool ( BRep::* )( const Line3D&, const Surface3D& )
                        const >( &BRep::is_boundary ),
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Return True if the Line is a boundary of the Surface" )
            .def( "is_block_boundary",
                static_cast< bool ( BRep::* )( const Surface3D&,
                    const Block3D& ) const >( &BRep::is_boundary ),
                pybind11::arg( "surface" ), pybind11::arg( "block" ),
                "Return True if the Surface is a boundary of the Block" )
            .def( "is_corner_in_line_internals",
                static_cast< bool ( BRep::* )( const Corner3D&, const Line3D& )
                        const >( &BRep::is_internal ),
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Return True if the Corner is internal to the Line" )
            .def( "is_corner_in_surface_internals",
                static_cast< bool ( BRep::* )( const Corner3D&,
                    const Surface3D& ) const >( &BRep::is_internal ),
                pybind11::arg( "corner" ), pybind11::arg( "surface" ),
                "Return True if the Corner is internal to the Surface" )
            .def( "is_line_in_surface_internals",
                static_cast< bool ( BRep::* )( const Line3D&, const Surface3D& )
                        const >( &BRep::is_internal ),
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Return True if the Line is internal to the Surface" )
            .def( "is_corner_in_block_internals",
                static_cast< bool ( BRep::* )( const Corner3D&, const Block3D& )
                        const >( &BRep::is_internal ),
                pybind11::arg( "corner" ), pybind11::arg( "block" ),
                "Return True if the Corner is internal to the Block" )
            .def( "is_line_in_block_internals",
                static_cast< bool ( BRep::* )( const Line3D&, const Block3D& )
                        const >( &BRep::is_internal ),
                pybind11::arg( "line" ), pybind11::arg( "block" ),
                "Return True if the Line is internal to the Block" )
            .def( "is_surface_in_block_internals",
                static_cast< bool ( BRep::* )( const Surface3D&,
                    const Block3D& ) const >( &BRep::is_internal ),
                pybind11::arg( "surface" ), pybind11::arg( "block" ),
                "Return True if the Surface is internal to the Block" )
            .def( "is_model_boundary_item", &BRep::is_model_boundary_item,
                pybind11::arg( "surface" ), pybind11::arg( "boundary" ),
                "Return True if the Surface belongs to the ModelBoundary" )
            .def( "is_corner_collection_item", &BRep::is_corner_collection_item,
                pybind11::arg( "corner" ), pybind11::arg( "corner_collection" ),
                "Return True if the Corner belongs to the CornerCollection" )
            .def( "is_line_collection_item", &BRep::is_line_collection_item,
                pybind11::arg( "line" ), pybind11::arg( "line_collection" ),
                "Return True if the Line belongs to the LineCollection" )
            .def( "is_surface_collection_item",
                &BRep::is_surface_collection_item, pybind11::arg( "surface" ),
                pybind11::arg( "surface_collection" ),
                "Return True if the Surface belongs to the SurfaceCollection" )
            .def( "is_block_collection_item", &BRep::is_block_collection_item,
                pybind11::arg( "block" ), pybind11::arg( "block_collection" ),
                "Return True if the Block belongs to the BlockCollection" )
            .def( "bounding_box", &BRep::bounding_box,
                "Return the bounding box of the meshes of all the components, "
                "active or not; raise an error if the model has no vertex" )
            .def( "native_extension", &BRep::native_extension,
                "Return the extension of the native file format of the class" )
            .def( "brep_component", &BRep::component,
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component of the BRep with the given uuid, "
                "whatever its type (an instance of its own class: Corner3D, "
                "Block3D, ...); raise an error if the BRep has none" );
    }
} // namespace geode
