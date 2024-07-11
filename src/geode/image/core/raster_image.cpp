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

#include <geode/image/core/raster_image.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/internal/array_impl.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/image/core/rgb_color.h>

namespace geode
{
    template < index_t dimension >
    class RasterImage< dimension >::Impl
        : public internal::ArrayImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        const RGBColor& color( index_t index ) const
        {
            OPENGEODE_ASSERT( index < colors_.size(),
                "[RasterImage::set_color] Accessing a "
                "cell that does not exist" );
            return colors_[index];
        }

        void set_color( index_t index, RGBColor color )
        {
            OPENGEODE_ASSERT( index < colors_.size(),
                "[RasterImage::set_color] Accessing a "
                "cell that does not exist" );
            colors_[index] = std::move( color );
        }

        void resize( index_t nb_cells )
        {
            colors_.resize( nb_cells );
        }

        void copy_colors( const Impl& impl )
        {
            colors_ = impl.colors_;
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass<
                                     internal::ArrayImpl< dimension > >{} );
                    a.container( impl.colors_, impl.colors_.max_size() );
                } } } );
        }

    private:
        std::vector< RGBColor > colors_;
    };

    template < index_t dimension >
    RasterImage< dimension >::RasterImage() = default;

    template < index_t dimension >
    RasterImage< dimension >::~RasterImage() = default;

    template < index_t dimension >
    RasterImage< dimension >::RasterImage(
        std::array< index_t, dimension > cells_number )
        : CellArray< dimension >{ std::move( cells_number ) }
    {
        impl_->resize( this->nb_cells() );
    }

    template < index_t dimension >
    RasterImage< dimension >::RasterImage( RasterImage&& ) noexcept = default;

    template < index_t dimension >
    RasterImage< dimension >& RasterImage< dimension >::operator=(
        RasterImage&& ) noexcept = default;

    template < index_t dimension >
    index_t RasterImage< dimension >::cell_index(
        const CellIndices& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    template < index_t dimension >
    auto RasterImage< dimension >::cell_indices(
        index_t index ) const -> CellIndices
    {
        return impl_->cell_indices( *this, index );
    }

    template < index_t dimension >
    const RGBColor& RasterImage< dimension >::color( index_t index ) const
    {
        return impl_->color( index );
    }

    template < index_t dimension >
    void RasterImage< dimension >::set_color( index_t index, RGBColor color )
    {
        impl_->set_color( index, std::move( color ) );
    }

    template < index_t dimension >
    RasterImage< dimension > RasterImage< dimension >::clone() const
    {
        RasterImage< dimension > image;
        image.copy( *this );
        image.impl_->copy_colors( *impl_ );
        return image;
    }

    template < index_t dimension >
    template < typename Archive >
    void RasterImage< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, RasterImage >{
                { []( Archive& a, RasterImage& raster ) {
                    a.ext( raster,
                        bitsery::ext::BaseClass< CellArray< dimension > >{} );
                    a.object( raster.impl_ );
                } } } );
    }

    template class opengeode_image_api RasterImage< 1 >;
    template class opengeode_image_api RasterImage< 2 >;
    template class opengeode_image_api RasterImage< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, RasterImage< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, RasterImage< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, RasterImage< 3 > );
} // namespace geode
