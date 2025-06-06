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

#include <geode/mesh/helpers/tetrahedral_solid_point_function.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/tetrahedral_solid.hpp>

namespace geode
{
    template < index_t dimension, index_t point_dimension >
    class TetrahedralSolidPointFunction< dimension, point_dimension >::Impl
    {
    public:
        Impl( const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            Point< point_dimension > value )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                !solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidPointFunction: attribute with "
                "name '",
                function_name, "' already exists." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< point_dimension > >(
                        function_name, std::move( value ), { false, true } );
        }

        Impl( const TetrahedralSolid< dimension >& solid,
            std::string_view function_name )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidPointFunction: attribute with "
                "name '",
                function_name, "' does not exist." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< point_dimension > >( function_name,
                        Point< point_dimension >(), { false, true } );
        }

        void set_value( index_t vertex_id, Point< point_dimension > value )
        {
            function_attribute_->set_value( vertex_id, std::move( value ) );
        }

        const Point< point_dimension >& value( index_t vertex_id ) const
        {
            return function_attribute_->value( vertex_id );
        }

        Point< point_dimension > value(
            const Point< dimension >& point, index_t tetrahedron_id ) const
        {
            Point< point_dimension > point_value;
            const auto tetrahedron = solid_.tetrahedron( tetrahedron_id );
            const auto tetrahedron_vertices =
                solid_.polyhedron_vertices( tetrahedron_id );
            const auto barycentric_coords =
                tetrahedron_barycentric_coordinates( point, tetrahedron );
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                point_value +=
                    function_attribute_->value( tetrahedron_vertices[node_id] )
                    * barycentric_coords[node_id];
            }
            return point_value;
        }

    private:
        const TetrahedralSolid< dimension >& solid_;
        std::shared_ptr< VariableAttribute< Point< point_dimension > > >
            function_attribute_;
    };

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension, point_dimension >::
        TetrahedralSolidPointFunction(
            TetrahedralSolidPointFunction< dimension, point_dimension >&&
                other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension, point_dimension >::
        TetrahedralSolidPointFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            Point< point_dimension > value )
        : impl_{ solid, function_name, value }
    {
    }

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension, point_dimension >::
        TetrahedralSolidPointFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name )
        : impl_{ solid, function_name }
    {
    }

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension,
        point_dimension >::~TetrahedralSolidPointFunction() = default;

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension, point_dimension >
        TetrahedralSolidPointFunction< dimension, point_dimension >::create(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            Point< point_dimension > value )
    {
        return { solid, function_name, value };
    }

    template < index_t dimension, index_t point_dimension >
    TetrahedralSolidPointFunction< dimension, point_dimension >
        TetrahedralSolidPointFunction< dimension, point_dimension >::find(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name )
    {
        return { solid, function_name };
    }

    template < index_t dimension, index_t point_dimension >
    void TetrahedralSolidPointFunction< dimension, point_dimension >::set_value(
        index_t vertex_index, Point< point_dimension > value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension, index_t point_dimension >
    const Point< point_dimension >&
        TetrahedralSolidPointFunction< dimension, point_dimension >::value(
            index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension, index_t point_dimension >
    Point< point_dimension >
        TetrahedralSolidPointFunction< dimension, point_dimension >::value(
            const Point< dimension >& point, index_t tetrahedron_id ) const
    {
        return impl_->value( point, tetrahedron_id );
    }

    template class opengeode_mesh_api TetrahedralSolidPointFunction< 3, 3 >;
    template class opengeode_mesh_api TetrahedralSolidPointFunction< 3, 2 >;
    template class opengeode_mesh_api TetrahedralSolidPointFunction< 3, 1 >;
} // namespace geode