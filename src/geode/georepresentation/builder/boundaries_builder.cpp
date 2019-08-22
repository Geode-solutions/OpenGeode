/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/georepresentation/builder/boundaries_builder.h>

#include <geode/georepresentation/core/boundary.h>
#include <geode/georepresentation/core/boundaries.h>

namespace geode
{
    template < index_t dimension >
    const uuid& BoundariesBuilder< dimension >::create_boundary()
    {
        return boundaries_.create_boundary();
    }

    template < index_t dimension >
    void BoundariesBuilder< dimension >::delete_boundary(
        const Boundary< dimension >& boundary )
    {
        boundaries_.delete_boundary( boundary );
    }

    template < index_t dimension >
    void BoundariesBuilder< dimension >::load_boundaries(
        const std::string& directory )
    {
        return boundaries_.load_boundaries( directory );
    }

    template class opengeode_georepresentation_api BoundariesBuilder< 2 >;
    template class opengeode_georepresentation_api BoundariesBuilder< 3 >;
} // namespace geode
