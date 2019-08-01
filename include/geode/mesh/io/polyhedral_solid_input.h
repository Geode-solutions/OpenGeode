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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an PolyhedralSolid.
     * The adequate loader is called depending on the filename extension.
     * @param[out] polyhedral_solid Loaded PolyhedralSolid.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    void load_polyhedral_solid( PolyhedralSolid< dimension >& polyhedral_solid,
        const std::string& filename );

    template < index_t dimension >
    class PolyhedralSolidInput : public VertexSetInput
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        OPENGEODE_DISABLE_COPY_AND_MOVE( PolyhedralSolidInput );

    public:
        virtual ~PolyhedralSolidInput() = default;

    protected:
        PolyhedralSolidInput( PolyhedralSolid< dimension >& polyhedral_solid,
            std::string filename );

        PolyhedralSolid< dimension >& polyhedral_solid()
        {
            return polyhedral_solid_;
        }

    private:
        PolyhedralSolid< dimension >& polyhedral_solid_;
    };

    template < index_t dimension >
    using PolyhedralSolidInputFactory = Factory< std::string,
        PolyhedralSolidInput< dimension >,
        PolyhedralSolid< dimension >&,
        std::string >;
    ALIAS_3D( PolyhedralSolidInputFactory );
} // namespace geode
