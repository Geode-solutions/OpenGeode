# Copyright (c) 2019 - 2023 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

add_geode_library(
    NAME image
    FOLDER "geode/image"
    SOURCES
        "common.cpp"
        "core/bitsery_archive.cpp"
        "core/raster_image.cpp"
        "io/raster_image_input.cpp"
        "io/raster_image_output.cpp"
    PUBLIC_HEADERS
        "common.h"
        "core/bitsery_archive.h"
        "core/greyscale_color.h"
        "core/raster_image.h"
        "core/rgb_color.h"
        "io/raster_image_input.h"
        "io/raster_image_output.h"
        "io/geode/geode_bitsery_raster_input.h"
        "io/geode/geode_bitsery_raster_output.h"
    PUBLIC_DEPENDENCIES
        ${PROJECT_NAME}::basic
)
