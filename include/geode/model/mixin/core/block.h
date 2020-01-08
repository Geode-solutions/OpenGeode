/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_type.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Blocks );
    FORWARD_DECLARATION_DIMENSION_CLASS( BlocksBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolid );
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
        OPENGEODE_DISABLE_COPY_AND_MOVE( Block );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        friend class Blocks< dimension >;

    public:
        ~Block();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Block" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        const PolyhedralSolid< dimension >& mesh() const;

    private:
        friend class bitsery::Access;
        Block();

        explicit Block( const MeshType& type );

        friend class BlocksBuilder< dimension >;
        PolyhedralSolid< dimension >& modifiable_mesh();

        friend class BlocksBuilder< dimension >;
        void set_mesh( std::unique_ptr< PolyhedralSolid< dimension > > mesh );

        friend class BlocksBuilder< dimension >;
        void set_block_name( absl::string_view name )
        {
            this->set_name( name );
        }

        void ensure_mesh_type();

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( Block );
} // namespace geode
