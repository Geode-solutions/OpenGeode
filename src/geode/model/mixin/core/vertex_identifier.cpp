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
#include <filesystem>
#include <fstream>

#include <async++.h>

#include <bitsery/ext/std_map.h>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bitsery_archive.hpp>

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
        : component_id( BITSERY::constructor )
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
            RegisteredComponent( ComponentID component_id_in,
                std::shared_ptr< VariableAttribute< index_t > > attribute_in,
                index_t registration_in )
                : component_id( std::move( component_id_in ) ),
                  attribute( std::move( attribute_in ) ),
                  registration( registration_in )
            {
            }

            RegisteredComponent( RegisteredComponent&& other ) noexcept
                : component_id( std::move( other.component_id ) ),
                  attribute( std::move( other.attribute ) ),
                  registration( other.registration ),
                  nb_component_mesh_vertices(
                      other.nb_component_mesh_vertices.load(
                          std::memory_order_relaxed ) )
            {
            }

            ComponentID component_id;
            std::shared_ptr< VariableAttribute< index_t > > attribute;
            /*!
             * Registration rank, giving a deterministic component order to
             * the rebuild of component_vertices_
             */
            index_t registration;
            /*!
             * Number of ComponentMeshVertex of this component stored in
             * component_vertices_. Used to detect inconsistencies between
             * the component mesh attribute and component_vertices_.
             */
            std::atomic< index_t > nb_component_mesh_vertices{ 0 };
        };

    public:
        Impl() = default;

        Impl( BITSERY bitsery ) : unique_vertex_id_{ bitsery } {}

        index_t nb_unique_vertices() const
        {
            return static_cast< index_t >( component_vertices_.size() );
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
            return component_vertices_[unique_vertex_id];
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
            const auto [_, inserted] = vertex2unique_vertex_.try_emplace(
                component.id(), component.component_id(),
                mesh.vertex_attribute_manager()
                    .template find_attribute< VariableAttribute, index_t >(
                        unique_vertex_id_ ),
                nb_registrations_ );
            nb_registrations_++;
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
            const auto [_, inserted] = vertex2unique_vertex_.try_emplace(
                component.id(), component.component_id(),
                mesh.vertex_attribute_manager()
                    .template find_attribute< VariableAttribute, index_t >(
                        unique_vertex_id_ ),
                nb_registrations_ );
            nb_registrations_++;
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
            return create_unique_vertices( 1 );
        }

        index_t create_unique_vertices( const index_t nb )
        {
            const auto first = nb_unique_vertices();
            component_vertices_.resize( component_vertices_.size() + nb );
            return first;
        }

        void set_unique_vertex( ComponentMeshVertex component_vertex_id,
            const index_t unique_vertex_id )
        {
            OpenGeodeModelException::check_assertion(
                unique_vertex_id < nb_unique_vertices(),
                "[VertexIdentifier::set_unique_vertex] Unique vertex ",
                unique_vertex_id, " does not exist (nb=", nb_unique_vertices(),
                ")" );
            auto& component =
                vertex2unique_vertex_.at( component_vertex_id.component_id.id );
            const auto old_unique_id =
                component.attribute->value( component_vertex_id.vertex );
            if( old_unique_id != NO_ID )
            {
                unset_unique_vertex(
                    component, component_vertex_id, old_unique_id );
            }
            component.attribute->set_value(
                component_vertex_id.vertex, unique_vertex_id );
            auto& vertices = component_vertices_[unique_vertex_id];
            if( absl::c_find( vertices, component_vertex_id )
                == vertices.end() )
            {
                vertices.emplace_back( std::move( component_vertex_id ) );
                component.nb_component_mesh_vertices.fetch_add(
                    1, std::memory_order_relaxed );
            }
        }

        void unset_unique_vertex(
            const ComponentMeshVertex& component_vertex_id,
            const index_t unique_vertex_id )
        {
            unset_unique_vertex(
                vertex2unique_vertex_.at( component_vertex_id.component_id.id ),
                component_vertex_id, unique_vertex_id );
        }

        void unset_unique_vertex( RegisteredComponent& component,
            const ComponentMeshVertex& component_vertex_id,
            const index_t unique_vertex_id )
        {
            component.attribute->set_value( component_vertex_id.vertex, NO_ID );
            auto& vertices = component_vertices_[unique_vertex_id];
            const auto it = absl::c_find( vertices, component_vertex_id );
            if( it == vertices.end() )
            {
                return;
            }
            component.nb_component_mesh_vertices.fetch_sub(
                1, std::memory_order_relaxed );
            vertices.erase( it );
        }

        void update_unique_vertices( const ComponentID& component_id,
            absl::Span< const index_t > old2new )
        {
            std::atomic< index_t > nb_deleted{ 0 };
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices() ),
                [this, &component_id, &old2new, &nb_deleted]( index_t uv ) {
                    if( !has_component_mesh_vertices( uv, component_id.id ) )
                    {
                        return;
                    }
                    auto& all_vertices = component_vertices_[uv];
                    std::vector< bool > to_delete( all_vertices.size(), false );
                    bool need_to_delete{ false };
                    for( const auto v : Indices{ all_vertices } )
                    {
                        auto& cmv = all_vertices[v];
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
                            cmv.vertex = new_id;
                        }
                    }
                    if( need_to_delete )
                    {
                        nb_deleted.fetch_add(
                            static_cast< index_t >(
                                absl::c_count( to_delete, true ) ),
                            std::memory_order_relaxed );
                        delete_vector_elements( to_delete, all_vertices );
                    }
                } );
            const auto component_it =
                vertex2unique_vertex_.find( component_id.id );
            if( component_it != vertex2unique_vertex_.end() )
            {
                component_it->second.nb_component_mesh_vertices.fetch_sub(
                    nb_deleted.load( std::memory_order_relaxed ),
                    std::memory_order_relaxed );
            }
        }

        std::vector< index_t > delete_isolated_vertices()
        {
            const auto nb_vertices = nb_unique_vertices();
            std::vector< bool > to_delete( nb_vertices, false );
            auto first_deleted = nb_vertices;
            for( const auto v : Range{ nb_vertices } )
            {
                if( is_unique_vertex_isolated( v ) )
                {
                    to_delete[v] = true;
                    first_deleted = std::min( first_deleted, v );
                }
            }
            if( first_deleted == nb_vertices )
            {
                std::vector< index_t > old2new( nb_vertices );
                absl::c_iota( old2new, index_t{ 0 } );
                return old2new;
            }
            std::vector< index_t > old2new( nb_vertices );
            index_t nb_kept{ 0 };
            for( const auto v : Range{ nb_vertices } )
            {
                if( to_delete[v] )
                {
                    old2new[v] = NO_ID;
                    continue;
                }
                old2new[v] = nb_kept;
                if( nb_kept != v )
                {
                    component_vertices_[nb_kept] =
                        std::move( component_vertices_[v] );
                }
                nb_kept++;
            }
            component_vertices_.resize( nb_kept );
            // Unique vertices before the first deleted one keep their index
            const uuid* last_component{ nullptr };
            VariableAttribute< index_t >* attribute{ nullptr };
            for( const auto uv : Range{ first_deleted, nb_unique_vertices() } )
            {
                for( const auto& cmv : component_vertices_[uv] )
                {
                    if( !last_component
                        || *last_component != cmv.component_id.id )
                    {
                        const auto it =
                            vertex2unique_vertex_.find( cmv.component_id.id );
                        last_component = &cmv.component_id.id;
                        attribute = it == vertex2unique_vertex_.end()
                                        ? nullptr
                                        : it->second.attribute.get();
                    }
                    if( !attribute )
                    {
                        continue;
                    }
                    const auto value = attribute->value( cmv.vertex );
                    if( value == NO_ID )
                    {
                        continue;
                    }
                    attribute->set_value( cmv.vertex, old2new[value] );
                }
            }
            return old2new;
        }

        void repair_component_vertices()
        {
            if( rebuild_component_vertices_ )
            {
                rebuild_component_vertices();
                return;
            }
            // Read-only parallel scan, nothing to repair in a valid model
            std::vector< uint8_t > to_repair( nb_unique_vertices(), 0 );
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices() ),
                [this, &to_repair]( index_t unique_vertex ) {
                    for( const auto& vertex :
                        component_vertices_[unique_vertex] )
                    {
                        if( !is_linked_to_unique_vertex(
                                vertex, unique_vertex ) )
                        {
                            to_repair[unique_vertex] = 1;
                            return;
                        }
                    }
                } );
            // Sequential repair: a component vertex may be listed in several
            // unique vertices, the first one keeps it
            ComponentVerticesRepair repair;
            for( const auto unique_vertex : Range{ nb_unique_vertices() } )
            {
                if( to_repair[unique_vertex] != 0 )
                {
                    repair_unique_vertex( unique_vertex, repair );
                    nb_component_mesh_vertices_up_to_date_ = false;
                }
            }
            repair.log();
        }

        /*!
         * Rebuild component_vertices_ from the component mesh attributes.
         * Components are visited in their registration order (the model
         * component order when loading), then their vertices in increasing
         * order: the result does not depend on hash order.
         */
        void rebuild_component_vertices()
        {
            rebuild_component_vertices_ = false;
            std::vector< RegisteredComponent* > components;
            components.reserve( vertex2unique_vertex_.size() );
            for( auto& [_, component] : vertex2unique_vertex_ )
            {
                components.push_back( &component );
            }
            absl::c_sort( components, []( const RegisteredComponent* lhs,
                                          const RegisteredComponent* rhs ) {
                return lhs->registration < rhs->registration;
            } );
            const auto nb_vertices = nb_unique_vertices();
            std::vector< index_t > nb_component_vertices( nb_vertices, 0 );
            index_t nb_invalid_links{ 0 };
            for( auto* component : components )
            {
                auto& attribute = *component->attribute;
                for( const auto v : Range{ attribute.size() } )
                {
                    const auto unique_vertex = attribute.value( v );
                    if( unique_vertex == NO_ID )
                    {
                        continue;
                    }
                    if( unique_vertex >= nb_vertices )
                    {
                        attribute.set_value( v, NO_ID );
                        nb_invalid_links++;
                        continue;
                    }
                    nb_component_vertices[unique_vertex]++;
                }
            }
            for( const auto unique_vertex : Range{ nb_vertices } )
            {
                auto& vertices = component_vertices_[unique_vertex];
                vertices.clear();
                vertices.reserve( nb_component_vertices[unique_vertex] );
            }
            for( auto* component : components )
            {
                const auto& attribute = *component->attribute;
                index_t nb_vertices_in_component{ 0 };
                for( const auto v : Range{ attribute.size() } )
                {
                    const auto unique_vertex = attribute.value( v );
                    if( unique_vertex == NO_ID )
                    {
                        continue;
                    }
                    component_vertices_[unique_vertex].emplace_back(
                        component->component_id, v );
                    nb_vertices_in_component++;
                }
                component->nb_component_mesh_vertices.store(
                    nb_vertices_in_component, std::memory_order_relaxed );
            }
            nb_component_mesh_vertices_up_to_date_ = true;
            if( nb_invalid_links > 0 )
            {
                Logger::warning( "[VertexIdentifier::load] Removed ",
                    nb_invalid_links,
                    " component vertex links to unknown unique vertices" );
            }
        }

        void save( std::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            std::ofstream file{ std::filesystem::u8path( filename ),
                std::ofstream::binary };
            OpenGeodeModelException::check_exception( file.is_open(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::save] Cannot open file: ", filename );
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeModelException::check_exception(
                file.good() && std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::save] Error while writing file: ",
                filename );
        }

        void load( std::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            const auto content = detail::read_file_content( filename );
            OpenGeodeModelException::check_exception( content.has_value(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::load] Error while reading file: ",
                filename );
            // Versions before 5 store the component vertices and reset this
            // flag
            component_vertices_.clear();
            rebuild_component_vertices_ = true;
            const auto success =
                detail::deserialize_bytes( content->span(), *this );
            nb_component_mesh_vertices_up_to_date_ = false;
            OpenGeodeModelException::check_exception( success, nullptr,
                OpenGeodeException::TYPE::internal,
                "[VertexIdentifier::load] Error while reading file: ",
                filename );
        }

    private:
        friend class bitsery::Access;
        using LegacyComponentVertices =
            VariableAttribute< std::vector< ComponentMeshVertex > >;

        /*!
         * Versions 1 to 4 stored the unique vertices as a vertex set holding
         * the component vertices in an attribute.
         */
        template < typename Archive >
        void serialize_legacy_unique_vertices( Archive& archive )
        {
            OpenGeodeVertexSet unique_vertices{ BITSERY::constructor };
            std::shared_ptr< LegacyComponentVertices > component_vertices;
            archive.object( unique_vertices );
            archive.ext( component_vertices, bitsery::ext::StdSmartPtr{} );
            set_legacy_component_vertices(
                component_vertices, unique_vertices.nb_vertices() );
        }

        template < typename Archive >
        static void serialize_legacy_map( Archive& archive )
        {
            absl::flat_hash_map< uuid,
                std::shared_ptr< VariableAttribute< index_t > > >
                old_map;
            archive.ext( old_map, bitsery::ext::StdMap{ old_map.max_size() },
                []( Archive& archive2, uuid& id,
                    std::shared_ptr< VariableAttribute< index_t > >&
                        attribute ) {
                    archive2.object( id );
                    archive2.ext( attribute, bitsery::ext::StdSmartPtr{} );
                } );
        }

        void set_legacy_component_vertices(
            const std::shared_ptr< LegacyComponentVertices >&
                component_vertices,
            index_t nb_unique_vertices )
        {
            component_vertices_.resize( nb_unique_vertices );
            rebuild_component_vertices_ = false;
            if( !component_vertices )
            {
                return;
            }
            const auto nb_values =
                std::min( nb_unique_vertices, component_vertices->size() );
            for( const auto uv : Range{ nb_values } )
            {
                component_vertices->modify_value( uv,
                    [this, uv]( std::vector< ComponentMeshVertex >& value ) {
                        component_vertices_[uv] = std::move( value );
                    } );
            }
        }

        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         // Not stored in this version
                         impl.unique_vertex_id_ = uuid{};
                         impl.serialize_legacy_unique_vertices( archive );
                         serialize_legacy_map( archive );
                     },
                        []( Archive& archive, Impl& impl ) {
                            // Not stored in this version
                            impl.unique_vertex_id_ = uuid{};
                            impl.serialize_legacy_unique_vertices( archive );
                            serialize_legacy_map( archive );
                        },
                        []( Archive& archive, Impl& impl ) {
                            // Not stored in this version
                            impl.unique_vertex_id_ = uuid{};
                            impl.serialize_legacy_unique_vertices( archive );
                        },
                        []( Archive& archive, Impl& impl ) {
                            impl.serialize_legacy_unique_vertices( archive );
                            archive.object( impl.unique_vertex_id_ );
                        },
                        []( Archive& archive, Impl& impl ) {
                            // Component vertices are not stored, they are
                            // rebuilt from the component mesh attributes
                            // (see rebuild_component_vertices). Loading
                            // starts from an empty Impl, so the size only
                            // changes on reading.
                            auto nb_unique_vertices = impl.nb_unique_vertices();
                            archive.value4b( nb_unique_vertices );
                            archive.object( impl.unique_vertex_id_ );
                            if( nb_unique_vertices
                                != impl.nb_unique_vertices() )
                            {
                                impl.component_vertices_.resize(
                                    nb_unique_vertices );
                            }
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
            auto& vertices = component_vertices_[unique_vertex_id];
            const auto is_component = [&component_id](
                                          const ComponentMeshVertex& cmv ) {
                return cmv.component_id.id == component_id;
            };
            if( absl::c_none_of( vertices, is_component ) )
            {
                return 0;
            }
            const auto old_size = vertices.size();
            vertices.erase( std::remove_if( vertices.begin(), vertices.end(),
                                is_component ),
                vertices.end() );
            return static_cast< index_t >( old_size - vertices.size() );
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
                for( const auto& cmv : component_vertices_[uv] )
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
            if( !unique_vertices_ids.has_value() )
            {
                Logger::warning(
                    "[VertexIdentifier::load_component] Unique vertices "
                    "attribute not found, it will be restored from the "
                    "unique vertices" );
                AttributeProperties attribute_properties;
                attribute_properties.assignable = false;
                attribute_properties.interpolable = false;
                attribute_properties.transferable = false;
                AttributeValues< index_t > unique_vertex_attribute_values;
                unique_vertex_attribute_values.default_value = NO_ID;
                unique_vertex_attribute_values.no_value = NO_ID;
                vertex_attribute_manager
                    .template create_attribute< VariableAttribute, index_t >(
                        UNIQUE_VERTICES_NAME, unique_vertex_id_,
                        unique_vertex_attribute_values, attribute_properties );
                return;
            }
            if( unique_vertices_ids.value().size() != 1 )
            {
                Logger::warning(
                    "[VertexIdentifier::load_component] Unique vertices "
                    "attribute is not unique, using the first one" );
            }
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
            for( const auto& old_id : unique_vertices_ids.value() )
            {
                vertex_attribute_manager.delete_attribute( old_id );
            }
        }

    private:
        struct ComponentVerticesRepair
        {
            void log() const
            {
                if( nb_unknown_components > 0 )
                {
                    Logger::warning( "[VertexIdentifier::load] Removed ",
                        nb_unknown_components,
                        " component vertices of unknown components" );
                }
                if( nb_invalid_vertices > 0 )
                {
                    Logger::warning( "[VertexIdentifier::load] Removed ",
                        nb_invalid_vertices,
                        " component vertices with invalid vertex index" );
                }
                if( nb_inconsistent_vertices > 0 )
                {
                    Logger::warning( "[VertexIdentifier::load] Removed ",
                        nb_inconsistent_vertices,
                        " component vertices linked to another unique "
                        "vertex" );
                }
                if( nb_restored_vertices > 0 )
                {
                    Logger::warning( "[VertexIdentifier::load] Restored ",
                        nb_restored_vertices,
                        " component vertex links to their unique vertex" );
                }
            }

            index_t nb_unknown_components{ 0 };
            index_t nb_invalid_vertices{ 0 };
            index_t nb_inconsistent_vertices{ 0 };
            index_t nb_restored_vertices{ 0 };
        };

        [[nodiscard]] bool is_linked_to_unique_vertex(
            const ComponentMeshVertex& vertex, index_t unique_vertex ) const
        {
            const auto it =
                vertex2unique_vertex_.find( vertex.component_id.id );
            if( it == vertex2unique_vertex_.end() )
            {
                return false;
            }
            const auto& attribute = *it->second.attribute;
            return vertex.vertex < attribute.size()
                   && attribute.value( vertex.vertex ) == unique_vertex;
        }

        void repair_unique_vertex(
            index_t unique_vertex, ComponentVerticesRepair& repair )
        {
            auto& vertices = component_vertices_[unique_vertex];
            std::vector< bool > to_keep( vertices.size(), true );
            bool update{ false };
            for( const auto v : Indices{ vertices } )
            {
                const auto& vertex = vertices[v];
                const auto it =
                    vertex2unique_vertex_.find( vertex.component_id.id );
                if( it == vertex2unique_vertex_.end() )
                {
                    to_keep[v] = false;
                    update = true;
                    repair.nb_unknown_components++;
                    continue;
                }
                auto& attribute = *it->second.attribute;
                if( vertex.vertex >= attribute.size() )
                {
                    to_keep[v] = false;
                    update = true;
                    repair.nb_invalid_vertices++;
                    continue;
                }
                const auto value = attribute.value( vertex.vertex );
                if( value == NO_ID )
                {
                    attribute.set_value( vertex.vertex, unique_vertex );
                    repair.nb_restored_vertices++;
                }
                else if( value != unique_vertex )
                {
                    to_keep[v] = false;
                    update = true;
                    repair.nb_inconsistent_vertices++;
                }
            }
            if( update )
            {
                vertices = extract_vector_elements( to_keep, vertices );
            }
        }

        /*!
         * Component mesh vertices of each unique vertex, its size is the
         * number of unique vertices
         */
        std::vector< std::vector< ComponentMeshVertex > > component_vertices_;
        absl::flat_hash_map< uuid, RegisteredComponent > vertex2unique_vertex_;
        geode::uuid unique_vertex_id_;
        index_t nb_registrations_{ 0 };
        bool nb_component_mesh_vertices_up_to_date_{ true };
        /*!
         * True after loading a file not storing the component vertices:
         * repair_component_vertices rebuilds them
         */
        bool rebuild_component_vertices_{ false };
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

    void VertexIdentifier::repair_component_vertices( BuilderKey /*key*/ )
    {
        impl_->repair_component_vertices();
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
