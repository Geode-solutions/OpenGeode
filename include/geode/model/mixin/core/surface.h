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
        Surface( Surface&& );
        ~Surface();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Surface" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        template < typename Mesh = SurfaceMesh< dimension > >
        const Mesh& mesh() const
        {
            return dynamic_cast< const Mesh& >( get_mesh() );
        }

        Surface( SurfacesKey ) : Surface() {}

        Surface( const MeshImpl& impl, SurfacesKey ) : Surface( impl ){};

        template < typename Mesh = SurfaceMesh< dimension > >
        Mesh& modifiable_mesh( SurfacesKey )
        {
            return dynamic_cast< Mesh& >( modifiable_mesh() );
        }

        const MeshImpl& mesh_type() const;

        void set_mesh(
            std::unique_ptr< SurfaceMesh< dimension > > mesh, SurfacesKey );

        void set_mesh( std::unique_ptr< SurfaceMesh< dimension > > mesh,
            SurfacesBuilderKey );

        void set_surface_name( absl::string_view name, SurfacesBuilderKey )
        {
            this->set_name( name );
        }

        template < typename Mesh = SurfaceMesh< dimension > >
        Mesh& modifiable_mesh( SurfacesBuilderKey )
        {
            return dynamic_cast< Mesh& >( modifiable_mesh() );
        }

    private:
        Surface();

        explicit Surface( const MeshImpl& impl );

        SurfaceMesh< dimension >& modifiable_mesh();

        const SurfaceMesh< dimension >& get_mesh() const;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Surface );
} // namespace geode
