/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/model_boundary.h>

#define PYTHON_MODEL_BOUNDARIES( dimension )                                   \
    const auto name##dimension =                                               \
        "ModelBoundaries" + std::to_string( dimension ) + "D";                 \
    pybind11::class_< ModelBoundaries##dimension##D >(                         \
        module, name##dimension.c_str() )                                      \
        .def( "nb_model_boundaries",                                           \
            &ModelBoundaries##dimension##D::nb_model_boundaries )              \
        .def( "model_boundary",                                                \
            &ModelBoundaries##dimension##D::model_boundary,                    \
            pybind11::return_value_policy::reference )                         \
        .def(                                                                  \
            "model_boundaries",                                                \
            []( const ModelBoundaries##dimension##D& self ) {                  \
                std::vector< const ModelBoundary##dimension##D* > components;  \
                for( const auto& component : self.model_boundaries() )         \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference )

namespace geode
{
    void define_model_boundaries( pybind11::module& module )
    {
        PYTHON_MODEL_BOUNDARIES( 2 );
        PYTHON_MODEL_BOUNDARIES( 3 );
    }
} // namespace geode
