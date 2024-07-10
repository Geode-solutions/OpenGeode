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

#include <geode/geometry/square_matrix.h>

#include <geode/basic/logger.h>

#include <geode/geometry/vector.h>

namespace
{
    double adj_coeff( const geode::SquareMatrix< 3 >& matrix,
        geode::local_index_t row,
        geode::local_index_t column )
    {
        const geode::local_index_t row1 = column == 0 ? 1 : 0;
        const geode::local_index_t row2 = column == 2 ? 1 : 2;
        const geode::local_index_t column1 = row == 0 ? 1 : 0;
        const geode::local_index_t column2 = row == 2 ? 1 : 2;
        return matrix.value( row1, column1 ) * matrix.value( row2, column2 )
               - matrix.value( row1, column2 ) * matrix.value( row2, column1 );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    SquareMatrix< dimension >::SquareMatrix( double default_value )
    {
        for( auto& row : matrix_rows_ )
        {
            for( const auto d : LRange{ dimension } )
            {
                row.set_value( d, default_value );
            }
        }
    }

    template < index_t dimension >
    SquareMatrix< dimension >::SquareMatrix(
        std::array< Vector< dimension >, dimension > matrix_rows )
        : matrix_rows_( std::move( matrix_rows ) )
    {
    }

    template < index_t dimension >
    double SquareMatrix< dimension >::value(
        geode::local_index_t row, geode::local_index_t column ) const
    {
        return matrix_rows_[row].value( column );
    }

    template < index_t dimension >
    void SquareMatrix< dimension >::set_value(
        local_index_t row, local_index_t column, double value )
    {
        matrix_rows_[row].set_value( column, value );
    }

    template < index_t dimension >
    geode::Vector< dimension > SquareMatrix< dimension >::operator*(
        const geode::Vector< dimension >& vector ) const
    {
        Vector< dimension > result;
        for( const auto d : LRange{ dimension } )
        {
            result.set_value( d, matrix_rows_[d].dot( vector ) );
        }
        return result;
    }

    template < index_t dimension >
    SquareMatrix< dimension > SquareMatrix< dimension >::operator*(
        const SquareMatrix< dimension >& matrix ) const
    {
        SquareMatrix< dimension > result{ 0 };
        for( const auto row : LRange{ dimension } )
        {
            for( const auto column : LRange{ dimension } )
            {
                for( const auto d : LRange{ dimension } )
                {
                    result.set_value( row, column,
                        result.value( row, column )
                            + value( row, d ) * matrix.value( d, column ) );
                }
            }
        }
        return result;
    }

    template < index_t dimension >
    double SquareMatrix< dimension >::determinant() const
    {
        throw OpenGeodeException{
            "[SquareMatrix::determinant] Not implemented for dimension ",
            dimension
        };
    }

    template <>
    double opengeode_geometry_api SquareMatrix< 2 >::determinant() const
    {
        return value( 0, 0 ) * value( 1, 1 ) - value( 0, 1 ) * value( 1, 0 );
    }

    template <>
    double opengeode_geometry_api SquareMatrix< 3 >::determinant() const
    {
        return value( 0, 0 )
                   * ( value( 1, 1 ) * value( 2, 2 )
                       - value( 1, 2 ) * value( 2, 1 ) )
               - value( 0, 1 )
                     * ( value( 1, 0 ) * value( 2, 2 )
                         - value( 1, 2 ) * value( 2, 0 ) )
               + value( 0, 2 )
                     * ( value( 1, 0 ) * value( 2, 1 )
                         - value( 1, 1 ) * value( 2, 0 ) );
    }

    template < index_t dimension >
    SquareMatrix< dimension > SquareMatrix< dimension >::transpose() const
    {
        std::array< Vector< dimension >, dimension > matrix_rows;
        for( const auto d1 : LRange{ dimension } )
        {
            for( const auto d2 : LRange{ dimension } )
            {
                matrix_rows[d1].set_value( d2, value( d2, d1 ) );
            }
        }
        return { std::move( matrix_rows ) };
    }

    template < index_t dimension >
    SquareMatrix< dimension > SquareMatrix< dimension >::inverse() const
    {
        throw OpenGeodeException{
            "[SquareMatrix::inverse] Not implemented for dimension ", dimension
        };
    }

    template <>
    SquareMatrix< 2 > opengeode_geometry_api SquareMatrix< 2 >::inverse() const
    {
        const auto det = determinant();
        OPENGEODE_EXCEPTION( det != 0,
            "[SquareMatrix::inverse] Cannot inverse matrix: determinant is "
            "null (given vectors do not form a well-defined base)." );

        std::array< Vector< 2 >, 2 > matrix_rows;
        matrix_rows[0].set_value( 0, value( 1, 1 ) / det );
        matrix_rows[0].set_value( 1, -value( 0, 1 ) / det );
        matrix_rows[1].set_value( 0, -value( 1, 0 ) / det );
        matrix_rows[1].set_value( 1, value( 0, 0 ) / det );
        return { std::move( matrix_rows ) };
    }

    template <>
    SquareMatrix< 3 > opengeode_geometry_api SquareMatrix< 3 >::inverse() const
    {
        const auto det = determinant();
        OPENGEODE_EXCEPTION( det != 0,
            "[SquareMatrix::inverse] Cannot inverse matrix: determinant is "
            "null (given vectors do not form a well-defined base)." );

        std::array< Vector3D, 3 > matrix_rows;
        matrix_rows[0] = Vector3D{ { adj_coeff( *this, 0, 0 ),
            -adj_coeff( *this, 0, 1 ), adj_coeff( *this, 0, 2 ) } };
        matrix_rows[1] = Vector3D{ { -adj_coeff( *this, 1, 0 ),
            adj_coeff( *this, 1, 1 ), -adj_coeff( *this, 1, 2 ) } };
        matrix_rows[2] = Vector3D{ { adj_coeff( *this, 2, 0 ),
            -adj_coeff( *this, 2, 1 ), adj_coeff( *this, 2, 2 ) } };
        for( const auto d : LRange{ 3 } )
        {
            matrix_rows[d] /= det;
        }
        return { std::move( matrix_rows ) };
    }

    template class opengeode_geometry_api SquareMatrix< 2 >;
    template class opengeode_geometry_api SquareMatrix< 3 >;
    template class opengeode_geometry_api SquareMatrix< 9 >;
} // namespace geode