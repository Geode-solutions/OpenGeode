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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/attribute_coordinate_reference_system.h>

#include <geode/model/helpers/model_coordinate_reference_system.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/section_input.h>

void check_result(
    absl::Span< const std::pair< geode::CRSType, std::string > > crss )
{
    OPENGEODE_EXCEPTION( crss.size() == 1, "[Test] Wrong number of CRS" );
    for( const auto& crs : crss )
    {
        OPENGEODE_EXCEPTION(
            crs.first
                == geode::AttributeCoordinateReferenceSystem3D::
                    type_name_static(),
            "[Test] Wrong CRS type" );
        OPENGEODE_EXCEPTION( crs.second == "points", "[Test] Wrong CRS name" );
    }
}

void test_brep_crs()
{
    const auto brep = geode::load_brep(
        absl::StrCat( geode::data_path, "random_dfn.og_brep" ) );
    const auto crss = geode::brep_coordinate_reference_systems( brep );
    check_result( crss );
}

void test_section_crs()
{
    const auto section = geode::load_section(
        absl::StrCat( geode::data_path, "fractures.og_sctn" ) );
    const auto crss = geode::section_coordinate_reference_systems( section );
    check_result( crss );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_brep_crs();
    test_section_crs();
}

OPENGEODE_TEST( "model-crs" )