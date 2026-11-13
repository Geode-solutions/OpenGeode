# Copyright (c) 2019 - 2026 Geode-solutions
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

from opengeode_py_image import *

OpenGeodeImageLibrary.initialize()

# The registry of the generic functions (load, save, builder...) is in basic:
# a sibling module in the wheel package, a top-level module in the build tree
try:
    from .basic import register_type as _register_type
except ImportError:
    from basic import register_type as _register_type


for _dimension in ("1D", "2D", "3D"):
    _module = globals()
    _register_type(
        _module["RasterImage" + _dimension],
        load=_module["load_raster_image" + _dimension],
        save=_module["save_raster_image" + _dimension],
        is_loadable=_module["is_raster_image_loadable" + _dimension],
        is_saveable=_module["is_raster_image_saveable" + _dimension],
        input_extensions=_module["RasterImageInputFactory" + _dimension].list_creators,
        output_extensions=_module["RasterImageOutputFactory" + _dimension].list_creators,
        additional_files=_module["raster_image_additional_files" + _dimension],
    )
del _dimension, _module
