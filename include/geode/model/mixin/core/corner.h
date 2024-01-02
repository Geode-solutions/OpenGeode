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

#include <absl/strings/string_view.h>

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
        using Mesh = PointSet< dimension >;

        Corner( Corner&& other ) noexcept;
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

        const Mesh& mesh() const;

        Mesh& modifiable_mesh( CornersKey key );

        const MeshImpl& mesh_type() const;

    public:
        explicit Corner( CornersKey key );

        Corner( const MeshImpl& impl, CornersKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, CornersKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, CornersBuilderKey key );

        void set_corner_name( absl::string_view name, CornersBuilderKey key );

        Mesh& modifiable_mesh( CornersBuilderKey key );

    private:
        Corner();

        explicit Corner( const MeshImpl& impl );

        Mesh& modifiable_mesh();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Corner );
} // namespace geode
