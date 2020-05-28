/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/logger.h>

#include <geode/mesh/core/mesh_factory.h>

#include <geode/tests/common.h>

class MeshTest : public geode::VertexSet
{
public:
    MeshTest() = default;

    absl::string_view native_extension() const override
    {
        static auto ext = "ext";
        return ext;
    }

    geode::MeshType type_name() const override
    {
        return geode::MeshType{ "type" };
    }

    geode::MeshKind kind_name() const override
    {
        return geode::MeshKind{ "kind" };
    }
};

void test()
{
    geode::MeshType type{ "type" };
    geode::MeshKind kind{ "kind" };
    geode::MeshFactory::register_mesh< MeshTest >( kind, type );

    OPENGEODE_EXCEPTION(
        geode::MeshFactory::kind( type ) == kind, "Wrong kind" );

    geode::MeshFactory::create_mesh< MeshTest >( type );
}

OPENGEODE_TEST( "mesh-factory" )