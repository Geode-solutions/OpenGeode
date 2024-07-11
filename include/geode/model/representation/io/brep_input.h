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

#include <string_view>

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/model/common.h>

namespace geode
{
    class BRep;
    class BRepBuilder;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a BoundaryRepresentation.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     * @return Loaded BRep.
     */
    BRep opengeode_model_api load_brep( std::string_view filename );

    class BRepInput : public Input< BRep >
    {
    public:
        using Base = Input< BRep >;
        using typename Base::InputData;
        using typename Base::MissingFiles;

    protected:
        explicit BRepInput( std::string_view filename ) : Base{ filename } {}
    };

    typename BRepInput::MissingFiles opengeode_model_api
        check_brep_missing_files( std::string_view filename );

    bool opengeode_model_api is_brep_loadable( std::string_view filename );

    using BRepInputFactory =
        Factory< std::string, BRepInput, std::string_view >;
} // namespace geode
