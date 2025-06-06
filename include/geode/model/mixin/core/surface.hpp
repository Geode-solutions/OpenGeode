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
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( Surfaces );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfacesBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Geometric component describing a Surface.
     * This component is described by a mesh.
     */
    template < index_t dimension >
    class Surface final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Surface );
        PASSKEY( Surfaces< dimension >, SurfacesKey );
        PASSKEY( SurfacesBuilder< dimension >, SurfacesBuilderKey );
        friend class bitsery::Access;

    public:
        using Mesh = SurfaceMesh< dimension >;

        Surface( Surface&& other ) noexcept;
        ~Surface();

        [[nodiscard]] static ComponentType component_type_static()
        {
            return ComponentType{ "Surface" };
        }

        [[nodiscard]] ComponentType component_type() const final
        {
            return component_type_static();
        }

        template < typename TypedMesh = Mesh >
        [[nodiscard]] const TypedMesh& mesh() const
        {
            return dynamic_cast< const TypedMesh& >( get_mesh() );
        }

    public:
        explicit Surface( SurfacesKey key );

        Surface( const MeshImpl& impl, SurfacesKey key );

        template < typename TypedMesh = Mesh >
        [[nodiscard]] TypedMesh& modifiable_mesh( SurfacesKey /*unused*/ )
        {
            return dynamic_cast< TypedMesh& >( get_modifiable_mesh() );
        }

        [[nodiscard]] const MeshImpl& mesh_type() const;

        void set_mesh( std::unique_ptr< Mesh > mesh, SurfacesKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, SurfacesBuilderKey key );

        void set_surface_name( std::string_view name, SurfacesBuilderKey key );

        void set_surface_active( bool active, SurfacesBuilderKey key );

        template < typename TypedMesh = Mesh >
        [[nodiscard]] TypedMesh& modifiable_mesh(
            SurfacesBuilderKey /*unused*/ )
        {
            return dynamic_cast< TypedMesh& >( get_modifiable_mesh() );
        }

        [[nodiscard]] std::unique_ptr< Mesh > steal_mesh(
            SurfacesBuilderKey key );

    private:
        Surface();

        explicit Surface( const MeshImpl& impl );

        [[nodiscard]] Mesh& get_modifiable_mesh();

        [[nodiscard]] const Mesh& get_mesh() const;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Surface );
} // namespace geode
