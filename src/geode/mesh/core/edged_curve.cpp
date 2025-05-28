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

#include <geode/mesh/core/edged_curve.hpp>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/texture1d.hpp>
#include <geode/mesh/core/texture_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class EdgedCurve< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        TextureManager1D texture_manager(
            const EdgedCurve< dimension >& curve ) const
        {
            return { curve.edge_attribute_manager(), texture_storage_ };
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.object( impl.texture_storage_ );
                } } } );
        }

    private:
        mutable TextureStorage1D texture_storage_;
    };

    template < index_t dimension >
    EdgedCurve< dimension >::EdgedCurve() = default;

    template < index_t dimension >
    EdgedCurve< dimension >::EdgedCurve( EdgedCurve&& ) noexcept = default;

    template < index_t dimension >
    EdgedCurve< dimension >& EdgedCurve< dimension >::operator=(
        EdgedCurve&& ) noexcept = default;

    template < index_t dimension >
    EdgedCurve< dimension >::~EdgedCurve() = default;

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > EdgedCurve< dimension >::create()
    {
        return MeshFactory::create_default_mesh< EdgedCurve< dimension > >(
            EdgedCurve< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > EdgedCurve< dimension >::create(
        const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< EdgedCurve< dimension > >( impl );
    }

    template < index_t dimension >
    MeshType EdgedCurve< dimension >::type_name_static()
    {
        return MeshType{ absl::StrCat( "EdgedCurve", dimension, "D" ) };
    }

    template < index_t dimension >
    double EdgedCurve< dimension >::edge_length( index_t edge_id ) const
    {
        const auto& e0 = this->point( edge_vertex( { edge_id, 0 } ) );
        const auto& e1 = this->point( edge_vertex( { edge_id, 1 } ) );
        return Vector< dimension >{ e0, e1 }.length();
    }

    template < index_t dimension >
    Point< dimension > EdgedCurve< dimension >::edge_barycenter(
        index_t edge_id ) const
    {
        const auto& e0 = this->point( edge_vertex( { edge_id, 0 } ) );
        const auto& e1 = this->point( edge_vertex( { edge_id, 1 } ) );
        return ( e0 + e1 ) / 2.;
    }

    template < index_t dimension >
    bool EdgedCurve< dimension >::is_edge_degenerated(
        index_t edge_id, double tolerance ) const
    {
        return edge_length( edge_id ) <= tolerance;
    }

    template < index_t dimension >
    bool EdgedCurve< dimension >::is_edge_degenerated( index_t edge_id ) const
    {
        return is_edge_degenerated( edge_id, GLOBAL_EPSILON );
    }

    template < index_t dimension >
    template < typename Archive >
    void EdgedCurve< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, EdgedCurve >{
                { []( Archive& a, EdgedCurve& edged_curve ) {
                     a.ext( edged_curve, bitsery::ext::BaseClass< Graph >{} );
                 },
                    []( Archive& a, EdgedCurve& edged_curve ) {
                        a.ext(
                            edged_curve, bitsery::ext::BaseClass< Graph >{} );
                        a.object( edged_curve.impl_ );
                    },
                    []( Archive& a, EdgedCurve& edged_curve ) {
                        a.ext(
                            edged_curve, bitsery::ext::BaseClass< Graph >{} );
                        a.ext(
                            edged_curve, bitsery::ext::BaseClass<
                                             CoordinateReferenceSystemManagers<
                                                 dimension > >{} );
                        a.object( edged_curve.impl_ );
                    } } } );
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > >
        EdgedCurve< dimension >::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = EdgedCurveBuilder< dimension >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    BoundingBox< dimension > EdgedCurve< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( this->point( p ) );
        }
        return box;
    }

    template < index_t dimension >
    Segment< dimension > EdgedCurve< dimension >::segment(
        index_t edge_id ) const
    {
        return { this->point( edge_vertex( { edge_id, 0 } ) ),
            this->point( edge_vertex( { edge_id, 1 } ) ) };
    }

    template < index_t dimension >
    TextureManager1D EdgedCurve< dimension >::texture_manager() const
    {
        return impl_->texture_manager( *this );
    }

    template class opengeode_mesh_api EdgedCurve< 1 >;
    template class opengeode_mesh_api EdgedCurve< 2 >;
    template class opengeode_mesh_api EdgedCurve< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgedCurve< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgedCurve< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgedCurve< 3 > );
} // namespace geode
