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

#include <geode/mesh/common.h>
#include <geode/mesh/core/grid.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGridBuilder );
    ALIAS_3D( Point );
    ALIAS_3D( RegularGrid );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api RegularGrid< 3 > : public SolidMesh< 3 >,
                                                public Grid< 3 >
    {
        friend class bitsery::Access;

    public:
        using Builder = RegularGridBuilder< 3 >;
        static constexpr index_t dim{ 3 };

        /*!
         * Create a new RegularGrid using default data structure.
         */
        static std::unique_ptr< RegularGrid3D > create();

        /*!
         * Create a new RegularGrid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< RegularGrid3D > create( const MeshImpl& impl );

        static MeshType type_name_static();

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto ext = "og_rgd3d";
            return ext;
        }

        AttributeManager& cell_attribute_manager() const override;

        AttributeManager& grid_vertex_attribute_manager() const override;

        std::unique_ptr< RegularGrid3D > clone() const;

        SolidMesh< 3 >::VerticesAroundVertex vertices_around_vertex(
            index_t vertex_id ) const final;

    protected:
        RegularGrid() = default;
        RegularGrid( RegularGrid&& other ) noexcept = default;
        RegularGrid& operator=( RegularGrid&& other ) noexcept = default;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        local_index_t get_nb_polyhedron_vertices(
            index_t /*unused*/ ) const final
        {
            return nb_cell_vertices();
        }

        local_index_t get_nb_polyhedron_facets( index_t /*unused*/ ) const final
        {
            return nb_cell_neighbors();
        }

        local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& /*unused*/ ) const final
        {
            return 4;
        }
    };
} // namespace geode
