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

#include <geode/mesh/core/grid_coordinate_reference_system.hpp>

namespace geode
{
    template < index_t dimension >
    GridCoordinateReferenceSystem<
        dimension >::GridCoordinateReferenceSystem() = default;

    template < index_t dimension >
    GridCoordinateReferenceSystem< dimension >::GridCoordinateReferenceSystem(
        CoordinateSystem< dimension > coordinate_system )
        : coordinate_system_{ std::move( coordinate_system ) }
    {
    }

    template < index_t dimension >
    GridCoordinateReferenceSystem< dimension >::GridCoordinateReferenceSystem(
        const GridCoordinateReferenceSystem& ) = default;

    template < index_t dimension >
    GridCoordinateReferenceSystem<
        dimension >::~GridCoordinateReferenceSystem() = default;

    template < index_t dimension >
    std::unique_ptr< GridCoordinateReferenceSystem< dimension > >
        GridCoordinateReferenceSystem< dimension >::clone() const
    {
        return std::unique_ptr< GridCoordinateReferenceSystem< dimension > >{
            new GridCoordinateReferenceSystem< dimension >{ *this }
        };
    }

    template < index_t dimension >
    const CoordinateSystem< dimension >&
        GridCoordinateReferenceSystem< dimension >::coordinate_system() const
    {
        return coordinate_system_;
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystem< dimension >::set_coordinate_system(
        CoordinateSystem< dimension > coordinate_system )
    {
        coordinate_system_ = std::move( coordinate_system );
    }

    template < index_t dimension >
    template < typename Archive >
    void GridCoordinateReferenceSystem< dimension >::serialize(
        Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, GridCoordinateReferenceSystem >{
                { []( Archive& archive, GridCoordinateReferenceSystem& crs ) {
                    archive.object( crs.coordinate_system_ );
                } } } );
    }

    template class opengeode_mesh_api GridCoordinateReferenceSystem< 2 >;
    template class opengeode_mesh_api GridCoordinateReferenceSystem< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, GridCoordinateReferenceSystem< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, GridCoordinateReferenceSystem< 3 > );
} // namespace geode
