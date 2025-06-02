/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/mixin/core/blocks.hpp>

#include <async++.h>

#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/hybrid_solid_output.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/polyhedral_solid_output.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_output.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/detail/components_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class Blocks< dimension >::Impl
        : public detail::ComponentsStorage< Block< dimension > >
    {
    };

    template < index_t dimension >
    Blocks< dimension >::Blocks() = default;

    template < index_t dimension >
    Blocks< dimension >::Blocks( Blocks&& ) noexcept = default;

    template < index_t dimension >
    Blocks< dimension >::~Blocks() = default;

    template < index_t dimension >
    auto Blocks< dimension >::operator=( Blocks&& ) noexcept
        -> Blocks& = default;

    template < index_t dimension >
    index_t Blocks< dimension >::nb_blocks() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    index_t Blocks< dimension >::nb_active_blocks() const
    {
        return detail::count_range_elements( active_blocks() );
    }

    template < index_t dimension >
    bool Blocks< dimension >::has_block( const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    const Block< dimension >& Blocks< dimension >::block( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Block< dimension >& Blocks< dimension >::modifiable_block(
        const uuid& id, BlocksBuilderKey /*unused*/ )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Blocks< dimension >::save_blocks( std::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/blocks" ) );
        const auto prefix = absl::StrCat(
            directory, "/", Block< dimension >::component_type_static().get() );
        absl::FixedArray< async::task< void > > tasks( nb_blocks() );
        index_t count{ 0 };
        for( const auto& block : blocks() )
        {
            tasks[count++] = async::spawn( [&block, &prefix] {
                const auto& mesh = block.mesh();
                const auto file = absl::StrCat(
                    prefix, block.id().string(), ".", mesh.native_extension() );

                if( const auto* tetra =
                        dynamic_cast< const TetrahedralSolid< dimension >* >(
                            &mesh ) )
                {
                    save_tetrahedral_solid( *tetra, file );
                }
                else if( const auto* hybrid =
                             dynamic_cast< const HybridSolid< dimension >* >(
                                 &mesh ) )
                {
                    save_hybrid_solid( *hybrid, file );
                }
                else if( const auto* poly = dynamic_cast<
                             const PolyhedralSolid< dimension >* >( &mesh ) )
                {
                    save_polyhedral_solid( *poly, file );
                }
                else
                {
                    throw OpenGeodeException(
                        "[Blocks::save_blocks] Cannot find the explicit "
                        "SolidMesh type" );
                }
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    void Blocks< dimension >::load_blocks(
        std::string_view directory, BlocksBuilderKey /*unused*/ )
    {
        impl_->load_components( absl::StrCat( directory, "/blocks" ) );
        const auto mapping = impl_->file_mapping( directory );
        absl::FixedArray< async::task< void > > tasks( nb_blocks() );
        index_t count{ 0 };
        for( auto& block : modifiable_blocks( {} ) )
        {
            tasks[count++] = async::spawn( [&block, &mapping] {
                const auto file = mapping.at( block.id().string() );
                if( MeshFactory::type( block.mesh_type() )
                    == TetrahedralSolid< dimension >::type_name_static() )
                {
                    block.set_mesh( load_tetrahedral_solid< dimension >(
                                        block.mesh_type(), file ),
                        typename Block< dimension >::BlocksKey{} );
                }
                else if( MeshFactory::type( block.mesh_type() )
                         == HybridSolid< dimension >::type_name_static() )
                {
                    block.set_mesh( load_hybrid_solid< dimension >(
                                        block.mesh_type(), file ),
                        typename Block< dimension >::BlocksKey{} );
                }
                else
                {
                    block.set_mesh( load_polyhedral_solid< dimension >(
                                        block.mesh_type(), file ),
                        typename Block< dimension >::BlocksKey{} );
                }
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    const uuid& Blocks< dimension >::create_block( BlocksBuilderKey /*unused*/ )
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ typename Block< dimension >::BlocksKey() }
        };
        const auto& id = block->id();
        impl_->add_component( std::move( block ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Blocks< dimension >::create_block(
        const MeshImpl& impl, BlocksBuilderKey /*unused*/ )
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ impl, {} }
        };
        const auto& id = block->id();
        impl_->add_component( std::move( block ) );
        return id;
    }

    template < index_t dimension >
    void Blocks< dimension >::create_block(
        uuid block_id, BlocksBuilderKey /*unused*/ )
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ typename Block< dimension >::BlocksKey{} }
        };
        IdentifierBuilder{ *block }.set_id( std::move( block_id ) );
        impl_->add_component( std::move( block ) );
    }

    template < index_t dimension >
    void Blocks< dimension >::create_block(
        uuid block_id, const MeshImpl& impl, BlocksBuilderKey /*unused*/ )
    {
        typename Blocks< dimension >::Impl::ComponentPtr block{
            new Block< dimension >{ impl, {} }
        };
        IdentifierBuilder{ *block }.set_id( std::move( block_id ) );
        impl_->add_component( std::move( block ) );
    }

    template < index_t dimension >
    void Blocks< dimension >::delete_block(
        const Block< dimension >& block, BlocksBuilderKey /*unused*/ )
    {
        impl_->delete_component( block.id() );
    }

    template < index_t dimension >
    auto Blocks< dimension >::blocks() const -> BlockRange
    {
        return { *this };
    }

    template < index_t dimension >
    auto Blocks< dimension >::active_blocks() const -> BlockRange
    {
        BlockRange range{ *this };
        range.set_active_only();
        return range;
    }

    template < index_t dimension >
    class Blocks< dimension >::BlockRangeBase::Impl
        : public BaseRange< typename Blocks< dimension >::Impl::Iterator >
    {
        using Iterator = typename Blocks< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Block< dimension >& block() const
        {
            return *this->current()->second;
        }
        void set_active_only()
        {
            active_only_ = true;
            next_block();
        }

        void next()
        {
            this->operator++();
            next_block();
        }

    private:
        void next_block()
        {
            while( this->operator!=( *this )
                   && ( active_only_ && !block().is_active() ) )
            {
                this->operator++();
            }
        }

    private:
        bool active_only_{ false };
    };

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase( const Blocks& blocks )
        : impl_( blocks.impl_->begin(), blocks.impl_->end() )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase(
        BlockRangeBase&& ) noexcept = default;

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::BlockRangeBase(
        const BlockRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRangeBase::~BlockRangeBase() = default;

    template < index_t dimension >
    bool Blocks< dimension >::BlockRangeBase::operator!=(
        const BlockRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Blocks< dimension >::BlockRangeBase::set_active_only()
    {
        impl_->set_active_only();
    }

    template < index_t dimension >
    void Blocks< dimension >::BlockRangeBase::operator++()
    {
        return impl_->next();
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRange::BlockRange( const Blocks& blocks )
        : BlockRangeBase( blocks )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRange::BlockRange( const BlockRange& range )
        : BlockRangeBase{ range }
    {
    }

    template < index_t dimension >
    Blocks< dimension >::BlockRange::~BlockRange() = default;

    template < index_t dimension >
    auto Blocks< dimension >::BlockRange::begin() const -> const BlockRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Blocks< dimension >::BlockRange::end() const -> const BlockRange&
    {
        return *this;
    }

    template < index_t dimension >
    const Block< dimension >& Blocks< dimension >::BlockRange::operator*() const
    {
        return this->impl_->block();
    }

    template < index_t dimension >
    auto Blocks< dimension >::modifiable_blocks( BlocksBuilderKey /*unused*/ )
        -> ModifiableBlockRange
    {
        return { *this };
    }

    template < index_t dimension >
    Blocks< dimension >::ModifiableBlockRange::ModifiableBlockRange(
        const Blocks& blocks )
        : BlockRangeBase( blocks )
    {
    }

    template < index_t dimension >
    Blocks< dimension >::ModifiableBlockRange::ModifiableBlockRange(
        const ModifiableBlockRange& range )
        : BlockRangeBase{ range }
    {
    }

    template < index_t dimension >
    Blocks< dimension >::ModifiableBlockRange::~ModifiableBlockRange() =
        default;

    template < index_t dimension >
    auto Blocks< dimension >::ModifiableBlockRange::begin() const
        -> const ModifiableBlockRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Blocks< dimension >::ModifiableBlockRange::end() const
        -> const ModifiableBlockRange&
    {
        return *this;
    }

    template < index_t dimension >
    Block< dimension >&
        Blocks< dimension >::ModifiableBlockRange::operator*() const
    {
        return this->impl_->block();
    }

    template class opengeode_model_api Blocks< 3 >;
} // namespace geode
