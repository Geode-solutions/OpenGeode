#include <geode/model/mixin/core/vertex_identifier.h>

#include <fstream>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/builder/geode_vertex_set_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/geode_vertex_set.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/bitsery_archive.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

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

    ComponentMeshVertex::~ComponentMeshVertex() {} // NOLINT

    bool ComponentMeshVertex::operator==(
        const ComponentMeshVertex& other ) const
    {
        return component_id == other.component_id && vertex == other.vertex;
    }

    template < typename Archive >
    void ComponentMeshVertex::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, ComponentMeshVertex >{},
            []( Archive& a, ComponentMeshVertex& component_mesh_vertex ) {
                a.object( component_mesh_vertex.component_id );
                a.value4b( component_mesh_vertex.vertex );
            } );
    }

    class VertexIdentifier::Impl
    {
        const std::string unique_vertices_name = "unique vertices";

    public:
        Impl()
            : component_vertices_(
                unique_vertices_.vertex_attribute_manager()
                    .find_or_create_attribute< VariableAttribute,
                        std::vector< ComponentMeshVertex > >(
                        "component vertices",
                        std::vector< ComponentMeshVertex >{} ) )
        {
        }

        index_t nb_unique_vertices() const
        {
            return unique_vertices_.nb_vertices();
        }

        const std::vector< ComponentMeshVertex >& component_mesh_vertices(
            index_t unique_vertex_id ) const
        {
            return component_vertices_->value( unique_vertex_id );
        }

        std::vector< ComponentMeshVertex > component_mesh_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const
        {
            const auto& component_vertices =
                component_mesh_vertices( unique_vertex_id );
            std::vector< ComponentMeshVertex > result;
            result.reserve( component_vertices.size() );
            for( const auto& component_vertex : component_vertices )
            {
                if( component_vertex.component_id.type() == type )
                {
                    result.push_back( component_vertex );
                }
            }
            return result;
        }

        std::vector< index_t > component_mesh_vertices(
            index_t unique_vertex_id, const uuid& component_id ) const
        {
            const auto& component_vertices =
                component_mesh_vertices( unique_vertex_id );
            std::vector< index_t > result;
            result.reserve( component_vertices.size() );
            for( const auto& component_vertex : component_vertices )
            {
                if( component_vertex.component_id.id() == component_id )
                {
                    result.push_back( component_vertex.vertex );
                }
            }
            return result;
        }

        index_t unique_vertex(
            const uuid& component_id, const index_t vertex_id ) const
        {
            return vertex2unique_vertex_.at( component_id )->value( vertex_id );
        }

        bool has_component_mesh_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const
        {
            const auto& component_vertices =
                component_mesh_vertices( unique_vertex_id );
            for( const auto& component_vertex : component_vertices )
            {
                if( component_vertex.component_id.type() == type )
                {
                    return true;
                }
            }
            return false;
        }

        bool has_component_mesh_vertices(
            index_t unique_vertex_id, const uuid& component_id ) const
        {
            const auto& component_vertices =
                component_mesh_vertices( unique_vertex_id );
            for( const auto& component_vertex : component_vertices )
            {
                if( component_vertex.component_id.id() == component_id )
                {
                    return true;
                }
            }
            return false;
        }

        template < typename MeshComponent >
        void register_component( const MeshComponent& component )
        {
            auto it = vertex2unique_vertex_.find( component.id() );
            const auto& mesh = component.mesh();
            if( it == vertex2unique_vertex_.end() )
            {
                mesh.vertex_attribute_manager().delete_attribute(
                    unique_vertices_name );
                vertex2unique_vertex_.emplace( component.id(),
                    mesh.vertex_attribute_manager()
                        .template find_or_create_attribute< VariableAttribute,
                            index_t >( unique_vertices_name, NO_ID ) );
            }
            else
            {
                auto attribute =
                    mesh.vertex_attribute_manager()
                        .template find_or_create_attribute< VariableAttribute,
                            index_t >( unique_vertices_name, NO_ID );
                try
                {
                    for( const auto v : Range{ mesh.nb_vertices() } )
                    {
                        attribute->set_value( v, it->second->value( v ) );
                    }
                }
                catch( const std::out_of_range& )
                {
                    Logger::warn(
                        "Registering MeshComponent: ", component.id().string(),
                        " in VertexIdentifier, wrong number of vertices." );
                }
                it->second = std::move( attribute );
            }
        }

        template < typename MeshComponent >
        void unregister_component( const MeshComponent& component )
        {
            const auto& mesh = component.mesh();
            mesh.vertex_attribute_manager().delete_attribute(
                "unique vertices" );
            vertex2unique_vertex_.erase( component.id() );
            filter_component_vertices( component.id() );
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
            const auto& old_unique_id =
                vertex2unique_vertex_
                    .at( component_vertex_id.component_id.id() )
                    ->value( component_vertex_id.vertex );
            if( old_unique_id == unique_vertex_id )
            {
                return;
            }

            if( old_unique_id != NO_ID )
            {
                unset_unique_vertex( component_vertex_id, old_unique_id );
            }
            vertex2unique_vertex_.at( component_vertex_id.component_id.id() )
                ->set_value( component_vertex_id.vertex, unique_vertex_id );
            const auto& vertices =
                component_vertices_->value( unique_vertex_id );
            if( absl::c_find( vertices, component_vertex_id )
                == vertices.end() )
            {
                component_vertices_->modify_value( unique_vertex_id,
                    [&component_vertex_id](
                        std::vector< ComponentMeshVertex >& value ) {
                        value.emplace_back( std::move( component_vertex_id ) );
                    } );
                ;
            }
        }

        void unset_unique_vertex(
            const ComponentMeshVertex& component_vertex_id,
            const index_t unique_vertex_id )
        {
            const auto& vertices =
                component_vertices_->value( unique_vertex_id );
            const auto it = absl::c_find( vertices, component_vertex_id );
            OPENGEODE_EXCEPTION( it != vertices.end(),
                "[VertexIdentifier::unset_unique_vertex] Unique vertex to "
                "unset is not correct" );
            component_vertices_->modify_value( unique_vertex_id,
                [&it]( std::vector< ComponentMeshVertex >& value ) {
                    value.erase(
                        // workaround for gcc < 4.9
                        value.begin() + ( it - value.cbegin() ) );
                } );
            vertex2unique_vertex_.at( component_vertex_id.component_id.id() )
                ->set_value( component_vertex_id.vertex, NO_ID );
        }

        void update_unique_vertices( const ComponentID& component_id,
            absl::Span< const index_t > old2new )
        {
            const auto unique_vertices =
                component_unique_vertices( component_id.id() );
            for( const auto uv : unique_vertices )
            {
                const auto& old_vertices =
                    component_mesh_vertices( uv, component_id.id() );
                for( const auto old_id : old_vertices )
                {
                    const auto& all_vertices = component_vertices_->value( uv );
                    const auto it = absl::c_find( all_vertices,
                        ComponentMeshVertex{ component_id, old_id } );
                    OPENGEODE_EXCEPTION( it != all_vertices.end(),
                        "[VertexIdentifier::update_unique_vertices] Old mesh "
                        "component vertex should be found in unique "
                        "vertex" );
                    const auto new_id = old2new[old_id];
                    if( new_id == NO_ID )
                    {
                        component_vertices_->modify_value( uv,
                            [&it](
                                std::vector< ComponentMeshVertex >& vertices ) {
                                vertices.erase(
                                    // workaround for gcc < 4.9
                                    vertices.begin()
                                    + ( it - vertices.cbegin() ) );
                            } );
                    }
                    else
                    {
                        component_vertices_->modify_value( uv,
                            [&it, &new_id](
                                std::vector< ComponentMeshVertex >& vertices ) {
                                const auto pos =
                                    std::distance( vertices.cbegin(), it );
                                vertices[pos].vertex = new_id;
                            } );
                    }
                }
            }
        }

        std::vector< index_t > delete_isolated_vertices()
        {
            std::vector< bool > to_delete(
                unique_vertices_.nb_vertices(), false );
            absl::flat_hash_map< uuid, std::vector< index_t > >
                components_vertices;
            for( const auto v : Range{ unique_vertices_.nb_vertices() } )
            {
                const auto& vertices = component_mesh_vertices( v );
                if( vertices.empty() )
                {
                    to_delete[v] = true;
                    continue;
                }
                for( const auto& cmv : vertices )
                {
                    components_vertices[cmv.component_id.id()].emplace_back(
                        cmv.vertex );
                }
            }
            const auto old2new = VertexSetBuilder::create( unique_vertices_ )
                                     ->delete_vertices( to_delete );
            for( const auto& component_vertices : components_vertices )
            {
                auto& attribute =
                    vertex2unique_vertex_.at( component_vertices.first );
                for( const auto v : component_vertices.second )
                {
                    attribute->set_value( v, old2new[attribute->value( v )] );
                }
            }
            return old2new;
        }

        void save( absl::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            register_basic_serialize_pcontext( std::get< 0 >( context ) );
            register_geometry_serialize_pcontext( std::get< 0 >( context ) );
            register_mesh_serialize_pcontext( std::get< 0 >( context ) );
            register_model_serialize_pcontext( std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[VertexIdentifier::save] Error while writing file: ",
                filename );
        }

        void load( absl::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/vertices" );
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            register_basic_deserialize_pcontext( std::get< 0 >( context ) );
            register_geometry_deserialize_pcontext( std::get< 0 >( context ) );
            register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
            register_model_deserialize_pcontext( std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[VertexIdentifier::load] Error while reading file: ",
                filename );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.object( impl.unique_vertices_ );
                    a.ext(
                        impl.component_vertices_, bitsery::ext::StdSmartPtr{} );
                    a.ext( impl.vertex2unique_vertex_,
                        bitsery::ext::StdMap{
                            impl.vertex2unique_vertex_.max_size() },
                        []( Archive& a2, uuid& id,
                            std::shared_ptr< VariableAttribute< index_t > >&
                                attribute ) {
                            a2.object( id );
                            a2.ext( attribute, bitsery::ext::StdSmartPtr{} );
                        } );
                } );
        }

        void filter_component_vertices( const uuid& component_id )
        {
            for( const auto uv_id : Range{ nb_unique_vertices() } )
            {
                const auto& component_mesh_vertices =
                    component_vertices_->value( uv_id );
                std::vector< bool > to_keep(
                    component_mesh_vertices.size(), true );
                bool update{ false };
                for( const auto i : Indices{ component_mesh_vertices } )
                {
                    if( component_mesh_vertices[i].component_id.id()
                        == component_id )
                    {
                        to_keep[i] = false;
                        update = true;
                    }
                }
                if( update )
                {
                    component_vertices_->set_value(
                        uv_id, extract_vector_elements(
                                   to_keep, component_mesh_vertices ) );
                }
            }
        }

        std::vector< index_t > component_unique_vertices(
            const uuid& component_id )
        {
            std::vector< index_t > result;
            result.reserve( nb_unique_vertices() );
            for( const auto uv_id : Range{ nb_unique_vertices() } )
            {
                if( !component_mesh_vertices( uv_id, component_id ).empty() )
                {
                    result.push_back( uv_id );
                }
            }
            return result;
        }

    private:
        OpenGeodeVertexSet unique_vertices_;
        std::shared_ptr<
            VariableAttribute< std::vector< ComponentMeshVertex > > >
            component_vertices_;
        absl::flat_hash_map< uuid,
            std::shared_ptr< VariableAttribute< index_t > > >
            vertex2unique_vertex_;
    };

    VertexIdentifier::VertexIdentifier() {} // NOLINT
    VertexIdentifier::VertexIdentifier( VertexIdentifier&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }
    VertexIdentifier::~VertexIdentifier() {} // NOLINT

    index_t VertexIdentifier::nb_unique_vertices() const
    {
        return impl_->nb_unique_vertices();
    }

    const std::vector< ComponentMeshVertex >&
        VertexIdentifier::component_mesh_vertices(
            index_t unique_vertex_id ) const
    {
        return impl_->component_mesh_vertices( unique_vertex_id );
    }

    std::vector< ComponentMeshVertex >
        VertexIdentifier::component_mesh_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const
    {
        return impl_->component_mesh_vertices( unique_vertex_id, type );
    }

    std::vector< index_t > VertexIdentifier::component_mesh_vertices(
        index_t unique_vertex_id, const uuid& component_id ) const
    {
        return impl_->component_mesh_vertices( unique_vertex_id, component_id );
    }

    index_t VertexIdentifier::unique_vertex(
        const ComponentMeshVertex& component_mesh_vertex ) const
    {
        return impl_->unique_vertex( component_mesh_vertex.component_id.id(),
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

    template < typename MeshComponent >
    void VertexIdentifier::register_mesh_component(
        const MeshComponent& component, BuilderKey )
    {
        impl_->register_component( component );
    }

    template < typename MeshComponent >
    void VertexIdentifier::unregister_mesh_component(
        const MeshComponent& component, BuilderKey )
    {
        impl_->unregister_component( component );
    }

    index_t VertexIdentifier::create_unique_vertex( BuilderKey )
    {
        return impl_->create_unique_vertex();
    }

    index_t VertexIdentifier::create_unique_vertices( index_t nb, BuilderKey )
    {
        return impl_->create_unique_vertices( nb );
    }

    void VertexIdentifier::set_unique_vertex(
        ComponentMeshVertex component_vertex_id,
        index_t unique_vertex_id,
        BuilderKey )
    {
        impl_->set_unique_vertex(
            std::move( component_vertex_id ), unique_vertex_id );
    }

    void VertexIdentifier::unset_unique_vertex(
        const ComponentMeshVertex& component_vertex_id,
        index_t unique_vertex_id,
        BuilderKey )
    {
        impl_->unset_unique_vertex( component_vertex_id, unique_vertex_id );
    }

    void VertexIdentifier::update_unique_vertices(
        const ComponentID& component_id,
        absl::Span< const index_t > old2new,
        BuilderKey )
    {
        impl_->update_unique_vertices( component_id, old2new );
    }

    void VertexIdentifier::save_unique_vertices(
        absl::string_view directory ) const
    {
        impl_->save( directory );
    }

    void VertexIdentifier::load_unique_vertices(
        absl::string_view directory, BuilderKey )
    {
        return impl_->load( directory );
    }

    std::vector< index_t > VertexIdentifier::delete_isolated_vertices(
        BuilderKey )
    {
        return impl_->delete_isolated_vertices();
    }

    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner2D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner3D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line2D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line3D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface2D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface3D&, BuilderKey );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Block3D&, BuilderKey );

    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Corner2D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Corner3D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Line2D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Line3D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Surface2D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Surface3D&, BuilderKey );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component(
            const Block3D&, BuilderKey );

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, ComponentMeshVertex );
} // namespace geode
