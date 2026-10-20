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
#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/tests/common.hpp>

void test_remove_foreign_component()
{
    geode::BRep other;
    geode::BRepBuilder other_builder{ other };
    const auto& foreign_id = other_builder.add_corner_collection();
    const auto& foreign = other.corner_collection( foreign_id );

    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& collection_id = builder.add_corner_collection();
    bool has_thrown{ false };
    try
    {
        builder.remove_corner_collection( foreign );
    }
    catch( const geode::OpenGeodeException& )
    {
        has_thrown = true;
    }
    geode::OpenGeodeModelException::test( has_thrown,
        "[Test] Removing a component from another model should throw" );
    geode::OpenGeodeModelException::test( model.nb_corner_collections() == 1
                                              && model.has_corner_collection(
                                                  collection_id ),
        "[Test] Model should still have its corner collection" );
    geode::OpenGeodeModelException::test( other.nb_corner_collections() == 1,
        "[Test] Other model should still have its corner collection" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_remove_foreign_component();
}

OPENGEODE_TEST( "model-builder-robustness" )
