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

#include <geode/geometry/basic_objects/cylinder.h>

#include <geode/basic/logger.h>

namespace
{
    // Gram-Schmidt orthonormalization to generate orthonormal vectors from
    // the linearly independent inputs.  The function returns the smallest
    // length of the unnormalized vectors computed during the process.  If
    // this value is nearly zero, it is possible that the inputs are linearly
    // dependent (within numerical round-off errors).
    void orthonormalize( geode::Basis3D& basis )
    {
        for( const auto i : geode::LRange{ 3 } )
        {
            for( const auto j : geode::LRange{ i } )
            {
                const auto dot_ij = basis[i].dot( basis[j] );
                basis[i] -= basis[j] * dot_ij;
            }
            basis[i] = basis[i].normalize();
        }
    }

    // Compute a right-handed orthonormal basis for the orthogonal complement
    // of the input vectors.  The function returns the smallest length of the
    // unnormalized vectors computed during the process.  If this value is
    // nearly zero, it is possible that the inputs are linearly dependent
    // (within numerical round-off errors).
    geode::Basis3D compute_orthogonal_basis( const geode::Vector3D& axis )
    {
        geode::Basis3D basis;
        basis[0] = axis;
        if( std::fabs( axis.value( 0 ) ) > std::fabs( axis.value( 1 ) ) )
        {
            basis[1] = { { -axis.value( 2 ), 0, axis.value( 0 ) } };
        }
        else
        {
            basis[1] = { { 0, axis.value( 2 ), -axis.value( 1 ) } };
        }
        basis[2] = basis[0].cross( basis[1] );
        orthonormalize( basis );
        return basis;
    }
} // namespace

namespace geode
{

    Cylinder::Cylinder( const Segment3D& axis, double radius )
        : axis_( axis ),
          radius_( radius ),
          basis_( compute_orthogonal_basis( axis.normalized_direction() ) )
    {
    }

    Cylinder::Cylinder( const Cylinder& other )
        : axis_( other.axis_ ), radius_( other.radius_ ), basis_( other.basis_ )
    {
    }

    Cylinder& Cylinder::operator=( const Cylinder& other )
    {
        axis_ = other.axis_;
        radius_ = other.radius_;
        basis_ = other.basis_;
        return *this;
    }

    Cylinder::Cylinder( Cylinder&& other )
        : axis_( other.axis_ ), radius_( other.radius_ ), basis_( other.basis_ )
    {
    }

    Cylinder& Cylinder::operator=( Cylinder&& other )
    {
        axis_ = other.axis_;
        radius_ = other.radius_;
        basis_ = other.basis_;
        return *this;
    }

    const Segment3D& Cylinder::axis() const
    {
        return axis_;
    }
    const Basis3D& Cylinder::basis() const
    {
        return basis_;
    }
    double Cylinder::radius() const
    {
        return radius_;
    }
} // namespace geode
