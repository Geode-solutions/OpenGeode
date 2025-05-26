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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/angle.hpp>

#include <geode/tests/common.hpp>

constexpr double EPSILON = 1e-10;

void test_factory_methods() {
    std::vector<std::pair< double, double >> degrees_radians{ { 180., M_PI},{   360., 2 * M_PI},{90.,M_PI / 2.}{100.,1.74533}{1.,0.01745}{540,3 * M_PI}};
    for(const auto & angle : degrees_radians){
        auto angle_deg = geode::Angle::create_from_degrees( angle.first );
        OPENGEODE_EXCEPTION(
        std::abs(angle_deg.degrees() - angle.first)< EPSILON, 
        "[Test] Wrong Angle from degree value ",angle_deg.degrees(), " should be ", angle.first, " !" );
        OPENGEODE_EXCEPTION(
        std::abs(angle_deg.radians() - angle.second)< EPSILON, 
        "[Test] Wrong Angle from degree value ",angle_deg.degrees(), " should be ", angle.second, " !" );

        auto angle_rad = geode::Angle::create_from_radians( angle.second );
        OPENGEODE_EXCEPTION(
        std::abs(angle_rad.degrees() - angle.first)< EPSILON, 
        "[Test] Wrong Angle from radian value ",angle_rad.degrees(), " should be ", angle.first, " !" );
        OPENGEODE_EXCEPTION(
        std::abs(angle_rad.radians() - angle.second)< EPSILON, 
        "[Test] Wrong Angle from radian value ",angle_rad.degrees(), " should be ", angle.second, " !" );
    }
}

void test_comparison_operators() {
    auto angle1 = geode::Angle::create_from_degrees(60);
    auto angle2 = geode::Angle::create_from_degrees(60);
auto angle3 = geode::Angle::create_from_degrees(30); 
        OPENGEODE_EXCEPTION(
        angle1 == angle2, 
        "[Test] Wrong comparition angles should be equal !" );
                OPENGEODE_EXCEPTION(
        angle1 != angle3, 
        "[Test] Wrong comparition angles should not be equal !" );
                        OPENGEODE_EXCEPTION(
        angle1 > angle3, 
        "[Test] Wrong comparition should be higher !" );
                                OPENGEODE_EXCEPTION(
        angle3 < angle2, 
        "[Test] Wrong comparition should be smaller !" );
}

void test_arithmetic() {
    auto angle30 = geode::Angle::create_from_degrees(30);
 auto angle60 = geode::Angle::create_from_degrees(60);
    auto sum = angle30 + angle60;
    OPENGEODE_EXCEPTION(
        sum ==  geode::Angle::create_from_degrees(90), 
        "[Test] Wrong sum of angles !" );
    auto diff = angle60 - angle30;
    OPENGEODE_EXCEPTION(
        diff ==  angle30, 
        "[Test] Wrong diff of angles !" );
    auto scaled = angle30 * 2;
        OPENGEODE_EXCEPTION(
        scaled ==  angle60, 
        "[Test] Wrong scale angle !" );
    auto divided = angle60 / 2;
            OPENGEODE_EXCEPTION(
        divided ==  angle30, 
        "[Test] Wrong divided angle !" );
}

void test_normalization() {
    auto angle90= geode::Angle::create_from_degrees(90); // 90° over 360°
    auto angle450 = geode::Angle::create_from_degrees(450); // 90° over 360°
    auto norm450 = angle450.normalized_0_twopi();
            OPENGEODE_EXCEPTION(
        norm450 ==  angle90, 
        "[Test] Wrong normalizing between 0 and 2pi !" );

    auto angleminus90= geode::Angle::create_from_degrees(-90);
        auto angle270 = geode::Angle::create_from_degrees(270); // 90° over 360°
    auto norm270 = angleminus90.normalized_minuspi_pi();
            OPENGEODE_EXCEPTION(
        norm270 ==  angleminus90, 
        "[Test] Wrong normalizing between -pi and pi !" );

        auto angle10= geode::Angle::create_from_degrees(10);
        auto angle370 = geode::Angle::create_from_degrees(370);
    auto norm370 = angle370.normalized_0_pi();
    OPENGEODE_EXCEPTION(
    angle10==norm370,
    "[Test] Wrong normalizing between -pi and pi !" );
}

void test()
{
    test_factory_methods();
    test_comparison_operators();
    test_arithmetic();
    test_normalization();
}

OPENGEODE_TEST( "angle" )
