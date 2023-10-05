/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <absl/strings/string_view.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateSystem );
    ALIAS_2D( CoordinateSystem );
    class BRep;
    class Section;
    class BRepBuilder;
    class SectionBuilder;
} // namespace geode

namespace geode
{
    void opengeode_model_api create_brep_coordinate_system( const BRep& model,
        BRepBuilder& builder,
        absl::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output );

    void opengeode_model_api create_section_coordinate_system(
        const Section& model,
        SectionBuilder& builder,
        absl::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output );

    void opengeode_model_api set_brep_active_coordinate_system(
        const BRep& model,
        BRepBuilder& builder,
        absl::string_view coordinate_system_name );

    void opengeode_model_api set_section_active_coordinate_system(
        const Section& model,
        SectionBuilder& builder,
        absl::string_view coordinate_system_name );
} // namespace geode