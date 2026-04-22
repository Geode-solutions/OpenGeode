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
#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

void test_polygons_around_vertex()
{
    auto moebius_strip = geode::load_triangulated_surface< 3 >(
        absl::StrCat( geode::DATA_PATH, "moebius_strip.og_tsf3d" ) );

    geode::index_t vertex_id_0{ 0 };
    auto polygons_around_0 =
        moebius_strip->polygons_around_vertex( vertex_id_0 );
    OPENGEODE_EXCEPTION( polygons_around_0.size() == 2,
        "[Test] Wrong computation of polygons around vertex 0 : should be 2 "
        "polygons, get ",
        polygons_around_0.size() );
    OPENGEODE_EXCEPTION( polygons_around_0[0].polygon_id == 159
                             && polygons_around_0[0].vertex_id == 2,
        "[TEST] Wrong polygons around vertex 0" );
    OPENGEODE_EXCEPTION( polygons_around_0[1].polygon_id == 0
                             && polygons_around_0[1].vertex_id == 0,
        "[TEST] Wrong polygons around vertex 0" );

    geode::index_t vertex_id_1{ 1 };
    auto polygons_around_1 =
        moebius_strip->polygons_around_vertex( vertex_id_1 );
    OPENGEODE_EXCEPTION( polygons_around_1.size() == 6,
        "[Test] Wrong computation of polygons around vertex 1 : should be 6 "
        "polygons, get ",
        polygons_around_1.size() );
    OPENGEODE_EXCEPTION( polygons_around_1[0].polygon_id == 159
                             && polygons_around_1[0].vertex_id == 1,
        "[TEST] Wrong polygons around vertex 1" );
    OPENGEODE_EXCEPTION( polygons_around_1[1].polygon_id == 158
                             && polygons_around_1[1].vertex_id == 1,
        "[TEST] Wrong polygons around vertex 1" );
    OPENGEODE_EXCEPTION( polygons_around_1[2].polygon_id == 157
                             && polygons_around_1[2].vertex_id == 2,
        "[TEST] Wrong polygons around vertex 1" );
    OPENGEODE_EXCEPTION( polygons_around_1[3].polygon_id == 2
                             && polygons_around_1[3].vertex_id == 0,
        "[TEST] Wrong polygons around vertex 1" );
    OPENGEODE_EXCEPTION( polygons_around_1[4].polygon_id == 1
                             && polygons_around_1[4].vertex_id == 0,
        "[TEST] Wrong polygons around vertex 1" );
    OPENGEODE_EXCEPTION( polygons_around_1[5].polygon_id == 0
                             && polygons_around_1[5].vertex_id == 2,
        "[TEST] Wrong polygons around vertex 1" );

    geode::index_t vertex_id_99{ 99 };
    auto polygons_around_99 =
        moebius_strip->polygons_around_vertex( vertex_id_99 );
    OPENGEODE_EXCEPTION( polygons_around_99.size() == 4,
        "[Test] Wrong computation of polygons around vertex 99 : should be 4 "
        "polygons, get ",
        polygons_around_99.size() );
    OPENGEODE_EXCEPTION( polygons_around_99[0].polygon_id == 153
                             && polygons_around_99[0].vertex_id == 1,
        "[TEST] Wrong polygons around vertex 99" );
    OPENGEODE_EXCEPTION( polygons_around_99[1].polygon_id == 152
                             && polygons_around_99[1].vertex_id == 1,
        "[TEST] Wrong polygons around vertex 99" );
    OPENGEODE_EXCEPTION( polygons_around_99[2].polygon_id == 6
                             && polygons_around_99[2].vertex_id == 2,
        "[TEST] Wrong polygons around vertex 99" );
    OPENGEODE_EXCEPTION( polygons_around_99[3].polygon_id == 7
                             && polygons_around_99[3].vertex_id == 0,
        "[TEST] Wrong polygons around vertex 99" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_polygons_around_vertex();
}

OPENGEODE_TEST( "moebius-strip" )