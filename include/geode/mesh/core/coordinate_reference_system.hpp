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

#include <absl/hash/hash.h>

#include <geode/basic/named_type.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    struct CRSTag
    {
    };

    using CRSType = NamedType< std::string, CRSTag >;

    template < index_t dimension >
    class CoordinateReferenceSystem
    {
        friend class bitsery::Access;

    public:
        virtual ~CoordinateReferenceSystem() = default;

        virtual CRSType type_name() const = 0;

        virtual const Point< dimension >& point( index_t point_id ) const = 0;

        virtual void set_point(
            index_t point_id, Point< dimension > point ) = 0;

        template < typename Type, typename Serializer >
        static void register_coordinate_reference_system_type(
            PContext& context, std::string_view name )
        {
            context.registerSingleBaseBranch< Serializer,
                CoordinateReferenceSystem, Type >( to_string( name ).c_str() );
        }

    protected:
        CoordinateReferenceSystem() = default;

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, CoordinateReferenceSystem >{
                    { []( Archive& /*unused*/,
                          CoordinateReferenceSystem& /*unused*/ ) {} } } );
        }
    };
    ALIAS_1D_AND_2D_AND_3D( CoordinateReferenceSystem );
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::CRSType >
    {
        std::size_t operator()( const geode::CRSType& type ) const
        {
            return absl::Hash< std::string >{}( type.get() );
        }
    };
} // namespace std
