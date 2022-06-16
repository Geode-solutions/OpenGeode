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

#include <geode/model/helpers/simplicial_section_creator.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/model/helpers/private/simplicial_model_creator.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    class SimplicialSectionCreator::Impl
        : public detail::SimplicialModelCreator< Section, SectionBuilder, 2 >
    {
    public:
        Impl( Section& section, std::vector< Point2D > unique_points )
            : detail::SimplicialModelCreator< Section, SectionBuilder, 2 >(
                section, std::move( unique_points ) )
        {
        }

        std::vector< uuid > create_model_boundaries(
            absl::Span< const uuid > lines,
            absl::Span< const BoundaryDefinition > definitions )
        {
            std::vector< geode::uuid > boundaries;
            boundaries.reserve( definitions.size() );
            for( const auto& definition : definitions )
            {
                const auto& bonudary_id = builder().add_model_boundary();
                const auto& boundary = model().model_boundary( bonudary_id );
                boundaries.push_back( bonudary_id );
                for( const auto line : definition.boundaries )
                {
                    builder().add_line_in_model_boundary(
                        model().line( lines[line] ), boundary );
                }
            }
            return boundaries;
        }
    };

    SimplicialSectionCreator::SimplicialSectionCreator(
        Section& section, std::vector< Point2D > unique_points )
        : impl_{ section, std::move( unique_points ) }
    {
    }

    SimplicialSectionCreator::~SimplicialSectionCreator() {}

    std::vector< uuid > SimplicialSectionCreator::create_corners(
        absl::Span< const CornerDefinition > definitions )
    {
        return impl_->create_corners( definitions );
    }

    std::vector< uuid > SimplicialSectionCreator::create_lines(
        absl::Span< const uuid > corners,
        absl::Span< const LineDefinition > definitions )
    {
        return impl_->create_lines( corners, definitions );
    }

    std::vector< uuid > SimplicialSectionCreator::create_surfaces(
        absl::Span< const uuid > lines,
        absl::Span< const SurfaceDefinition > definitions )
    {
        return impl_->create_surfaces( lines, definitions );
    }

    std::vector< uuid > SimplicialSectionCreator::create_surfaces(
        absl::Span< const uuid > corners,
        absl::Span< const uuid > lines,
        absl::Span< const SurfaceDefinition > definitions )
    {
        return impl_->create_surfaces( corners, lines, definitions );
    }

    std::vector< uuid > SimplicialSectionCreator::create_model_boundaries(
        absl::Span< const uuid > lines,
        absl::Span< const BoundaryDefinition > definitions )
    {
        return impl_->create_model_boundaries( lines, definitions );
    }
} // namespace geode