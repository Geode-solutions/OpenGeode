/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/io/vertex_set_input.h>

#include <geode/model/common.h>
#include <geode/model/representation/builder/section_builder.h>

namespace geode
{
    class Section;
    class SectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a Section.
     * The adequate loader is called depending on the filename extension.
     * @param[out] section Loaded Section.
     * @param[in] filename Path to the file to load.
     */
    void opengeode_model_api load_section(
        Section& section, absl::string_view filename );

    class opengeode_model_api SectionInput : public Input
    {
    protected:
        SectionInput( Section& section, absl::string_view filename );

        Section& section()
        {
            return section_;
        }

    private:
        Section& section_;
    };

    using SectionInputFactory =
        Factory< std::string, SectionInput, Section&, absl::string_view >;
} // namespace geode
