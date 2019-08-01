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

#include <geode/georepresentation/builder/blocks_builder.h>
#include <geode/georepresentation/builder/corners_builder.h>
#include <geode/georepresentation/builder/georepresentation_builder.h>
#include <geode/georepresentation/builder/lines_builder.h>
#include <geode/georepresentation/builder/surfaces_builder.h>
#include <geode/georepresentation/common.h>
#include <geode/georepresentation/core/blocks.h>
#include <geode/georepresentation/core/corners.h>
#include <geode/georepresentation/core/georepresentation.h>
#include <geode/georepresentation/core/lines.h>
#include <geode/georepresentation/core/surfaces.h>

#include <geode/mesh/core/mesh_type.h>

namespace geode
{
    ALIAS_3D( Block );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( Surface );

    class BRep;

    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modification of a BRep
     */
    class opengeode_georepresentation_api BRepBuilder
        : public GeoRepresentationBuilder< 3,
              CornersBuilder,
              LinesBuilder,
              SurfacesBuilder,
              BlocksBuilder >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( BRepBuilder );

    public:
        BRepBuilder( BRep& brep );

        const uuid& add_corner();

        const uuid& add_corner( const MeshType& type );

        const uuid& add_line();

        const uuid& add_line( const MeshType& type );

        const uuid& add_surface();

        const uuid& add_surface( const MeshType& type );

        const uuid& add_block();

        const uuid& add_block( const MeshType& type );

        void remove_corner( const Corner3D& corner );

        void remove_line( const Line3D& line );

        void remove_surface( const Surface3D& surface );

        void remove_block( const Block3D& block );

        void add_boundary_relation(
            const Corner3D& corner, const Line3D& line );

        void add_boundary_relation(
            const Line3D& line, const Surface3D& surface );

        void add_boundary_relation(
            const Surface3D& surface, const Block3D& block );

    private:
        BRep& brep_;
    };
} // namespace geode
