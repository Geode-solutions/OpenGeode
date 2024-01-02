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

#include <geode/model/mixin/builder/model_boundaries_builder.h>

#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/model_boundary.h>

namespace geode
{
    template < index_t dimension >
    const uuid& ModelBoundariesBuilder< dimension >::create_model_boundary()
    {
        return model_boundaries_.create_model_boundary();
    }

    template < index_t dimension >
    void ModelBoundariesBuilder< dimension >::create_model_boundary(
        uuid model_boundary_id )
    {
        model_boundaries_.create_model_boundary(
            std::move( model_boundary_id ) );
    }

    template < index_t dimension >
    void ModelBoundariesBuilder< dimension >::delete_model_boundary(
        const ModelBoundary< dimension >& boundary )
    {
        model_boundaries_.delete_model_boundary( boundary );
    }

    template < index_t dimension >
    void ModelBoundariesBuilder< dimension >::load_model_boundaries(
        absl::string_view directory )
    {
        return model_boundaries_.load_model_boundaries( directory );
    }

    template < index_t dimension >
    void ModelBoundariesBuilder< dimension >::set_model_boundary_name(
        const uuid& id, absl::string_view name )
    {
        model_boundaries_.modifiable_model_boundary( id )
            .set_model_boundary_name( name, {} );
    }

    template class opengeode_model_api ModelBoundariesBuilder< 2 >;
    template class opengeode_model_api ModelBoundariesBuilder< 3 >;
} // namespace geode
