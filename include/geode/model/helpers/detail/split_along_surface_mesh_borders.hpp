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

#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    struct ComponentMeshVertex;
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < typename Model >
        class SplitAlongSurfaceMeshBorders
        {
        public:
            explicit SplitAlongSurfaceMeshBorders( Model& model );
            SplitAlongSurfaceMeshBorders(
                const Model& model, typename Model::Builder& builder );
            ~SplitAlongSurfaceMeshBorders();

            /* Splits the surfaces along internal lines, and returns pairs of
             * component mesh vertices where the surfaces vertices were split
             * (first the initial cmv of the vertices, second the cmv of the
             * newly created vertex)
             */
            std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
                split_all_surfaces();

            /* Splits the surface along internal lines, and returns pairs of
             * component mesh vertices where the surface vertices were split
             * (first the initial id of the vertices, second the id of the newly
             * created vertex)
             */
            std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
                split_surface( const Surface< Model::dim >& surface );

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
    } // namespace detail
} // namespace geode