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

#include <geode/model/common.h>

#include <geode/mesh/core/mesh_type.h>

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
        OPENGEODE_DISABLE_COPY_AND_MOVE( Surfaces );

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

        class opengeode_model_api SurfaceRange
            : public SurfaceRangeBase
        {
        public:
            SurfaceRange( const Surfaces& surfaces );

            const SurfaceRange& begin() const
            {
                return *this;
            }

            const SurfaceRange& end() const
            {
                return *this;
            }

            const Surface< dimension >& operator*() const;
        };

    public:
        ~Surfaces();

        index_t nb_surfaces() const;

        const Surface< dimension >& surface( const uuid& id ) const;

        SurfaceRange surfaces() const;

        std::vector< std::string > save_surfaces(
            const std::string& directory ) const;

    protected:
        Surfaces();

    private:
        class ModifiableSurfaceRange : public SurfaceRangeBase
        {
        public:
            ModifiableSurfaceRange( const Surfaces& surfaces );

            const ModifiableSurfaceRange& begin() const
            {
                return *this;
            }

            const ModifiableSurfaceRange& end() const
            {
                return *this;
            }

            Surface< dimension >& operator*() const;
        };

    private:
        const uuid& create_surface();

        const uuid& create_surface( const MeshType& type );

        void delete_surface( const Surface< dimension >& surface );

        void load_surfaces( const std::string& directory );

        ModifiableSurfaceRange modifiable_surfaces();

        Surface< dimension >& modifiable_surface( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Surfaces );
} // namespace geode
