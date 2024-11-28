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

#pragma once

#include <memory>
#include <string_view>

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Blocks );
    FORWARD_DECLARATION_DIMENSION_CLASS( BlocksBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
} // namespace geode

namespace geode
{
    /*!
     * Geometric component describing a 3D volume
     * This component is described by a mesh.
     */
    template < index_t dimension >
    class Block final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Block );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( Blocks< dimension >, BlocksKey );
        PASSKEY( BlocksBuilder< dimension >, BlocksBuilderKey );
        friend class bitsery::Access;

    public:
        using Mesh = SolidMesh< dimension >;

        Block( Block&& other ) noexcept;
        ~Block();

        [[nodiscard]] static ComponentType component_type_static()
        {
            return ComponentType{ "Block" };
        }

        [[nodiscard]] ComponentType component_type() const final
        {
            return component_type_static();
        }

        [[nodiscard]] ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        template < typename TypedMesh = Mesh >
        [[nodiscard]] const TypedMesh& mesh() const
        {
            return dynamic_cast< const TypedMesh& >( get_mesh() );
        }

        [[nodiscard]] const MeshImpl& mesh_type() const;

    public:
        explicit Block( BlocksKey key );

        Block( const MeshImpl& impl, BlocksKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, BlocksKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, BlocksBuilderKey key );

        template < typename TypedMesh = Mesh >
        [[nodiscard]] TypedMesh& modifiable_mesh( BlocksKey /*unused*/ )
        {
            return dynamic_cast< TypedMesh& >( get_modifiable_mesh() );
        }

        template < typename TypedMesh = Mesh >
        [[nodiscard]] TypedMesh& modifiable_mesh( BlocksBuilderKey /*unused*/ )
        {
            return dynamic_cast< TypedMesh& >( get_modifiable_mesh() );
        }

        void set_block_name( std::string_view name, BlocksBuilderKey key );

        [[nodiscard]] std::unique_ptr< Mesh > steal_mesh(
            BlocksBuilderKey key );

    private:
        Block();

        explicit Block( const MeshImpl& impl );

        [[nodiscard]] Mesh& get_modifiable_mesh();

        [[nodiscard]] const Mesh& get_mesh() const;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( Block );
} // namespace geode
