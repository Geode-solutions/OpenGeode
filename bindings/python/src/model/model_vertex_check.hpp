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

#pragma once

#include "../common.hpp"

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/component_registry.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    namespace detail
    {
        /*!
         * Return the number of vertices of the mesh of the component, NO_ID
         * if it is not a mesh component of the model (one lookup by uuid).
         */
        template < typename Model >
        index_t nb_component_mesh_vertices(
            const Model& model, const ComponentID& component_id )
        {
            constexpr auto dimension = Model::dim;
            const auto& type = component_id.type;
            const auto& id = component_id.id;
            if( type == Corner< dimension >::component_type_static() )
            {
                return model.has_corner( id )
                           ? model.corner( id ).mesh().nb_vertices()
                           : NO_ID;
            }
            if( type == Line< dimension >::component_type_static() )
            {
                return model.has_line( id )
                           ? model.line( id ).mesh().nb_vertices()
                           : NO_ID;
            }
            if( type == Surface< dimension >::component_type_static() )
            {
                return model.has_surface( id )
                           ? model.surface( id ).mesh().nb_vertices()
                           : NO_ID;
            }
            if constexpr( dimension == 3 )
            {
                if( type == Block< dimension >::component_type_static() )
                {
                    return model.has_block( id )
                               ? model.block( id ).mesh().nb_vertices()
                               : NO_ID;
                }
            }
            return NO_ID;
        }

        template < typename Model >
        void check_component_mesh_vertex( std::string_view name,
            const Model& model,
            const ComponentMeshVertex& component_vertex )
        {
            const auto& component_id = component_vertex.component_id;
            const auto nb_vertices =
                nb_component_mesh_vertices( model, component_id );
            OpenGeodeModelException::check_exception( nb_vertices != NO_ID,
                nullptr, OpenGeodeException::TYPE::data, "[", name,
                "] The component ", component_id.string(),
                " is not a mesh component of the model" );
            check_index( name, "component vertex", component_vertex.vertex,
                nb_vertices );
        }

        template < typename Model >
        bool check_model_vertex( std::string_view name,
            const pybind11::handle& model,
            const ComponentMeshVertex& component_vertex )
        {
            if( pybind11::isinstance< Model >( model ) )
            {
                check_component_mesh_vertex(
                    name, model.cast< const Model& >(), component_vertex );
                return true;
            }
            return false;
        }
    } // namespace detail

    /*!
     * Check that the component of the vertex is a mesh component of the model
     * (OpenGeodeDataError otherwise) and that the vertex is in its mesh
     * (IndexError otherwise). Nothing is checked if @a model is not a BRep or
     * a Section, as a VertexIdentifier alone does not know the meshes.
     */
    inline void check_model_component_vertex( std::string_view name,
        const pybind11::handle& model,
        const ComponentMeshVertex& component_vertex )
    {
        if( !detail::check_model_vertex< BRep >(
                name, model, component_vertex ) )
        {
            detail::check_model_vertex< Section >(
                name, model, component_vertex );
        }
    }

    /*!
     * Check that the unique vertex exists in the VertexIdentifier of the model
     * (IndexError otherwise).
     */
    inline void check_model_unique_vertex( std::string_view name,
        const pybind11::handle& model,
        index_t unique_vertex_id )
    {
        check_index( name, "unique vertex", unique_vertex_id,
            model.cast< const VertexIdentifier& >().nb_unique_vertices() );
    }

    /*!
     * Call policy of the constructor of a builder, to give to the builder the
     * `_model` attribute (the class needs dynamic_attr): the model it modifies
     * (the second argument of the constructor), to check the arguments of the
     * functions of the VertexIdentifierBuilder. As a call policy, it keeps the
     * new-style pybind11 constructor, which a replacement of __init__ would
     * turn into an old-style one (deprecated by pybind11). A class deriving
     * from the builder can use it in its own constructor, if not it has no
     * `_model` and its calls are not checked.
     */
    struct remember_model
    {
    };
} // namespace geode

namespace pybind11::detail
{
    template <>
    struct process_attribute< geode::remember_model >
        : process_attribute_default< geode::remember_model >
    {
        static void postcall( function_call& call, handle /*unused*/ )
        {
            // The constructor is called with (self, model), and self is not
            // in the arguments of a new-style constructor
            call.init_self.attr( "_model" ) = call.args[1];
        }
    };
} // namespace pybind11::detail
