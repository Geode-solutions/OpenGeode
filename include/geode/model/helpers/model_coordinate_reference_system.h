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

#include <string>
#include <utility>

#include <absl/container/fixed_array.h>

#include <geode/mesh/core/coordinate_reference_system.h>

#include <geode/model/common.h>

namespace geode
{
    class BRep;
    class Section;
} // namespace geode

namespace geode
{
    absl::FixedArray< std::pair< CRSType, std::string > > opengeode_model_api
        brep_coordinate_reference_systems( const BRep& brep );

    absl::FixedArray< std::pair< CRSType, std::string > > opengeode_model_api
        section_coordinate_reference_systems( const Section& section );

    absl::FixedArray< std::pair< CRSType, std::string > > opengeode_model_api
        brep_active_coordinate_reference_systems( const BRep& brep );

    absl::FixedArray< std::pair< CRSType, std::string > > opengeode_model_api
        section_active_coordinate_reference_systems( const Section& section );
} // namespace geode