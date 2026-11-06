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

#include <memory>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geometry/coordinate_system.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/coordinate_reference_system.hpp>

namespace geode
{
    /*!
     * Coordinate reference system of a LightRegularGrid: the grid geometry
     * (origin and directions) expressed in this reference system.
     */
    template < index_t dimension >
    class GridCoordinateReferenceSystem
    {
        friend class bitsery::Access;

    public:
        explicit GridCoordinateReferenceSystem(
            CoordinateSystem< dimension > coordinate_system );
        virtual ~GridCoordinateReferenceSystem();

        [[nodiscard]] static CRSType type_name_static()
        {
            return CRSType{ "GridCoordinateReferenceSystem" };
        }

        [[nodiscard]] virtual CRSType type_name() const
        {
            return type_name_static();
        }

        [[nodiscard]] virtual std::unique_ptr<
            GridCoordinateReferenceSystem< dimension > >
            clone() const;

        [[nodiscard]] const CoordinateSystem< dimension >&
            coordinate_system() const;

        void set_coordinate_system(
            CoordinateSystem< dimension > coordinate_system );

        template < typename Type, typename Serializer >
        static void register_grid_coordinate_reference_system_type(
            PContext& context, std::string_view name )
        {
            context.registerSingleBaseBranch< Serializer,
                GridCoordinateReferenceSystem, Type >(
                to_string( name ).c_str() );
        }

    protected:
        GridCoordinateReferenceSystem();
        GridCoordinateReferenceSystem(
            const GridCoordinateReferenceSystem& other );

    private:
        template < typename Archive >
        void serialize( Archive& serializer );

    private:
        CoordinateSystem< dimension > coordinate_system_;
    };
    ALIAS_2D_AND_3D( GridCoordinateReferenceSystem );
} // namespace geode
