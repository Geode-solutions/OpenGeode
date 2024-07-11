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

#include <geode/mesh/core/attribute_coordinate_reference_system.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/internal/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class AttributeCoordinateReferenceSystem< dimension >::Impl
        : public internal::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        Impl( AttributeManager& manager )
            : internal::PointsImpl< dimension >{ manager }
        {
        }
        Impl( AttributeManager& manager, std::string_view attribute_name )
            : internal::PointsImpl< dimension >{ manager, attribute_name }
        {
        }

        Impl() = default;

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass<
                                     internal::PointsImpl< dimension > >{} );
                } } } );
        }
    };

    template < index_t dimension >
    AttributeCoordinateReferenceSystem<
        dimension >::AttributeCoordinateReferenceSystem()
    {
    }

    template < index_t dimension >
    AttributeCoordinateReferenceSystem< dimension >::
        AttributeCoordinateReferenceSystem( AttributeManager& manager )
        : impl_{ manager }
    {
    }

    template < index_t dimension >
    AttributeCoordinateReferenceSystem< dimension >::
        AttributeCoordinateReferenceSystem(
            AttributeManager& manager, std::string_view attribute_name )
        : impl_{ manager, attribute_name }
    {
    }

    template < index_t dimension >
    AttributeCoordinateReferenceSystem<
        dimension >::~AttributeCoordinateReferenceSystem()
    {
    }

    template < index_t dimension >
    const Point< dimension >&
        AttributeCoordinateReferenceSystem< dimension >::point(
            index_t point_id ) const
    {
        return impl_->get_point( point_id );
    }

    template < index_t dimension >
    void AttributeCoordinateReferenceSystem< dimension >::set_point(
        index_t point_id, Point< dimension > point )
    {
        impl_->set_point( point_id, std::move( point ) );
    }

    template < index_t dimension >
    std::string_view
        AttributeCoordinateReferenceSystem< dimension >::attribute_name() const
    {
        return impl_->attribute_name();
    }

    template < index_t dimension >
    index_t AttributeCoordinateReferenceSystem< dimension >::nb_points() const
    {
        return impl_->nb_points();
    }

    template < index_t dimension >
    template < typename Archive >
    void AttributeCoordinateReferenceSystem< dimension >::serialize(
        Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, AttributeCoordinateReferenceSystem >{
                { []( Archive& a, AttributeCoordinateReferenceSystem& crs ) {
                    a.ext(
                        crs, bitsery::ext::BaseClass<
                                 CoordinateReferenceSystem< dimension > >{} );
                    a.object( crs.impl_ );
                } } } );
    }

    template class opengeode_mesh_api AttributeCoordinateReferenceSystem< 1 >;
    template class opengeode_mesh_api AttributeCoordinateReferenceSystem< 2 >;
    template class opengeode_mesh_api AttributeCoordinateReferenceSystem< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, AttributeCoordinateReferenceSystem< 1 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, AttributeCoordinateReferenceSystem< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, AttributeCoordinateReferenceSystem< 3 > );
} // namespace geode