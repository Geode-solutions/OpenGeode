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

#include <geode/model/common.hpp>
#include <geode/model/representation/core/mapping.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    ALIAS_2D_AND_3D( PointSet );
    ALIAS_2D_AND_3D( EdgedCurve );
    ALIAS_2D_AND_3D( SurfaceMesh );
    ALIAS_2D_AND_3D( SolidMesh );

    class Section;
    class SectionBuilder;
    class BRep;
    class BRepBuilder;
} // namespace geode

namespace geode
{
    namespace detail
    {
        void opengeode_model_api transfer_brep_metadata( const BRep& old_brep,
            BRepBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping );

        void opengeode_model_api transfer_section_metadata(
            const Section& old_section,
            SectionBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping );

        template < typename ModelBuilder >
        void transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper<
                const PointSet< ModelBuilder::dim > > > pointsets,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template < typename ModelBuilder >
        void transfer_curves_metadata(
            absl::Span< const std::reference_wrapper<
                const EdgedCurve< ModelBuilder::dim > > > curves,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template < typename ModelBuilder >
        void transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper<
                const SurfaceMesh< ModelBuilder::dim > > > surfaces,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        void opengeode_model_api transfer_solids_metadata(
            absl::Span< const std::reference_wrapper< const SolidMesh3D > >
                solids,
            BRepBuilder& model_builder,
            const ModelGenericMapping& component_mapping );
    } // namespace detail
} // namespace geode
