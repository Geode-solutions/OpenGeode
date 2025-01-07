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

#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/coordinate_reference_system_managers.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/texture_manager.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurveBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class EdgedCurve : public Graph,
                       public CoordinateReferenceSystemManagers< dimension >
    {
        friend class bitsery::Access;

    public:
        using Builder = EdgedCurveBuilder< dimension >;
        static constexpr auto dim = dimension;

        ~EdgedCurve();

        [[nodiscard]] static std::unique_ptr< EdgedCurve< dimension > >
            create();

        [[nodiscard]] static std::unique_ptr< EdgedCurve< dimension > > create(
            const MeshImpl& impl );

        [[nodiscard]] static MeshType type_name_static();

        [[nodiscard]] std::unique_ptr< EdgedCurve< dimension > > clone() const;

        [[nodiscard]] double edge_length( index_t edge_id ) const;

        [[nodiscard]] Point< dimension > edge_barycenter(
            index_t edge_id ) const;

        /*!
         * Return if an edge is degenerated (means stands into an
         * epsilon-large ball)
         */
        [[nodiscard]] bool is_edge_degenerated( index_t edge_id ) const;

        [[nodiscard]] Segment< dimension > segment( index_t edge_id ) const;

        [[nodiscard]] TextureManager1D texture_manager() const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;

    protected:
        EdgedCurve();
        EdgedCurve( EdgedCurve&& other ) noexcept;
        EdgedCurve& operator=( EdgedCurve&& other ) noexcept;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( EdgedCurve );
} // namespace geode
