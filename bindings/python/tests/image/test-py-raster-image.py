# -*- coding: utf-8 -*-
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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)


import opengeode_py_basic as basic
import opengeode_py_image as image


def test_raster(raster):
    if raster.nb_cells() != 100:
        raise ValueError("[Test] Wrong number of cells")
    if raster.nb_cells_in_direction(0) != 10:
        raise ValueError("[Test] Wrong number of cells in direction 0")
    if raster.nb_cells_in_direction(1) != 10:
        raise ValueError("[Test] Wrong number of cells in direction 1")
    for i in range(raster.nb_cells()):
        color = image.RGBColor(i, i, i)
        if raster.color(i) != color:
            raise ValueError("[Test] Wrong color for ", i)


if __name__ == '__main__':
    image.OpenGeodeImageLibrary.initialize()
    raster = image.RasterImage2D([10, 10])
    for i in range(raster.nb_cells()):
        color = image.RGBColor(i, i, i)
        raster.set_color(i, color)
    test_raster(raster)
    test_raster(raster.clone())
    image.save_raster_image2D(raster, "test.og_img2d")
    reload = image.load_raster_image2D("test.og_img2d")
    test_raster(reload)
