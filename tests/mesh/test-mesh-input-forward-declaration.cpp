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

/*
 * The input headers only forward declare the meshes: instantiating every
 * member of their Input base class (as MSVC does for the virtual functions)
 * must not need the complete mesh type.
 */
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/tests/common.hpp>

template class geode::Input< std::unique_ptr< geode::EdgedCurve< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::HybridSolid< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::PointSet< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::PolygonalSurface< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::PolyhedralSolid< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::TetrahedralSolid< 3 > >,
    geode::MeshImpl >;
template class geode::Input< std::unique_ptr< geode::TriangulatedSurface< 3 > >,
    geode::MeshImpl >;

void test()
{
    geode::Logger::info(
        "Input base classes instantiated with forward declared meshes" );
}

OPENGEODE_TEST( "mesh-input-forward-declaration" )
