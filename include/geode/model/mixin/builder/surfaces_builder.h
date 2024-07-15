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

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( Surfaces );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class SurfacesBuilder
    {
    public:
        void load_surfaces( std::string_view directory );

        /*!
         * Get a pointer to the builder of a Surface mesh
         * @param[in] id Unique index of the Surface
         */
        template < typename Mesh = SurfaceMesh< dimension > >
        std::unique_ptr< typename Mesh::Builder > surface_mesh_builder(
            const uuid& id )
        {
            auto& mesh = surfaces_.modifiable_surface( id, {} ).modifiable_mesh(
                typename Surface< dimension >::SurfacesBuilderKey{} );
            return MeshBuilderFactory::create_mesh_builder<
                typename Mesh::Builder >( dynamic_cast< Mesh& >( mesh ) );
        }

        void set_surface_name( const uuid& id, std::string_view name );

    protected:
        explicit SurfacesBuilder( Surfaces< dimension >& surfaces )
            : surfaces_( surfaces )
        {
        }

        const uuid& create_surface();

        const uuid& create_surface( const MeshImpl& impl );

        void create_surface( uuid surface_id );

        void create_surface( uuid surface_id, const MeshImpl& impl );

        void delete_surface( const Surface< dimension >& surface );

        void set_surface_mesh(
            const uuid& id, std::unique_ptr< SurfaceMesh< dimension > > mesh );

        SurfaceMesh< dimension >& modifiable_surface_mesh( const uuid& id );

    private:
        Surfaces< dimension >& surfaces_;
    };
    ALIAS_2D_AND_3D( SurfacesBuilder );
} // namespace geode
