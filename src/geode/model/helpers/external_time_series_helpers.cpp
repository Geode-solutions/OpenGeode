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

#include <geode/model/helpers/external_time_series_helpers.hpp>

#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    void delete_replaced_steps( const geode::AttributeManager& from,
        geode::AttributeManager& to,
        std::string_view name )
    {
        for( const auto& step : from.time_steps( name ) )
        {
            const auto attribute =
                from.find_generic_attribute( step.attribute_id );
            if( !attribute->properties().transferable )
            {
                continue;
            }
            for( const auto& existing_step : to.time_steps( name ) )
            {
                if( existing_step.time == step.time )
                {
                    to.delete_attribute( existing_step.attribute_id );
                }
            }
        }
    }

    void delete_replaced_steps(
        const geode::AttributeManager& from, geode::AttributeManager& to )
    {
        for( const auto& name : from.time_series_names() )
        {
            delete_replaced_steps( from, to, name );
        }
    }
} // namespace

namespace geode
{
    class ExternalTimeSeriesHelpers::Impl
    {
        friend class bitsery::Access;

    public:
        Impl() = default;

        explicit Impl( const BRep& model )
        {
            unique_vertex_attributes_.resize( model.nb_unique_vertices() );
            for( const auto& block : model.blocks() )
            {
                block_polyhedron_attributes_[block.id()].resize(
                    block.mesh().nb_polyhedra() );
            }
        }

        [[nodiscard]] AttributeManager& unique_vertex_attribute_manager()
        {
            return unique_vertex_attributes_;
        }

        [[nodiscard]] const AttributeManager&
            unique_vertex_attribute_manager() const
        {
            return unique_vertex_attributes_;
        }

        [[nodiscard]] AttributeManager& block_polyhedron_attribute_manager(
            const uuid& block_id )
        {
            const auto it = block_polyhedron_attributes_.find( block_id );
            check_block( it != block_polyhedron_attributes_.end(), block_id );
            return it->second;
        }

        [[nodiscard]] const AttributeManager&
            block_polyhedron_attribute_manager( const uuid& block_id ) const
        {
            const auto it = block_polyhedron_attributes_.find( block_id );
            check_block( it != block_polyhedron_attributes_.end(), block_id );
            return it->second;
        }

        void apply_to( const BRep& model ) const
        {
            check_model( model );
            for( const auto& block : model.blocks() )
            {
                apply_vertices( model, block );
            }
            for( const auto& [block_id, polyhedron_attributes] :
                block_polyhedron_attributes_ )
            {
                apply_polyhedra(
                    model.block( block_id ), polyhedron_attributes );
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& local_archive, Impl& impl ) {
                        local_archive.object( impl.unique_vertex_attributes_ );
                        local_archive.ext( impl.block_polyhedron_attributes_,
                            bitsery::ext::StdMap{
                                impl.block_polyhedron_attributes_.max_size() },
                            []( Archive& map_archive, uuid& block_id,
                                AttributeManager& polyhedron_attributes ) {
                                map_archive.object( block_id );
                                map_archive.object( polyhedron_attributes );
                            } );
                    } } } );
        }

        static void check_block( bool is_known, const uuid& block_id )
        {
            OpenGeodeModelException::check_exception( is_known, nullptr,
                OpenGeodeException::TYPE::data,
                "[ExternalTimeSeriesHelpers] Unknown Block ",
                block_id.string() );
        }

        void check_model( const BRep& model ) const
        {
            OpenGeodeModelException::check_exception(
                unique_vertex_attributes_.nb_elements()
                    == model.nb_unique_vertices(),
                nullptr, OpenGeodeException::TYPE::data,
                "[ExternalTimeSeriesHelpers::apply_to] Model has ",
                model.nb_unique_vertices(), " unique vertices instead of ",
                unique_vertex_attributes_.nb_elements() );
            for( const auto& [block_id, polyhedron_attributes] :
                block_polyhedron_attributes_ )
            {
                check_block_polyhedra( model, block_id, polyhedron_attributes );
            }
        }

        static void check_block_polyhedra( const BRep& model,
            const uuid& block_id,
            const AttributeManager& polyhedron_attributes )
        {
            OpenGeodeModelException::check_exception(
                model.has_block( block_id ), nullptr,
                OpenGeodeException::TYPE::data,
                "[ExternalTimeSeriesHelpers::apply_to] Model has no Block ",
                block_id.string() );
            const auto nb_polyhedra =
                model.block( block_id ).mesh().nb_polyhedra();
            OpenGeodeModelException::check_exception(
                nb_polyhedra == polyhedron_attributes.nb_elements(), nullptr,
                OpenGeodeException::TYPE::data,
                "[ExternalTimeSeriesHelpers::apply_to] Block ",
                block_id.string(), " has ", nb_polyhedra,
                " polyhedra instead of ", polyhedron_attributes.nb_elements() );
        }

        void apply_vertices( const BRep& model, const Block3D& block ) const
        {
            const auto& mesh = block.mesh();
            GenericMapping< index_t > unique2block;
            unique2block.reserve( mesh.nb_vertices() );
            ComponentMeshVertex block_vertex{ block.component_id(), 0 };
            for( const auto vertex : Range{ mesh.nb_vertices() } )
            {
                block_vertex.vertex = vertex;
                const auto unique_vertex = model.unique_vertex( block_vertex );
                if( unique_vertex != NO_ID )
                {
                    unique2block.map( unique_vertex, vertex );
                }
            }
            auto& vertex_attributes = mesh.vertex_attribute_manager();
            delete_replaced_steps(
                unique_vertex_attributes_, vertex_attributes );
            vertex_attributes.import( unique_vertex_attributes_, unique2block );
        }

        static void apply_polyhedra( const Block3D& block,
            const AttributeManager& polyhedron_attributes )
        {
            auto& block_polyhedron_attributes =
                block.mesh().polyhedron_attribute_manager();
            delete_replaced_steps(
                polyhedron_attributes, block_polyhedron_attributes );
            block_polyhedron_attributes.copy( polyhedron_attributes );
        }

    private:
        AttributeManager unique_vertex_attributes_;
        absl::flat_hash_map< uuid, AttributeManager >
            block_polyhedron_attributes_;
    };

    ExternalTimeSeriesHelpers::ExternalTimeSeriesHelpers() = default;

    ExternalTimeSeriesHelpers::ExternalTimeSeriesHelpers( const BRep& model )
        : impl_{ model }
    {
    }

    ExternalTimeSeriesHelpers::ExternalTimeSeriesHelpers(
        ExternalTimeSeriesHelpers&& ) noexcept = default;

    ExternalTimeSeriesHelpers& ExternalTimeSeriesHelpers::operator=(
        ExternalTimeSeriesHelpers&& ) noexcept = default;

    ExternalTimeSeriesHelpers::~ExternalTimeSeriesHelpers() = default;

    AttributeManager&
        ExternalTimeSeriesHelpers::unique_vertex_attribute_manager()
    {
        return impl_->unique_vertex_attribute_manager();
    }

    const AttributeManager&
        ExternalTimeSeriesHelpers::unique_vertex_attribute_manager() const
    {
        return impl_->unique_vertex_attribute_manager();
    }

    AttributeManager&
        ExternalTimeSeriesHelpers::block_polyhedron_attribute_manager(
            const uuid& block_id )
    {
        return impl_->block_polyhedron_attribute_manager( block_id );
    }

    const AttributeManager&
        ExternalTimeSeriesHelpers::block_polyhedron_attribute_manager(
            const uuid& block_id ) const
    {
        return impl_->block_polyhedron_attribute_manager( block_id );
    }

    void ExternalTimeSeriesHelpers::apply_to( BRep& model ) const
    {
        impl_->apply_to( model );
    }

    template < typename Archive >
    void ExternalTimeSeriesHelpers::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, ExternalTimeSeriesHelpers >{
                                { []( Archive& local_archive,
                                      ExternalTimeSeriesHelpers& helpers ) {
                                    local_archive.object( helpers.impl_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, ExternalTimeSeriesHelpers );
} // namespace geode
