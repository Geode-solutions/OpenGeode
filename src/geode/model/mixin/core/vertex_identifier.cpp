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

#include <geode/model/mixin/core/vertex_identifier.hpp>

#include <atomic>
#include <fstream>

#include <async++.h>

#include <bitsery/ext/std_map.h>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/node_hash_map.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/mesh/builder/geode/geode_vertex_set_builder.hpp>
#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/geode/geode_vertex_set.hpp>
#include <geode/mesh/core/mesh_element.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    ComponentMeshVertex::ComponentMeshVertex(
        ComponentID component_id_in, index_t vertex_id_in )
        : component_id( std::move( component_id_in ) ), vertex( vertex_id_in )
    {
    }

    ComponentMeshVertex::ComponentMeshVertex()
        : component_id( bitsery::Access::create< ComponentID >() )
    {
    }

    ComponentMeshVertex::~ComponentMeshVertex() = default;

    MeshVertex ComponentMeshVertex::mesh_vertex() const
    {
        return { component_id.id, vertex };
    }

    bool ComponentMeshVertex::operator==(
        const ComponentMeshVertex& other ) const
    {
        return component_id == other.component_id && vertex == other.vertex;
    }

    bool ComponentMeshVertex::operator<(
        const ComponentMeshVertex& other ) const
    {
        if( component_id != other.component_id )
        {
            return component_id < other.component_id;
        }
        return vertex < other.vertex;
    }

    template < typename Archive >
    void ComponentMeshVertex::serialize( Archive& serializer )
    {
        serializer.ext(
            *this, Growable< Archive, ComponentMeshVertex >{
                       { []( Archive& archive,
                             ComponentMeshVertex& component_mesh_vertex ) {
                           archive.object( component_mesh_vertex.component_id );
                           archive.value4b( component_mesh_vertex.vertex );
                       } } } );
    }

    class VertexIdentifier::Impl
    {
        constexpr static auto UNIQUE_VERTICES_NAME = "unique vertices";

        struct RegisteredComponent
        {
            explicit RegisteredComponent(
                std::shared_ptr< VariableAttribute< index_t > > attribute_in )
                : attribute( std::move( attribute_in ) )
            {
            }

            std::shared_ptr< VariableAttribute< index_t > > attribute;
            /*!
             * Number of ComponentMeshVertex of this component stored in
             * component_vertices_. Used to detect inconsistencies between
             * the component mesh attribute and component_vertices_.
             */
            std::atomic< index_t > nb_component_mesh_vertices{ 0 };
        };

    public:
        Impl()
        {
            AttributeProperties attribute_properties;
            attribute_properties.assignable = false;
            attribute_properties.interpolable = false;
            attribute_properties.transferable = false;
            AttributeValues< std::vector< ComponentMeshVertex > >
                component_vertices_attribute_values;
            component_vertices_attribute_values.default_value = {};
            const auto unique_vertices_attribute_id =
                unique_vertices_.vertex_attribute_manager()
                    .create_attribute< VariableAttribute,
                        std::vector< ComponentMeshVertex > >(
                        "component vertices",
                        component_vertices_attribute_values,
                        attribute_properties );
            component_vertices_ = unique_vertices_.vertex_attribute_manager()
                                      .find_attribute< VariableAttribute,
                                          std::vector< ComponentMeshVertex > >(
                                          unique_vertices_attribute_id );
        }

        Impl( BITSERY ) {}

        index_t nb_unique_vertices() const
        {
            return unique_vertices_.nb_vertices();
        }

        bool is_unique_vertex_isolated( index_t unique_vertex_id ) const
        {
            return component_mesh_vertices( unique_vertex_id ).empty();
        }

        const std::vector< ComponentMeshVertex >& component_mesh_vertices(
            index_t unique_vertex_id ) const
        {
            OpenGeodeModelException::check_assertion(
                unique_vertex_id < nb_unique_vertices(),
                "[VertexIdentifier::component_mesh_vertices] Given "
                "unique_vertex_id is bigger than the number of unique "
                "vertices." );
            return component_vertices_->value( unique_vertex_id );
        }

        index_t unique_vertex(
            const uuid& component_id, const index_t vertex_id ) const
        {
            return vertex2unique_vertex_.at( component_id )
                .attribute->value( vertex_id );
        }

        bool has_component_mesh_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const
        {
            const auto& component_vertices =
                component_mesh_vertices( unique_vertex_id );
            for( const auto& component_vertex : component_vertices )
            {
                if( component_vertex.component_id.type == type )
                {
                    return true;
                }
            }
            return false;
        }

        bool has_component_mesh_vertices(
            index_t unique_vertex_id, const uuid& component_id ) const
        {
            for( const auto& component_vertex :
                component_mesh_vertices( unique_vertex_id ) )
            {
                if( component_vertex.component_id.id == component_id )
                {
                    return true;
                }
            }
            return false;
        }

        const uuid& unique_vertex_attribute_id() const
        {
            return unique_vertex_id_;
        }

        template < typename MeshComponent >
        void register_component( const MeshComponent& component )
        {
            AttributeProperties attribute_properties;
            attribute_properties.assignable = false;
            attribute_properties.interpolable = false;
            attribute_properties.transferable = false;
            AttributeValues< index_t > unique_vertex_attribute_values;
            unique_vertex_attribute_values.default_value = NO_ID;
            unique_vertex_attribute_values.no_value = NO_ID;
            const auto& mesh = component.mesh();
            mesh.vertex_attribute_manager()
                .template create_attribute< VariableAttribute, index_t >(
                    UNIQUE_VERTICES_NAME, unique_vertex_id_,
                    unique_vertex_attribute_values, attribute_properties );
            const auto [_, inserted] =
                vertex2unique_vertex_.try_emplace( component.id(),
                    mesh.vertex_attribute_manager()
                        .template find_attribute< VariableAttribute, index_t >(
                            unique_vertex_id_ ) );
            OpenGeodeModelException::check_exception( inserted,
                component.component_id(), OpenGeodeException::TYPE::data,
                "[VertexIdentifier::register_component] Component ",
                component.id().string(), " is already registered." );
        }

        template < typename MeshComponent >
        void load_component( const MeshComponent& component )
        {
            const auto& mesh = component.mesh();
            if( !mesh.vertex_attribute_manager().attribute_exists(
                    unique_vertex_id_ ) )
            {
                import_old_attribute_based_on_name(
                    mesh.vertex_attribute_manager(), mesh.nb_vertices() );
            }
            const auto [_, inserted] =
                vertex2unique_vertex_.try_emplace( component.id(),
                    mesh.vertex_attribute_manager()
                        .template find_attribute< VariableAttribute, index_t >(
                            unique_vertex_id_ ) );
            OpenGeodeModelException::check_exception( inserted,
                component.component_id(), OpenGeodeException::TYPE::data,
                "[VertexIdentifier::load_component] Component ",
                component.id().string(), " is already registered." );
        }

        template < typename MeshComponent >
        void unregister_component( const MeshComponent& component )
        {
            const auto& mesh = component.mesh();
            const auto it = vertex2unique_vertex_.find( component.id() );
            OpenGeodeModelException::check_exception(
                it != vertex2unique_vertex_.end(), component.component_id(),
                OpenGeodeException::TYPE::data,
                "[VertexIdentifier::unregister_component] Component ",
                component.id().string(), " is not registered." );
            remove_component_vertices(
                component.id(), it->second, mesh.nb_vertices() );
            const auto attribute_id = it->second.attribute->id();
            mesh.vertex_attribute_manager().delete_attribute( attribute_id );
            vertex2unique_vertex_.erase( it );
        }

        index_t create_unique_vertex()
        {
            return VertexSetBuilder::create( unique_vertices_ )
                ->create_vertex();
        }

        index_t create_unique_vertices( const index_t nb )
        {
            return VertexSetBuilder::create( unique_vertices_ )
                ->create_vertices( nb );
        }

        void set_unique_vertex( ComponentMeshVertex component_vertex_id,
            const index_t unique_vertex_id )
        {
            OpenGeodeModelException::check_assertion(
                unique_vertex_id < nb_unique_vertices(),
                "[VertexIdentifier::set_unique_vertex] Unique vertex ",
                unique_vertex_id, " does not exist (nb=", nb_unique_vertices(),
                ")" );
            const auto& old_unique_id =
                vertex2unique_vertex_.at( component_vertex_id.component_id.id )
                    .attribute->value( component_vertex_id.vertex );
            if( old_unique_id != NO_ID )
            {
                unset_unique_vertex( component_vertex_id, old_unique_id );
            }
            auto& component =
                vertex2unique_vertex_.at( component_vertex_id.component_id.id );
            component.attribute->set_value(
                component_vertex_id.vertex, unique_vertex_id );
            component_vertices_->modify_value( unique_vertex_id,
                [&component_vertex_id, &component](
                    std::vector< ComponentMeshVertex >& value ) {
                    if( absl::c_find( value, component_vertex_id )
                        == value.end() )
                    {
                        value.emplace_back( std::move( component_vertex_id ) );
                        component.nb_component_mesh_vertices.fetch_add(
                            1, std::memory_order_relaxed );
                    }
                } );
        }

        void unset_unique_vertex(
            const ComponentMeshVertex& component_vertex_id,
            const index_t unique_vertex_id )
        {
            auto& component =
                vertex2unique_vertex_.at( component_vertex_id.component_id.id );
            component.attribute->set_value( component_vertex_id.vertex, NO_ID );
            const auto& vertices =
                component_vertices_->value( unique_vertex_id );
            const auto it = absl::c_find( vertices, component_vertex_id );
            if( it == vertices.end() )
            {
                return;
            }
            component.nb_component_mesh_vertices.fetch_sub(
                1, std::memory_order_relaxed );
            component_vertices_->modify_value( unique_vertex_id,
                [&it]( std::vector< ComponentMeshVertex >& value ) {
                    value.erase(
                        // workaround for gcc < 4.9
                        value.begin() + ( it - value.cbegin() ) );
                } );
        }

        void update_unique_vertices( const ComponentID& component_id,
            absl::Span< const index_t > old2new )
        {
            const auto component_it =
                vertex2unique_vertex_.find( component_id.id );
            auto* component = component_it == vertex2unique_vertex_.end()
                                  ? nullptr
                                  : &component_it->second;
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices() ),
                [this, &component_id, &old2new, component]( index_t uv ) {
                    if( !has_component_mesh_vertices( uv, component_id.id ) )
                    {
                        return;
                    }
                    const auto& all_vertices = component_vertices_->value( uv );
                    std::vector< bool > to_delete( all_vertices.size(), false );
                    bool need_to_delete{ false };
                    for( const auto v : Indices{ all_vertices } )
                    {
                        const auto& cmv = all_vertices[v];
                        if( cmv.component_id.id != component_id.id )
                        {
                            continue;
                        }
                        const auto new_id = old2new[cmv.vertex];
                        if( new_id == NO_ID )
                        {
                            to_delete[v] = true;
                            need_to_delete = true;
                        }
                        else
                        {
                            component_vertices_->modify_value( uv,
                                [v, new_id]( std::vector< ComponentMeshVertex >&
                                        vertices ) {
                                    vertices[v].vertex = new_id;
                                } );
                        }
                    }
                    if( need_to_delete )
                    {
                        if( component )
                        {
                            component->nb_component_mesh_vertices.fetch_sub(
                                static_cast< index_t >(
                                    absl::c_count( to_delete, true ) ),
                                std::memory_order_relaxed );
                        }
                        component_vertices_->modify_value( uv,
                            [&to_delete](
                                std::vector< ComponentMeshVertex >& vertices ) {
                                delete_vector_elements( to_delete, vertices );
                            } );
                    }
                } );
        }

        std::vector< index_t > delete_isolated_vertices()
        {
            std::vector< bool > to_delete( nb_unique_vertices(), false );
            absl::flat_hash_map< uuid, std::vector< index_t > >
                components_vertices;
            for( const auto v : Range{ nb_unique_vertices() } )
            {
                if( is_unique_vertex_isolated( v ) )
                {
                    to_delete[v] = true;
                    continue;
                }
                for( const auto& cmv : component_mesh_vertices( v ) )
                {
                    components_vertices[cmv.component_id.id].emplace_back(
                        cmv.vertex );
                }
            }
            const auto old2new = VertexSetBuilder::create( unique_vertices_ )
                                     ->delete_vertices( to_delete );
            for( const auto& component_vertices : components_vertices )
            {
                auto& attribute =
                    vertex2unique_vertex_.at( component_vertices.first )
                        .attribute;
                for( const auto v : component_vertices.second )
                {
                    const auto value = attribute->value( v );
                    if( value == NO_ID )
                    {
                        continue;
                    }
                    attribute->set_value( v, old2new[value] );
                }
            }
            return old2new;
        }

        void save( std::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeModelException::check_exception(
                std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::save] Error while writing file: ",
                filename );
        }

        void load( std::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            nb_component_mesh_vertices_up_to_date_ = false;
            const auto& adapter = archive.adapter();
            OpenGeodeModelException::check_exception(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::load] Error while reading file: ",
                filename );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         archive.object( impl.unique_vertices_ );
                         archive.ext( impl.component_vertices_,
                             bitsery::ext::StdSmartPtr{} );
                         absl::flat_hash_map< uuid,
                             std::shared_ptr< VariableAttribute< index_t > > >
                             old_map;
                         archive.ext( old_map,
                             bitsery::ext::StdMap{ old_map.max_size() },
                             []( Archive& archive2, uuid& id,
                                 std::shared_ptr< VariableAttribute<
                                     index_t > >& attribute ) {
                                 archive2.object( id );
                                 archive2.ext(
                                     attribute, bitsery::ext::StdSmartPtr{} );
                             } );
                         const auto& old_component_vertices_properties =
                             impl.component_vertices_->properties();
                         impl.component_vertices_->set_properties(
                             { old_component_vertices_properties.assignable,
                                 old_component_vertices_properties.interpolable,
                                 false } );
                     },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.unique_vertices_ );
                            archive.ext( impl.component_vertices_,
                                bitsery::ext::StdSmartPtr{} );
                            absl::flat_hash_map< uuid,
                                std::shared_ptr<
                                    VariableAttribute< index_t > > >
                                old_map;
                            archive.ext( old_map,
                                bitsery::ext::StdMap{ old_map.max_size() },
                                []( Archive& archive2, uuid& id,
                                    std::shared_ptr< VariableAttribute<
                                        index_t > >& attribute ) {
                                    archive2.object( id );
                                    archive2.ext( attribute,
                                        bitsery::ext::StdSmartPtr{} );
                                } );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.unique_vertices_ );
                            archive.ext( impl.component_vertices_,
                                bitsery::ext::StdSmartPtr{} );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.unique_vertices_ );
                            archive.ext( impl.component_vertices_,
                                bitsery::ext::StdSmartPtr{} );
                            archive.object( impl.unique_vertex_id_ );
                        } } } );
        }

        /*!
         * Remove every ComponentMeshVertex of the given component from
         * component_vertices_. Only the unique vertices referenced by the
         * component mesh attribute are visited. If the number of removed
         * ComponentMeshVertex does not match the number registered for this
         * component (i.e. the attribute and component_vertices_ are not
         * consistent), all the unique vertices are scanned.
         */
        void remove_component_vertices( const uuid& component_id,
            RegisteredComponent& component,
            index_t nb_component_vertices )
        {
            update_nb_component_mesh_vertices();
            const auto nb_expected = component.nb_component_mesh_vertices.load(
                std::memory_order_relaxed );
            if( nb_expected == 0 )
            {
                return;
            }
            std::vector< index_t > unique_vertices;
            unique_vertices.reserve( nb_component_vertices );
            const auto nb_uv = nb_unique_vertices();
            for( const auto v : Range{ nb_component_vertices } )
            {
                const auto unique_vertex = component.attribute->value( v );
                if( unique_vertex < nb_uv )
                {
                    unique_vertices.push_back( unique_vertex );
                }
            }
            absl::c_sort( unique_vertices );
            unique_vertices.erase(
                std::unique( unique_vertices.begin(), unique_vertices.end() ),
                unique_vertices.end() );
            std::atomic< index_t > nb_removed{ 0 };
            async::parallel_for(
                async::irange( index_t{ 0 },
                    static_cast< index_t >( unique_vertices.size() ) ),
                [this, &component_id, &unique_vertices, &nb_removed](
                    index_t i ) {
                    nb_removed.fetch_add(
                        filter_unique_vertex(
                            unique_vertices[i], component_id ),
                        std::memory_order_relaxed );
                } );
            if( nb_removed.load() != nb_expected )
            {
                filter_component_vertices( component_id );
            }
        }

        index_t filter_unique_vertex(
            index_t unique_vertex_id, const uuid& component_id )
        {
            const auto& component_mesh_vertices =
                component_vertices_->value( unique_vertex_id );
            const auto is_component = [&component_id](
                                          const ComponentMeshVertex& cmv ) {
                return cmv.component_id.id == component_id;
            };
            if( absl::c_none_of( component_mesh_vertices, is_component ) )
            {
                return 0;
            }
            index_t nb_removed{ 0 };
            component_vertices_->modify_value( unique_vertex_id,
                [&is_component, &nb_removed](
                    std::vector< ComponentMeshVertex >& vertices ) {
                    const auto old_size = vertices.size();
                    vertices.erase( std::remove_if( vertices.begin(),
                                        vertices.end(), is_component ),
                        vertices.end() );
                    nb_removed =
                        static_cast< index_t >( old_size - vertices.size() );
                } );
            return nb_removed;
        }

        void filter_component_vertices( const uuid& component_id )
        {
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices() ),
                [this, &component_id]( index_t uv_id ) {
                    filter_unique_vertex( uv_id, component_id );
                } );
        }

        void update_nb_component_mesh_vertices()
        {
            if( nb_component_mesh_vertices_up_to_date_ )
            {
                return;
            }
            for( auto& [_, component] : vertex2unique_vertex_ )
            {
                component.nb_component_mesh_vertices.store(
                    0, std::memory_order_relaxed );
            }
            for( const auto uv : Range{ nb_unique_vertices() } )
            {
                for( const auto& cmv : component_vertices_->value( uv ) )
                {
                    const auto it =
                        vertex2unique_vertex_.find( cmv.component_id.id );
                    if( it != vertex2unique_vertex_.end() )
                    {
                        it->second.nb_component_mesh_vertices.fetch_add(
                            1, std::memory_order_relaxed );
                    }
                }
            }
            nb_component_mesh_vertices_up_to_date_ = true;
        }

        void import_old_attribute_based_on_name(
            AttributeManager& vertex_attribute_manager,
            const index_t nb_vertices )
        {
            const auto unique_vertices_ids =
                vertex_attribute_manager.attribute_ids_matching_name(
                    UNIQUE_VERTICES_NAME );
            OpenGeodeModelException::check_exception(
                unique_vertices_ids.has_value(), nullptr,
                OpenGeodeException::TYPE::data,
                "[VertexIdentifier::load_component] Unique vertices "
                "attribute not found." );
            OpenGeodeModelException::check_exception(
                unique_vertices_ids.value().size() == 1, nullptr,
                OpenGeodeException::TYPE::data,
                "[VertexIdentifier::load_component] Unique vertices "
                "attribute is not unique." );
            const auto old_attribute =
                vertex_attribute_manager
                    .template find_attribute< VariableAttribute, index_t >(
                        unique_vertices_ids.value().front() );
            vertex_attribute_manager
                .template create_attribute< VariableAttribute, index_t >(
                    UNIQUE_VERTICES_NAME, unique_vertex_id_,
                    old_attribute->default_values(),
                    old_attribute->properties() );
            const auto new_attribute =
                vertex_attribute_manager
                    .template find_attribute< VariableAttribute, index_t >(
                        unique_vertex_id_ );
            for( const auto vertex : Range{ nb_vertices } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
            vertex_attribute_manager.delete_attribute(
                unique_vertices_ids.value().front() );
        }

    private:
        OpenGeodeVertexSet unique_vertices_;
        std::shared_ptr<
            VariableAttribute< std::vector< ComponentMeshVertex > > >
            component_vertices_;
        absl::node_hash_map< uuid, RegisteredComponent > vertex2unique_vertex_;
        geode::uuid unique_vertex_id_;
        bool nb_component_mesh_vertices_up_to_date_{ true };
    };

    VertexIdentifier::VertexIdentifier() = default;
    VertexIdentifier::VertexIdentifier( BITSERY bitsery ) : impl_{ bitsery } {}
    VertexIdentifier::VertexIdentifier( VertexIdentifier&& ) noexcept = default;

    VertexIdentifier& VertexIdentifier::operator=(
        VertexIdentifier&& ) noexcept = default;

    VertexIdentifier::~VertexIdentifier() = default;

    index_t VertexIdentifier::nb_unique_vertices() const
    {
        return impl_->nb_unique_vertices();
    }

    bool VertexIdentifier::is_unique_vertex_isolated(
        index_t unique_vertex_id ) const
    {
        return impl_->is_unique_vertex_isolated( unique_vertex_id );
    }

    const std::vector< ComponentMeshVertex >&
        VertexIdentifier::component_mesh_vertices(
            index_t unique_vertex_id ) const
    {
        return impl_->component_mesh_vertices( unique_vertex_id );
    }

    index_t VertexIdentifier::unique_vertex(
        const ComponentMeshVertex& component_mesh_vertex ) const
    {
        return impl_->unique_vertex( component_mesh_vertex.component_id.id,
            component_mesh_vertex.vertex );
    }

    bool VertexIdentifier::has_component_mesh_vertices(
        index_t unique_vertex_id, const ComponentType& type ) const
    {
        return impl_->has_component_mesh_vertices( unique_vertex_id, type );
    }

    bool VertexIdentifier::has_component_mesh_vertices(
        index_t unique_vertex_id, const uuid& component_id ) const
    {
        return impl_->has_component_mesh_vertices(
            unique_vertex_id, component_id );
    }

    const uuid& VertexIdentifier::unique_vertex_attribute_id() const
    {
        return impl_->unique_vertex_attribute_id();
    }

    template < typename MeshComponent >
    void VertexIdentifier::load_mesh_component(
        const MeshComponent& component, BuilderKey /*key*/ )
    {
        impl_->load_component( component );
    }

    template < typename MeshComponent >
    void VertexIdentifier::register_mesh_component(
        const MeshComponent& component, BuilderKey /*key*/ )
    {
        impl_->register_component( component );
    }

    template < typename MeshComponent >
    void VertexIdentifier::unregister_mesh_component(
        const MeshComponent& component, BuilderKey /*key*/ )
    {
        impl_->unregister_component( component );
    }

    index_t VertexIdentifier::create_unique_vertex( BuilderKey /*key*/ )
    {
        return impl_->create_unique_vertex();
    }

    index_t VertexIdentifier::create_unique_vertices(
        index_t nb, BuilderKey /*key*/ )
    {
        return impl_->create_unique_vertices( nb );
    }

    void VertexIdentifier::set_unique_vertex(
        ComponentMeshVertex component_vertex_id,
        index_t unique_vertex_id,
        BuilderKey /*key*/ )
    {
        impl_->set_unique_vertex(
            std::move( component_vertex_id ), unique_vertex_id );
    }

    void VertexIdentifier::unset_unique_vertex(
        const ComponentMeshVertex& component_vertex_id,
        index_t unique_vertex_id,
        BuilderKey /*key*/ )
    {
        impl_->unset_unique_vertex( component_vertex_id, unique_vertex_id );
    }

    void VertexIdentifier::update_unique_vertices(
        const ComponentID& component_id,
        absl::Span< const index_t > old2new,
        BuilderKey /*key*/ )
    {
        impl_->update_unique_vertices( component_id, old2new );
    }

    void VertexIdentifier::save_unique_vertices(
        std::string_view directory ) const
    {
        impl_->save( directory );
    }

    void VertexIdentifier::load_unique_vertices(
        std::string_view directory, BuilderKey /*key*/ )
    {
        return impl_->load( directory );
    }

    std::vector< index_t > VertexIdentifier::delete_isolated_vertices(
        BuilderKey /*key*/ )
    {
        return impl_->delete_isolated_vertices();
    }

    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Corner2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Corner3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Line2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Line3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Surface2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Surface3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::load_mesh_component(
        const Block3D&, BuilderKey /*key*/ );

    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface2D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface3D&, BuilderKey /*key*/ );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Block3D&, BuilderKey /*key*/ );

    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Corner2D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Corner3D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Line2D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Line3D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Surface2D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Surface3D&, BuilderKey /*key*/ );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Block3D&, BuilderKey /*key*/ );

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, ComponentMeshVertex );
} // namespace geode

namespace std
{
    size_t hash< geode::ComponentMeshVertex >::operator()(
        const geode::ComponentMeshVertex& cmv ) const
    {
        return absl::Hash< geode::uuid >()( cmv.component_id.id )
               ^ absl::Hash< geode::index_t >()( cmv.vertex );
    }
} // namespace std
