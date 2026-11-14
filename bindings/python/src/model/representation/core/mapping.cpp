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

#include "../../model_mapping.hpp"

#include "../../../basic/mapping.hpp"

namespace geode
{
    void define_copy_mapping( pybind11::module& module )
    {
        define_model_mapping< BijectiveMapping< uuid > >( module,
            "ModelCopyMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a source model and the uuids of their copies",
            "BijectiveMappinguuid" );
    }
    void define_generic_mapping( pybind11::module& module )
    {
        define_model_mapping< GenericMapping< uuid > >( module,
            "ModelGenericMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a model before and after an operation, a component "
            "possibly corresponding to several others",
            "GenericMappinguuid" );
    }
} // namespace geode
