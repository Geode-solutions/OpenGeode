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

#pragma once

#include <memory>

#include <geode/georepresentation/common.h>

#include <geode/mesh/core/mesh_type.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    FORWARD_DECLARATION_DIMENSION_CLASS( Lines );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurveBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class LinesBuilder
    {
    public:
        void load_lines( const std::string& directory );

        /*!
         * Get a pointer to the builder of a Line mesh
         * @param[in] id Unique index of the Line
         */
        std::unique_ptr< EdgedCurveBuilder< dimension > > line_mesh_builder(
            const uuid& id );

    protected:
        LinesBuilder( Lines< dimension >& lines ) : lines_( lines ) {}

        const uuid& create_line();

        const uuid& create_line( const MeshType& type );

        void delete_line( const Line< dimension >& line );

    private:
        Lines< dimension >& lines_;
    };
} // namespace geode
