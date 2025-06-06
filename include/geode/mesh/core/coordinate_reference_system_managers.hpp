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

#include <geode/mesh/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateReferenceSystemManager );
    FORWARD_DECLARATION_DIMENSION_CLASS(
        CoordinateReferenceSystemManagersBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_1D_AND_2D_AND_3D( CoordinateReferenceSystemManager );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManagers
    {
        PASSKEY( CoordinateReferenceSystemManagersBuilder< dimension >,
            CRSManagersKey );
        friend class bitsery::Access;

    public:
        ~CoordinateReferenceSystemManagers();

        [[nodiscard]] const CoordinateReferenceSystemManager1D&
            coordinate_reference_system_manager1D() const;

        [[nodiscard]] const CoordinateReferenceSystemManager2D&
            coordinate_reference_system_manager2D() const;

        [[nodiscard]] const CoordinateReferenceSystemManager3D&
            coordinate_reference_system_manager3D() const;

        [[nodiscard]] const CoordinateReferenceSystemManager< dimension >&
            main_coordinate_reference_system_manager() const;

        [[nodiscard]] const Point< dimension >& point( index_t vertex ) const;

    public:
        [[nodiscard]] CoordinateReferenceSystemManager1D&
            coordinate_reference_system_manager1D( CRSManagersKey );

        [[nodiscard]] CoordinateReferenceSystemManager2D&
            coordinate_reference_system_manager2D( CRSManagersKey );

        [[nodiscard]] CoordinateReferenceSystemManager3D&
            coordinate_reference_system_manager3D( CRSManagersKey );

        [[nodiscard]] CoordinateReferenceSystemManager< dimension >&
            main_coordinate_reference_system_manager( CRSManagersKey );

        void set_point(
            index_t vertex, Point< dimension > point, CRSManagersKey );

    protected:
        CoordinateReferenceSystemManagers();
        CoordinateReferenceSystemManagers(
            CoordinateReferenceSystemManagers&& other ) noexcept;
        CoordinateReferenceSystemManagers& operator=(
            CoordinateReferenceSystemManagers&& other ) noexcept;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( CoordinateReferenceSystemManagers );
} // namespace geode