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

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/model/common.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a BoundaryRepresentation.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] brep BRep to save.
     * @param[in] filename Path to the file where save the brep.
     */
    void opengeode_model_api save_brep(
        const BRep& brep, absl::string_view filename );

    class BRepOutput : public Output< BRep >
    {
    protected:
        BRepOutput( absl::string_view filename ) : Output< BRep >
        {
            filename
        }
        {
        }
    };

    bool opengeode_model_api is_brep_savable(
        const BRep& brep, absl::string_view filename );

    using BRepOutputFactory =
        Factory< std::string, BRepOutput, absl::string_view >;
} // namespace geode
