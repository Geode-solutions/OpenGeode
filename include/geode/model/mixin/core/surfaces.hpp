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

#pragma once

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>

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
        PASSKEY( SurfacesBuilder< dimension >, SurfacesBuilderKey );

    public:
        using Builder = SurfacesBuilder< dimension >;
        using Type = Surface< dimension >;

        class opengeode_model_api SurfaceRangeBase
        {
        public:
            ~SurfaceRangeBase();

            [[nodiscard]] bool operator!=(
                const SurfaceRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

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

            [[nodiscard]] const SurfaceRange& begin() const;

            [[nodiscard]] const SurfaceRange& end() const;

            [[nodiscard]] const Surface< dimension >& operator*() const;
        };

    public:
        ~Surfaces();

        [[nodiscard]] index_t nb_surfaces() const;

        [[nodiscard]] index_t nb_active_surfaces() const;

        [[nodiscard]] bool has_surface( const uuid& id ) const;

        [[nodiscard]] const Surface< dimension >& surface(
            const uuid& id ) const;

        [[nodiscard]] SurfaceRange surfaces() const;

        [[nodiscard]] SurfaceRange active_surfaces() const;

        [[nodiscard]] SurfaceRange components() const
        {
            return surfaces();
        }

        [[nodiscard]] bool has_component( const uuid& id ) const
        {
            return has_surface( id );
        }

        [[nodiscard]] const Surface< dimension >& component(
            const uuid& id ) const
        {
            return surface( id );
        }

        void save_surfaces( std::string_view directory ) const;

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

            [[nodiscard]] const ModifiableSurfaceRange& begin() const;

            [[nodiscard]] const ModifiableSurfaceRange& end() const;

            [[nodiscard]] Surface< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_surface( SurfacesBuilderKey key );

        [[nodiscard]] const uuid& create_surface(
            const MeshImpl& impl, SurfacesBuilderKey key );

        void create_surface( uuid surface_id, SurfacesBuilderKey key );

        void create_surface(
            uuid surface_id, const MeshImpl& impl, SurfacesBuilderKey key );

        void delete_surface(
            const Surface< dimension >& surface, SurfacesBuilderKey key );

        void load_surfaces(
            std::string_view directory, SurfacesBuilderKey key );

        [[nodiscard]] ModifiableSurfaceRange modifiable_surfaces(
            SurfacesBuilderKey key );

        [[nodiscard]] Surface< dimension >& modifiable_surface(
            const uuid& id, SurfacesBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Surfaces );
} // namespace geode
