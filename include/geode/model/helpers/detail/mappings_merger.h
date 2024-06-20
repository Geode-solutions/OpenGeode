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

#pragma once

#include <geode/model/common.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    namespace detail
    {
        ModelGenericMapping opengeode_model_api merge_mappings(
            const ModelGenericMapping& mappings1,
            const ModelCopyMapping& mappings2 );

        ModelGenericMapping opengeode_model_api merge_mappings(
            const ModelGenericMapping& mappings1,
            const ModelGenericMapping& mappings2 );

        ModelGenericMapping opengeode_model_api copy_to_generic_mappings(
            const ModelCopyMapping& mappings2 );

        ModelMappings opengeode_model_api merge_mappings(
            const ModelMappings& mappings1, const ModelMappings& mappings2 );
    } // namespace detail
} // namespace geode
