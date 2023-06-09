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

#include <absl/container/fixed_array.h>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateReferenceSystem );
    FORWARD_DECLARATION_DIMENSION_CLASS(
        CoordinateReferenceSystemManagerBuilder );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManager
    {
        PASSKEY( CoordinateReferenceSystemManagerBuilder< dimension >,
            CRSManagerKey );
        friend class bitsery::Access;

    public:
        CoordinateReferenceSystemManager();
        CoordinateReferenceSystemManager( CoordinateReferenceSystemManager&& other );
        ~CoordinateReferenceSystemManager();

        index_t nb_coordinate_reference_systems() const;

        const CoordinateReferenceSystem< dimension >&
            find_coordinate_reference_system( absl::string_view name ) const;

        const CoordinateReferenceSystem< dimension >&
            active_coordinate_reference_system() const;

        absl::string_view active_coordinate_reference_system_name() const;

        absl::FixedArray< absl::string_view >
            coordinate_reference_system_names() const;

        bool coordinate_reference_system_exists( absl::string_view name ) const;

    public:
        void register_coordinate_reference_system( absl::string_view name,
            std::shared_ptr< CoordinateReferenceSystem< dimension > >&& crs,
            CRSManagerKey );

        void delete_coordinate_reference_system(
            absl::string_view name, CRSManagerKey );

        void set_active_coordinate_reference_system(
            absl::string_view name, CRSManagerKey );

        CoordinateReferenceSystem< dimension >&
            modifiable_active_coordinate_reference_system( CRSManagerKey );

        CoordinateReferenceSystem< dimension >&
            modifiable_coordinate_reference_system(
                absl::string_view name, CRSManagerKey );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( CoordinateReferenceSystemManager );
} // namespace geode