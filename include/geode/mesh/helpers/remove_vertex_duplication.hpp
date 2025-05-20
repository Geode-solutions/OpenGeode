/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSetBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurveBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );
    ALIAS_3D( SolidMesh );
    ALIAS_3D( SolidMeshBuilder );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    void remove_vertex_duplication( const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder );

    template < index_t dimension >
    void remove_vertex_duplication( const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder );

    template < index_t dimension >
    void remove_vertex_duplication( const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder );

    void remove_vertex_duplication(
        const SolidMesh3D& mesh, SolidMeshBuilder3D& builder );
} // namespace geode
