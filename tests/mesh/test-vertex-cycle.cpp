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

#include <fstream>

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/detail/vertex_cycle.hpp>

#include <geode/tests/common.hpp>

void test_single_vertex_cycle()
{
    const geode::detail::VertexCycle< std::array< geode::index_t, 1 > > cycle1{
        { 3 }
    };
    const geode::detail::VertexCycle< std::array< geode::index_t, 1 > > cycle2{
        { 4 }
    };
    geode::OpenGeodeMeshException::test( cycle1 != cycle2,
        "Wrong result for operator!= with cycle1 and cycle2" );
}

void test_vertex_cycle()
{
    const geode::detail::VertexCycle< std::vector< geode::index_t > > cycle1{
        { 0, 1, 2, 3 }
    };
    const geode::detail::VertexCycle< std::vector< geode::index_t > > cycle2{
        { 2, 3, 0, 1 }
    };
    const geode::detail::VertexCycle< std::vector< geode::index_t > > cycle3{
        { 2, 3, 1, 0 }
    };
    const geode::detail::VertexCycle< std::vector< geode::index_t > > cycle4{
        { 1, 0, 3, 2 }
    };

    const geode::detail::VertexCycle< std::array< geode::index_t, 2 > > cycle5{
        { 2, 3 }
    };
    const geode::detail::VertexCycle< std::array< geode::index_t, 2 > > cycle6{
        { 3, 2 }
    };
    const geode::detail::VertexCycle< std::array< geode::index_t, 2 > > cycle7{
        { 3, 1 }
    };

    geode::OpenGeodeMeshException::test( cycle1 == cycle2,
        "Wrong result for operator== with cycle1 and cycle2" );
    geode::OpenGeodeMeshException::test( cycle1 != cycle3,
        "Wrong result for operator!= with cycle1 and cycle3" );
    geode::OpenGeodeMeshException::test( cycle1 == cycle4,
        "Wrong result for operator== with cycle1 and cycle4" );
    geode::OpenGeodeMeshException::test( cycle5 == cycle6,
        "Wrong result for operator== with cycle5 and cycle6" );
    geode::OpenGeodeMeshException::test( cycle5 != cycle7,
        "Wrong result for operator!= with cycle5 and cycle7" );
}

void test_oriented_vertex_cycle()
{
    const geode::detail::OrientedVertexCycle< std::vector< geode::index_t > >
        cycle1{ { 0, 1, 2, 3 } };
    const geode::detail::OrientedVertexCycle< std::vector< geode::index_t > >
        cycle2{ { 2, 3, 0, 1 } };
    const geode::detail::OrientedVertexCycle< std::vector< geode::index_t > >
        cycle3{ { 2, 3, 1, 0 } };
    const geode::detail::OrientedVertexCycle< std::vector< geode::index_t > >
        cycle4{ { 1, 0, 3, 2 } };

    const geode::detail::OrientedVertexCycle< std::array< geode::index_t, 2 > >
        cycle5{ { 2, 3 } };
    const geode::detail::OrientedVertexCycle< std::array< geode::index_t, 2 > >
        cycle6{ { 3, 2 } };
    const geode::detail::OrientedVertexCycle< std::array< geode::index_t, 2 > >
        cycle7{ { 3, 1 } };

    geode::OpenGeodeMeshException::test( cycle1 == cycle2,
        "Wrong result for operator== with cycle1 and cycle2" );
    geode::OpenGeodeMeshException::test( cycle1 != cycle3,
        "Wrong result for operator== with cycle1 and cycle3" );
    geode::OpenGeodeMeshException::test( cycle1 != cycle4,
        "Wrong result for operator!= with cycle1 and cycle4" );
    geode::OpenGeodeMeshException::test( cycle1.is_opposite( cycle4 ),
        "Wrong result for is_opposite with cycle1 and cycle4" );
    geode::OpenGeodeMeshException::test( cycle5 == cycle6,
        "Wrong result for operator== with cycle5 and cycle6" );
    geode::OpenGeodeMeshException::test( cycle5 != cycle7,
        "Wrong result for operator!= with cycle5 and cycle7" );
}

void test_several_containers()
{
    const geode::detail::VertexCycle< std::vector< geode::index_t > > cycle1{
        { 0, 1, 2, 3 }
    };
    const geode::detail::VertexCycle< absl::InlinedVector< geode::index_t, 4 > >
        cycle2{ { 2, 3, 0, 1 } };
    const geode::detail::VertexCycle< std::array< geode::index_t, 3 > > cycle3{
        { 2, 3, 1 }
    };

    geode::OpenGeodeMeshException::test( cycle1 == cycle2,
        "Wrong result for operator== with cycle1 and cycle2" );
    geode::OpenGeodeMeshException::test( cycle1 != cycle3,
        "Wrong result for operator!= with cycle1 and cycle3" );
}

void test()
{
    test_vertex_cycle();
    test_oriented_vertex_cycle();
    test_single_vertex_cycle();
    test_several_containers();
}

OPENGEODE_TEST( "vertex-cycle" )