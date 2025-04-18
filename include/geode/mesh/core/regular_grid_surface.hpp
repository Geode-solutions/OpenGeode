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

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/grid.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGridBuilder );
    ALIAS_2D( RegularGrid );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api RegularGrid< 2 > : public SurfaceMesh< 2 >,
                                                public Grid< 2 >
    {
        friend class bitsery::Access;

    public:
        using Builder = RegularGridBuilder< 2 >;
        static constexpr index_t dim{ 2 };

        /*!
         * Create a new RegularGrid using default data structure.
         */
        [[nodiscard]] static std::unique_ptr< RegularGrid2D > create();

        /*!
         * Create a new RegularGrid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        [[nodiscard]] static std::unique_ptr< RegularGrid2D > create(
            const MeshImpl& impl );

        [[nodiscard]] static MeshType type_name_static();

        [[nodiscard]] std::string_view native_extension() const override
        {
            return native_extension_static();
        }

        [[nodiscard]] static std::string_view native_extension_static()
        {
            static const auto ext = "og_rgd2d";
            return ext;
        }

        [[nodiscard]] AttributeManager& cell_attribute_manager() const override;

        [[nodiscard]] AttributeManager&
            grid_vertex_attribute_manager() const override;

        [[nodiscard]] std::unique_ptr< RegularGrid2D > clone() const;

        [[nodiscard]] SurfaceMesh< 2 >::VerticesAroundVertex
            vertices_around_vertex( index_t vertex_id ) const final;

    protected:
        RegularGrid() = default;
        RegularGrid( RegularGrid&& other ) noexcept = default;
        RegularGrid& operator=( RegularGrid&& other ) noexcept = default;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        [[nodiscard]] local_index_t get_nb_polygon_vertices(
            index_t /*unused*/ ) const final
        {
            return nb_cell_vertices();
        }
    };
} // namespace geode
