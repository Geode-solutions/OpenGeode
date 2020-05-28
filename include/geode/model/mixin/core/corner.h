/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Corners );
    FORWARD_DECLARATION_DIMENSION_CLASS( CornersBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
} // namespace geode

namespace geode
{
    /*!
     * Geometric component describing a Point.
     * This component is described by a mesh.
     */
    template < index_t dimension >
    class Corner final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Corner );
        PASSKEY( Corners< dimension >, CornersKey );
        PASSKEY( CornersBuilder< dimension >, CornersBuilderKey );
        friend class bitsery::Access;

    public:
        Corner( Corner&& );
        ~Corner();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Corner" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        const PointSet< dimension >& mesh() const;

        PointSet< dimension >& modifiable_mesh( CornersKey )
        {
            return modifiable_mesh();
        }

        const MeshType& mesh_type() const;

        Corner( CornersKey ) : Corner() {}

        Corner( const MeshType& type, CornersKey ) : Corner( type ) {}

        void set_mesh(
            std::unique_ptr< PointSet< dimension > > mesh, CornersKey );

        void set_mesh(
            std::unique_ptr< PointSet< dimension > > mesh, CornersBuilderKey );

        void set_corner_name( absl::string_view name, CornersBuilderKey )
        {
            this->set_name( name );
        }

        PointSet< dimension >& modifiable_mesh( CornersBuilderKey )
        {
            return modifiable_mesh();
        }

    private:
        Corner();

        explicit Corner( const MeshType& type );

        PointSet< dimension >& modifiable_mesh();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Corner );
} // namespace geode
