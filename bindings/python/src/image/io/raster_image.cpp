/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <string>

#include "../../common.hpp"

#include "../../basic/factory.hpp"
#include "../../basic/input.hpp"

#include <geode/image/core/raster_image.hpp>
#include <geode/image/io/raster_image_input.hpp>
#include <geode/image/io/raster_image_output.hpp>

#define PYTHON_RASTER_IMAGE_IO( dimension )                                    \
    const auto save##dimension =                                               \
        "save_raster_image" + std::to_string( dimension ) + "D";               \
    module.def( save##dimension.c_str(), &save_raster_image< dimension > );    \
    const auto load##dimension =                                               \
        "load_raster_image" + std::to_string( dimension ) + "D";               \
    module.def( load##dimension.c_str(), &load_raster_image< dimension > );    \
    const auto check##dimension = "check_raster_image_missing_files"           \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &check_raster_image_missing_files< dimension > );                      \
    const auto loadable##dimension =                                           \
        "is_raster_image_loadable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        loadable##dimension.c_str(), &is_raster_image_loadable< dimension > ); \
    PYTHON_INPUT_CLASS( RasterImage##dimension##D,                             \
        "RasterImage" + std::to_string( dimension ) + "D" );                   \
    const auto saveable##dimension =                                           \
        "is_raster_image_saveable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        saveable##dimension.c_str(), &is_raster_image_saveable< dimension > ); \
    PYTHON_FACTORY_CLASS( RasterImageInputFactory##dimension##D );             \
    PYTHON_FACTORY_CLASS( RasterImageOutputFactory##dimension##D )

namespace geode
{
    void define_raster_image_io( pybind11::module& module )
    {
        PYTHON_RASTER_IMAGE_IO( 2 );
        PYTHON_RASTER_IMAGE_IO( 3 );
    }
} // namespace geode
