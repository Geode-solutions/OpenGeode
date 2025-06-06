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

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/mesh_factory.hpp>

#include <geode/tests/common.hpp>

class MeshTest : public geode::VertexSet
{
public:
    MeshTest() = default;

    std::string_view native_extension() const override
    {
        static auto ext = "ext";
        return ext;
    }

    geode::MeshImpl impl_name() const override
    {
        return geode::MeshImpl{ "type" };
    }

    geode::MeshType type_name() const override
    {
        return geode::MeshType{ "type" };
    }
};

void test()
{
    geode::MeshImpl impl{ "impl" };
    geode::MeshType type{ "type" };
    geode::MeshFactory::register_mesh< MeshTest >( type, impl );

    OPENGEODE_EXCEPTION(
        geode::MeshFactory::type( impl ) == type, "Wrong type" );

    const auto mesh = geode::MeshFactory::create_mesh< MeshTest >( impl );
    geode_unused( mesh );
}

OPENGEODE_TEST( "mesh-factory" )