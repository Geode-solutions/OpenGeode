/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading time series onto an existing
     * BoundaryRepresentation.
     * Each series is stored on the Block meshes (vertex and polyhedron
     * attribute managers) as time step attributes (see
     * AttributeManager::create_time_step_attribute).
     * The adequate loader is called depending on the filename extension.
     * @param[in] brep BRep receiving the time series.
     * @param[in] filename Path to the file to load.
     */
    void opengeode_model_api load_brep_time_series(
        BRep& brep, std::string_view filename );

    class opengeode_model_api BRepTimeSeriesInput : public IOFile
    {
    public:
        [[nodiscard]] virtual AdditionalFiles additional_files() const = 0;

        [[nodiscard]] virtual Percentage is_loadable() const = 0;

        virtual void read( BRep& brep ) = 0;

    protected:
        explicit BRepTimeSeriesInput( std::string_view filename )
            : IOFile{ filename }
        {
        }
    };

    [[nodiscard]] AdditionalFiles opengeode_model_api
        brep_time_series_additional_files( std::string_view filename );

    [[nodiscard]] Percentage opengeode_model_api is_brep_time_series_loadable(
        std::string_view filename );

    using BRepTimeSeriesInputFactory =
        Factory< std::string, BRepTimeSeriesInput, std::string_view >;
} // namespace geode
