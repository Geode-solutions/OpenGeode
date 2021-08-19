/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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
#include <geode/model/helpers/simplicial_creator_definitions.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_2D( Point );
    class Section;
    struct uuid;
} // namespace geode

namespace geode
{
    class opengeode_model_api SimplicialSectionCreator
    {
    public:
        SimplicialSectionCreator(
            Section& brep, std::vector< Point2D > unique_points );

        ~SimplicialSectionCreator();

        std::vector< uuid > create_corners(
            absl::Span< const CornerDefinition > definitions );

        std::vector< uuid > create_lines( absl::Span< const uuid > corners,
            absl::Span< const LineDefinition > definitions );

        std::vector< uuid > create_surfaces( absl::Span< const uuid > lines,
            absl::Span< const SurfaceDefinition > definitions );

        std::vector< uuid > create_model_boundaries(
            absl::Span< const uuid > lines,
            absl::Span< const BoundaryDefinition > definitions );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode