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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( ModelBoundary );
    FORWARD_DECLARATION_DIMENSION_CLASS( ModelBoundariesBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class ModelBoundaries
    {
        OPENGEODE_DISABLE_COPY( ModelBoundaries );
        PASSKEY(
            ModelBoundariesBuilder< dimension >, ModelBoundariesBuilderKey );

    public:
        using Builder = ModelBoundariesBuilder< dimension >;
        using Type = ModelBoundary< dimension >;

        class opengeode_model_api ModelBoundaryRangeBase
        {
        public:
            ~ModelBoundaryRangeBase();

            [[nodiscard]] bool operator!=(
                const ModelBoundaryRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

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

            [[nodiscard]] const ModelBoundaryRange& begin() const;

            [[nodiscard]] const ModelBoundaryRange& end() const;

            [[nodiscard]] const ModelBoundary< dimension >& operator*() const;
        };

    public:
        ~ModelBoundaries();

        [[nodiscard]] index_t nb_model_boundaries() const;

        [[nodiscard]] index_t nb_active_model_boundaries() const;

        [[nodiscard]] const ModelBoundary< dimension >& model_boundary(
            const uuid& id ) const;

        [[nodiscard]] ModelBoundaryRange model_boundaries() const;

        [[nodiscard]] ModelBoundaryRange active_model_boundaries() const;

        [[nodiscard]] ModelBoundaryRange components() const
        {
            return model_boundaries();
        }

        void save_model_boundaries( std::string_view directory ) const;

    protected:
        ModelBoundaries();
        ModelBoundaries( ModelBoundaries&& other ) noexcept;
        ModelBoundaries& operator=( ModelBoundaries&& other ) noexcept;

    private:
        class ModifiableModelBoundaryRange : public ModelBoundaryRangeBase
        {
        public:
            ModifiableModelBoundaryRange( const ModelBoundaries& boundaries );
            ModifiableModelBoundaryRange(
                const ModifiableModelBoundaryRange& range );
            ~ModifiableModelBoundaryRange();

            [[nodiscard]] const ModifiableModelBoundaryRange& begin() const;

            [[nodiscard]] const ModifiableModelBoundaryRange& end() const;

            [[nodiscard]] ModelBoundary< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_model_boundary(
            ModelBoundariesBuilderKey key );

        void create_model_boundary(
            uuid model_boundary_id, ModelBoundariesBuilderKey key );

        void delete_model_boundary( const ModelBoundary< dimension >& boundary,
            ModelBoundariesBuilderKey key );

        void load_model_boundaries(
            std::string_view directory, ModelBoundariesBuilderKey key );

        [[nodiscard]] ModifiableModelBoundaryRange modifiable_model_boundaries(
            ModelBoundariesBuilderKey key );

        [[nodiscard]] ModelBoundary< dimension >& modifiable_model_boundary(
            const uuid& id, ModelBoundariesBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( ModelBoundaries );
} // namespace geode
