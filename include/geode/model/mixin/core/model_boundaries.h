/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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
    FORWARD_DECLARATION_DIMENSION_CLASS( ModelBoundary );
    FORWARD_DECLARATION_DIMENSION_CLASS( ModelBoundariesBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_model_api ModelBoundaries
    {
        OPENGEODE_DISABLE_COPY( ModelBoundaries );

    public:
        using Builder = ModelBoundariesBuilder< dimension >;
        friend Builder;

        class opengeode_model_api ModelBoundaryRangeBase
        {
        public:
            ~ModelBoundaryRangeBase();

            bool operator!=( const ModelBoundaryRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            ModelBoundaryRangeBase( const ModelBoundaries& boundaries );
            ModelBoundaryRangeBase( ModelBoundaryRangeBase&& other ) noexcept;
            ModelBoundaryRangeBase( const ModelBoundaryRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api ModelBoundaryRange
            : public ModelBoundaryRangeBase
        {
        public:
            ModelBoundaryRange( const ModelBoundaries& boundaries );
            ModelBoundaryRange( const ModelBoundaryRange& range );
            ~ModelBoundaryRange();

            const ModelBoundaryRange& begin() const;

            const ModelBoundaryRange& end() const;

            const ModelBoundary< dimension >& operator*() const;
        };

    public:
        ~ModelBoundaries();

        index_t nb_model_boundaries() const;

        const ModelBoundary< dimension >& model_boundary(
            const uuid& id ) const;

        ModelBoundaryRange model_boundaries() const;

        void save_model_boundaries( absl::string_view directory ) const;

    protected:
        ModelBoundaries();
        ModelBoundaries( ModelBoundaries&& other );
        ModelBoundaries& operator=( ModelBoundaries&& other );

    private:
        class ModifiableModelBoundaryRange : public ModelBoundaryRangeBase
        {
        public:
            ModifiableModelBoundaryRange( const ModelBoundaries& boundaries );
            ModifiableModelBoundaryRange(
                const ModifiableModelBoundaryRange& range );
            ~ModifiableModelBoundaryRange();

            const ModifiableModelBoundaryRange& begin() const;

            const ModifiableModelBoundaryRange& end() const;

            ModelBoundary< dimension >& operator*() const;
        };

    private:
        const uuid& create_model_boundary();

        void delete_model_boundary(
            const ModelBoundary< dimension >& boundary );

        void load_model_boundaries( absl::string_view directory );

        ModifiableModelBoundaryRange modifiable_model_boundaries();

        ModelBoundary< dimension >& modifiable_model_boundary( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( ModelBoundaries );
} // namespace geode
