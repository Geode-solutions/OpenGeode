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

#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfacesBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_model_api Surfaces
    {
        OPENGEODE_DISABLE_COPY( Surfaces );

    public:
        using Builder = SurfacesBuilder< dimension >;
        friend Builder;

        class opengeode_model_api SurfaceRangeBase
        {
        public:
            ~SurfaceRangeBase();

            bool operator!=( const SurfaceRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            SurfaceRangeBase( const Surfaces& surfaces );
            SurfaceRangeBase( SurfaceRangeBase&& other ) noexcept;
            SurfaceRangeBase( const SurfaceRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api SurfaceRange : public SurfaceRangeBase
        {
        public:
            SurfaceRange( const Surfaces& surfaces );
            SurfaceRange( const SurfaceRange& range );
            ~SurfaceRange();

            const SurfaceRange& begin() const;

            const SurfaceRange& end() const;

            const Surface< dimension >& operator*() const;
        };

    public:
        ~Surfaces();

        index_t nb_surfaces() const;

        bool has_surface( const uuid& id ) const;

        const Surface< dimension >& surface( const uuid& id ) const;

        SurfaceRange surfaces() const;

        void save_surfaces( absl::string_view directory ) const;

    protected:
        Surfaces();
        Surfaces( Surfaces&& other ) noexcept;

        Surfaces& operator=( Surfaces&& other ) noexcept;

    private:
        class ModifiableSurfaceRange : public SurfaceRangeBase
        {
        public:
            ModifiableSurfaceRange( const Surfaces& surfaces );
            ModifiableSurfaceRange( const ModifiableSurfaceRange& range );
            ~ModifiableSurfaceRange();

            const ModifiableSurfaceRange& begin() const;

            const ModifiableSurfaceRange& end() const;

            Surface< dimension >& operator*() const;
        };

    private:
        const uuid& create_surface();

        const uuid& create_surface( const MeshImpl& impl );

        void create_surface( uuid surface_id );

        void create_surface( uuid surface_id, const MeshImpl& impl );

        void delete_surface( const Surface< dimension >& surface );

        void load_surfaces( absl::string_view directory );

        ModifiableSurfaceRange modifiable_surfaces();

        Surface< dimension >& modifiable_surface( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Surfaces );
} // namespace geode
