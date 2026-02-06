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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/angle.hpp>

#include <geode/tests/common.hpp>

void test_factory_methods()
{
    std::vector< std::pair< double, double > > degrees_radians{
        { -720., -4. * M_PI }, { -360., -2. * M_PI },
        { -270., -3. * M_PI / 2. }, { -180., -M_PI },
        { -135., -3. * M_PI / 4. }, { -90., -M_PI / 2. }, { -45., -M_PI / 4. },
        { 0., 0. }, { 22.5, M_PI / 8. }, { 45., M_PI / 4. }, { 90., M_PI / 2. },
        { 135., 3. * M_PI / 4. }, { 180., M_PI }, { 270., 3. * M_PI / 2. },
        { 360., 2. * M_PI }, { 540., 3. * M_PI }, { 720., 4. * M_PI },
        { 1080., 6. * M_PI }
    };
    for( const auto& angle : degrees_radians )
    {
        auto angle_deg = geode::Angle::create_from_degrees( angle.first );
        OPENGEODE_EXCEPTION( std::abs( angle_deg.degrees() - angle.first )
                                 < geode::GLOBAL_ANGULAR_EPSILON,
            "[Test] Wrong Angle from degree value ", angle_deg.degrees(),
            " should be ", angle.first, " !" );
        OPENGEODE_EXCEPTION( std::abs( angle_deg.radians() - angle.second )
                                 < geode::GLOBAL_ANGULAR_EPSILON,
            "[Test] Wrong Angle from degree value ", angle_deg.degrees(),
            " should be ", angle.second, " !" );

        auto angle_rad = geode::Angle::create_from_radians( angle.second );
        OPENGEODE_EXCEPTION( std::abs( angle_rad.degrees() - angle.first )
                                 < geode::GLOBAL_ANGULAR_EPSILON,
            "[Test] Wrong Angle from radian value ", angle_rad.degrees(),
            " should be ", angle.first, " !" );
        OPENGEODE_EXCEPTION( std::abs( angle_rad.radians() - angle.second )
                                 < geode::GLOBAL_ANGULAR_EPSILON,
            "[Test] Wrong Angle from radian value ", angle_rad.degrees(),
            " should be ", angle.second, " !" );
    }
}

void test_comparison_operators()
{
    auto angle60 = geode::Angle::create_from_degrees( 60.00 );
    auto angle_almost60 = geode::Angle::create_from_degrees( 60.05 );
    auto angle_not60 = geode::Angle::create_from_degrees( 60.07 );
    auto angle30 = geode::Angle::create_from_degrees( 30.00 );

    OPENGEODE_EXCEPTION( angle60 == angle60,
        "[Test] Wrong == comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle60.degrees() ), " should be equal !" );
    OPENGEODE_EXCEPTION( !( angle60 == angle_not60 ),
        "[Test] Wrong == comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle_not60.degrees() ), " should be equal !" );
    OPENGEODE_EXCEPTION( !( angle60 == angle_almost60 ),
        "[Test] Wrong == comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle_almost60.degrees() ), " should not be equal !" );

    OPENGEODE_EXCEPTION( angle60.inexact_equal( angle60 ),
        "[Test] Wrong inexact_equal comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle60.degrees() ), " should be equal !" );
    OPENGEODE_EXCEPTION( !( angle60.inexact_equal( angle_not60 ) ),
        "[Test] Wrong inexact_equal comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle_not60.degrees() ), " should be equal !" );
    OPENGEODE_EXCEPTION( angle60.inexact_equal( angle_almost60 ),
        "[Test] Wrong inexact_equal comparition angles ",
        std::to_string( angle60.degrees() ), " and ",
        std::to_string( angle_almost60.degrees() ), " should be equal !" );

    OPENGEODE_EXCEPTION( angle_almost60 > angle60, "[Test] Wrong ",
        std::to_string( angle_almost60.degrees() ), " >",
        std::to_string( angle60.degrees() ),
        " comparition should be higher !" );
    OPENGEODE_EXCEPTION( angle_almost60 < angle_not60, "[Test] Wrong ",
        std::to_string( angle_almost60.degrees() ), " < ",
        std::to_string( angle_not60.degrees() ),
        " comparition should be smaller !" );
}

void test_arithmetic()
{
    auto angle30 = geode::Angle::create_from_degrees( 30 );
    auto angle60 = geode::Angle::create_from_degrees( 60 );
    auto sum = angle30 + angle60;
    OPENGEODE_EXCEPTION( sum == geode::Angle::create_from_degrees( 90 ),
        "[Test] Wrong sum of angles !" );
    auto diff = angle60 - angle30;
    OPENGEODE_EXCEPTION( diff == angle30, "[Test] Wrong diff of angles !" );
    auto scaled = angle30 * 2;
    OPENGEODE_EXCEPTION( scaled == angle60, "[Test] Wrong scale angle !" );
    auto divided = angle60 / 2;
    OPENGEODE_EXCEPTION( divided == angle30, "[Test] Wrong divided angle !" );
}

void test_normalization()
{
    std::vector< geode::Angle > to_normalize{ geode::Angle::create_from_radians(
                                                  -4. * M_PI ),
        geode::Angle::create_from_radians( -2. * M_PI ),
        geode::Angle::create_from_radians( -3. * M_PI / 2. ),
        geode::Angle::create_from_radians( -M_PI ),
        geode::Angle::create_from_radians( -3. * M_PI / 4. ),
        geode::Angle::create_from_radians( -M_PI / 2. ),
        geode::Angle::create_from_radians( -M_PI / 4. ),
        geode::Angle::create_from_radians( 0 ),
        geode::Angle::create_from_radians( M_PI / 8. ),
        geode::Angle::create_from_radians( M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 3. * M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( 3. * M_PI / 2. ),
        geode::Angle::create_from_radians( 2. * M_PI ),
        geode::Angle::create_from_radians( 3. * M_PI ),
        geode::Angle::create_from_radians( 4. * M_PI ),
        geode::Angle::create_from_radians( 6. * M_PI ) };

    std::vector< geode::Angle > norm_0_TWOPI{ geode::Angle::create_from_radians(
                                                  0. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( 5. * M_PI / 4. ),
        geode::Angle::create_from_radians( 3. * M_PI / 2. ),
        geode::Angle::create_from_radians( 7. * M_PI / 4. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 8. ),
        geode::Angle::create_from_radians( M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 3. * M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( 3. * M_PI / 2. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. ) };
    for( const auto angle_id : geode::Range( to_normalize.size() ) )
    {
        auto normalize = to_normalize[angle_id].normalized_between_0_and_2pi();
        OPENGEODE_EXCEPTION( normalize == norm_0_TWOPI[angle_id],
            "[Test] Wrong normalizing between 0 and 2pi - ", angle_id,
            " should be : ", norm_0_TWOPI[angle_id].degrees(), " but get ",
            normalize.degrees() );
    }

    std::vector< geode::Angle > norm_minusPI_PI{
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( -3. * M_PI / 4. ),
        geode::Angle::create_from_radians( -M_PI / 2. ),
        geode::Angle::create_from_radians( -M_PI / 4. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 8. ),
        geode::Angle::create_from_radians( M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 3. * M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( -M_PI / 2. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. )
    };
    for( const auto angle_id : geode::Range( to_normalize.size() ) )
    {
        auto normalize =
            to_normalize[angle_id].normalized_between_minuspi_and_pi();
        OPENGEODE_EXCEPTION( normalize == norm_minusPI_PI[angle_id],
            "[Test] Wrong normalizing between -pi and pi - ", angle_id,
            " should be : ", norm_minusPI_PI[angle_id].degrees(), " but get ",
            normalize.degrees() );
    }

    std::vector< geode::Angle > norm_0_PI{ geode::Angle::create_from_radians(
                                               0. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 3. * M_PI / 4. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 8. ),
        geode::Angle::create_from_radians( M_PI / 4. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 3. * M_PI / 4. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( M_PI / 2. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. ),
        geode::Angle::create_from_radians( 0. ) };
    for( const auto angle_id : geode::Range( to_normalize.size() ) )
    {
        auto normalize = to_normalize[angle_id].normalized_between_0_and_pi();
        OPENGEODE_EXCEPTION( normalize == norm_0_PI[angle_id],
            "[Test] Wrong normalizing between 0 and pi -", angle_id,
            " should be : ", norm_0_PI[angle_id].degrees(), " but get ",
            normalize.degrees() );
    }
}

void test()
{
    test_factory_methods();
    test_comparison_operators();
    test_arithmetic();
    test_normalization();
}

OPENGEODE_TEST( "angle" )
