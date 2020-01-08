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

#include <geode/mesh/io/output.h>

#include <geode/model/common.h>

namespace geode
{
    class Section;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a Section.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] section Section to save.
     * @param[in] filename Path to the file where save the section.
     */
    void opengeode_model_api save_section(
        const Section& section, const std::string& filename );

    class opengeode_model_api SectionOutput : public Output
    {
    protected:
        SectionOutput( const Section& section, std::string filename );

        const Section& section() const
        {
            return section_;
        }

    private:
        const Section& section_;
    };

    using SectionOutputFactory =
        Factory< std::string, SectionOutput, const Section&, std::string >;
} // namespace geode
