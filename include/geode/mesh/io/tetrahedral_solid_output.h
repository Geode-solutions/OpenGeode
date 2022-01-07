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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a TetrahedralSolid.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] tetrahedral_solid TetrahedralSolid to save.
     * @param[in] filename Path to the file where save the TetrahedralSolid.
     */
    template < index_t dimension >
    void save_tetrahedral_solid(
        const TetrahedralSolid< dimension >& tetrahedral_solid,
        absl::string_view filename );

    template < index_t dimension >
    class TetrahedralSolidOutput : public VertexSetOutput
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        OPENGEODE_DISABLE_COPY_AND_MOVE( TetrahedralSolidOutput );

    protected:
        TetrahedralSolidOutput(
            const TetrahedralSolid< dimension >& tetrahedral_solid,
            absl::string_view filename );

        const TetrahedralSolid< dimension >& tetrahedral_solid() const
        {
            return tetrahedral_solid_;
        }

    private:
        const TetrahedralSolid< dimension >& tetrahedral_solid_;
    };

    template < index_t dimension >
    using TetrahedralSolidOutputFactory = Factory< std::string,
        TetrahedralSolidOutput< dimension >,
        const TetrahedralSolid< dimension >&,
        absl::string_view >;
    ALIAS_3D( TetrahedralSolidOutputFactory );
} // namespace geode
