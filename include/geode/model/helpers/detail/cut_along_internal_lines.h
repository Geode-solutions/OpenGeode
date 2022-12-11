/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/model/common.h>

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
        class CutAlongInternalLines
        {
        public:
            CutAlongInternalLines( Model& model );
            CutAlongInternalLines(
                const Model& model, typename Model::Builder& builder );
            ~CutAlongInternalLines();

            /* Cuts the surfaces along internal lines, and returns pairs of
             * component mesh vertices where the surfaces vertices were split
             * (first the initial cmv of the vertices, second the cmv of the
             * newly created vertex)
             */
            std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
                cut_all_surfaces();

            /* Cuts the surface along internal lines, and returns pairs of
             * component mesh vertices where the surface vertices were split
             * (first the initial id of the vertices, second the id of the newly
             * created vertex)
             */
            std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
                cut_surface( const Surface< Model::dimension >& surface );

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
    } // namespace detail
} // namespace geode