#include <geode/model/mixin/core/vertex_identifier.h>

#include <fstream>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/builder/geode_vertex_set_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/geode_vertex_set.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/model/mixin/core/bitsery_archive.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    class VertexIdentifier::Impl
    {
    public:
        Impl()
            : component_vertices_(
                  unique_vertices_.vertex_attribute_manager()
                      .find_or_create_attribute< VariableAttribute,
                          std::vector< MeshComponentVertex > >(
                          "component vertices",
                          std::vector< MeshComponentVertex >{} ) )
        {
        }

        index_t nb_unique_vertices() const
        {
            return unique_vertices_.nb_vertices();
        }

        const std::vector< MeshComponentVertex >& mesh_component_vertices(
            index_t unique_vertex_id ) const
        {
            return component_vertices_->value( unique_vertex_id );
        }

        index_t unique_vertex(
            const uuid& component_id, const index_t vertex_id ) const
        {
            return vertex2unique_vertex_.at( component_id )->value( vertex_id );
        }

        template < typename MeshComponent >
        void register_component( const MeshComponent& component )
        {
            auto it = vertex2unique_vertex_.find( component.id() );
            const auto& mesh = component.mesh();
            if( it == vertex2unique_vertex_.end() )
            {
                OPENGEODE_EXCEPTION(
                    !mesh.vertex_attribute_manager().attribute_exists(
                        "unique_vertices" ),
                    "[VertexIdentifier::register_component] At component "
                    "registration, no attribute called "
                    "\"unique_vertices\" should exists on component mesh. " );
                vertex2unique_vertex_.emplace( component.id(),
                    mesh.vertex_attribute_manager()
                        .template find_or_create_attribute< VariableAttribute,
                            index_t >( "unique vertices", NO_ID ) );
            }
            else
            {
                auto attribute =
                    mesh.vertex_attribute_manager()
                        .template find_or_create_attribute< VariableAttribute,
                            index_t >( "unique vertices", NO_ID );
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
            return OpenGeodeVertexSetBuilder{ unique_vertices_ }
                .create_vertex();
        }

        index_t create_unique_vertices( const index_t nb )
        {
            return OpenGeodeVertexSetBuilder{ unique_vertices_ }
                .create_vertices( nb );
        }

        void set_unique_vertex( MeshComponentVertex component_vertex_id,
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
                const auto& old_vertices =
                    component_vertices_->value( old_unique_id );
                const auto it =
                    absl::c_find( old_vertices, component_vertex_id );
                if( it != old_vertices.end() )
                {
                    component_vertices_->modify_value( old_unique_id,
                        [&it]( std::vector< MeshComponentVertex >& vertices ) {
                            vertices.erase(
                                // workaround for gcc < 4.9
                                vertices.begin() + ( it - vertices.cbegin() ) );
                        } );
                }
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
                        std::vector< MeshComponentVertex >& vertices ) {
                        vertices.emplace_back(
                            std::move( component_vertex_id ) );
                    } );
                ;
            }
        }

        void save( absl::string_view directory ) const
        {
            const auto filename =
                static_cast< std::string >( directory ) + "/vertices";
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
                "[VertexIdentifier::save] Error while writing file: ",
                filename );
        }

        void load( absl::string_view directory )
        {
            const auto filename =
                static_cast< std::string >( directory ) + "/vertices";
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
                []( Archive& archive, Impl& impl ) {
                    archive.object( impl.unique_vertices_ );
                    archive.ext(
                        impl.component_vertices_, bitsery::ext::StdSmartPtr{} );
                    archive.ext( impl.vertex2unique_vertex_,
                        bitsery::ext::StdMap{
                            impl.vertex2unique_vertex_.max_size() },
                        []( Archive& archive, uuid& id,
                            std::shared_ptr< VariableAttribute< index_t > >&
                                attribute ) {
                            archive.object( id );
                            archive.ext(
                                attribute, bitsery::ext::StdSmartPtr{} );
                        } );
                } );
        }

        void filter_component_vertices( const uuid& component_id )
        {
            for( const auto uv_id : Range{ nb_unique_vertices() } )
            {
                const auto& mesh_component_vertices =
                    component_vertices_->value( uv_id );
                std::vector< bool > to_keep(
                    mesh_component_vertices.size(), true );
                bool update{ false };
                for( const auto i : Range{ mesh_component_vertices.size() } )
                {
                    if( mesh_component_vertices[i].component_id.id()
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
                                   to_keep, mesh_component_vertices ) );
                }
            }
        }

    private:
        OpenGeodeVertexSet unique_vertices_;
        std::shared_ptr<
            VariableAttribute< std::vector< MeshComponentVertex > > >
            component_vertices_;
        absl::flat_hash_map< uuid,
            std::shared_ptr< VariableAttribute< index_t > > >
            vertex2unique_vertex_;
    };

    VertexIdentifier::VertexIdentifier() {} // NOLINT

    VertexIdentifier::~VertexIdentifier() {} // NOLINT

    index_t VertexIdentifier::nb_unique_vertices() const
    {
        return impl_->nb_unique_vertices();
    }

    const std::vector< MeshComponentVertex >&
        VertexIdentifier::mesh_component_vertices(
            index_t unique_vertex_id ) const
    {
        return impl_->mesh_component_vertices( unique_vertex_id );
    }

    std::vector< MeshComponentVertex >
        VertexIdentifier::mesh_component_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const
    {
        const auto& component_vertices =
            mesh_component_vertices( unique_vertex_id );
        std::vector< MeshComponentVertex > result;
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

    std::vector< index_t > VertexIdentifier::mesh_component_vertices(
        index_t unique_vertex_id, const uuid& component_id ) const
    {
        const auto& component_vertices =
            mesh_component_vertices( unique_vertex_id );
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

    index_t VertexIdentifier::unique_vertex(
        const MeshComponentVertex& mesh_component_vertex ) const
    {
        return impl_->unique_vertex( mesh_component_vertex.component_id.id(),
            mesh_component_vertex.vertex );
    }

    template < typename MeshComponent >
    void VertexIdentifier::register_mesh_component(
        const MeshComponent& component )
    {
        impl_->register_component( component );
    }

    template < typename MeshComponent >
    void VertexIdentifier::unregister_mesh_component(
        const MeshComponent& component )
    {
        impl_->unregister_component( component );
    }

    index_t VertexIdentifier::create_unique_vertex()
    {
        return impl_->create_unique_vertex();
    }

    index_t VertexIdentifier::create_unique_vertices( index_t nb )
    {
        return impl_->create_unique_vertices( nb );
    }

    void VertexIdentifier::set_unique_vertex(
        MeshComponentVertex component_vertex_id, index_t unique_vertex_id )
    {
        impl_->set_unique_vertex(
            std::move( component_vertex_id ), unique_vertex_id );
    }

    void VertexIdentifier::save_unique_vertices(
        absl::string_view directory ) const
    {
        impl_->save( directory );
    }

    void VertexIdentifier::load_unique_vertices( absl::string_view directory )
    {
        return impl_->load( directory );
    }

    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner2D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Corner3D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line2D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Line3D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface2D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Surface3D& );
    template void opengeode_model_api VertexIdentifier::register_mesh_component(
        const Block3D& );

    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Corner2D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Corner3D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Line2D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Line3D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Surface2D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Surface3D& );
    template void opengeode_model_api
        VertexIdentifier::unregister_mesh_component( const Block3D& );
} // namespace geode
