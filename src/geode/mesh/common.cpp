/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/common.hpp>

#include <geode/geometry/common.hpp>

#include <geode/image/common.hpp>

#include <geode/mesh/builder/geode/register_builder.hpp>
#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/geode/register_mesh.hpp>
#include <geode/mesh/io/geode/register_input.hpp>
#include <geode/mesh/io/geode/register_output.hpp>

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( OpenGeodeMesh )
    {
        OpenGeodeGeometryLibrary::initialize();
        OpenGeodeImageLibrary::initialize();
        register_geode_mesh();
        register_geode_builder();
        register_geode_mesh_input();
        register_geode_mesh_output();
        BitseryExtensions::register_functions( register_mesh_serialize_pcontext,
            register_mesh_deserialize_pcontext );
    }
} // namespace geode
