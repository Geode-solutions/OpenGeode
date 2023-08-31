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

#include <geode/model/common.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    class BRep;
    class Section;
} // namespace geode

namespace geode
{

    std::tuple< Section, ModelCopyMapping > opengeode_model_api
        convert_brep_into_section( const BRep& brep, index_t axis_to_remove );

    std::tuple< BRep, ModelCopyMapping >
        opengeode_model_api convert_section_into_brep( const Section& section,
            index_t axis_to_add,
            double axis_coordinate );

    struct opengeode_model_api SectionExtruderOptions
    {
        SectionExtruderOptions() = default;
        index_t axis_to_extrude{ NO_ID };
        double min_coordinate{ 0. };
        double max_coordinate{ 0. };
    };

    BRep opengeode_model_api extrude_section_to_brep(
        const Section& section, const SectionExtruderOptions& coordinate );
} // namespace geode