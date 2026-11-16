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

from opengeode_py_model import *
OpenGeodeModelLibrary.initialize()

# The registry of the generic functions (load, save, builder...) is in basic:
# a sibling module in the wheel package, a top-level module in the build tree
try:
    from .basic import register_type as _register_type
except ImportError:
    from basic import register_type as _register_type


_register_type(
    BRep,
    load=load_brep,
    save=save_brep,
    is_loadable=is_brep_loadable,
    is_saveable=is_brep_saveable,
    input_extensions=BRepInputFactory.list_creators,
    output_extensions=BRepOutputFactory.list_creators,
    additional_files=brep_additional_files,
    builder=BRepBuilder,
)
_register_type(
    Section,
    load=load_section,
    save=save_section,
    is_loadable=is_section_loadable,
    is_saveable=is_section_saveable,
    input_extensions=SectionInputFactory.list_creators,
    output_extensions=SectionOutputFactory.list_creators,
    additional_files=section_additional_files,
    builder=SectionBuilder,
)


def _load_external_time_series(filename: str) -> "list[ExternalTimeSeries]":
    return [load_external_time_series(filename)]


def _save_external_time_series(time_series: ExternalTimeSeries, filename: str) -> "list[str]":
    # The native writer writes only the given file
    save_external_time_series(time_series, filename)
    return [filename]


def _external_time_series_extensions() -> "list[str]":
    return [ExternalTimeSeries.native_extension_static()]


_register_type(
    ExternalTimeSeries,
    load=_load_external_time_series,
    save=_save_external_time_series,
    input_extensions=_external_time_series_extensions,
    output_extensions=_external_time_series_extensions,
)
