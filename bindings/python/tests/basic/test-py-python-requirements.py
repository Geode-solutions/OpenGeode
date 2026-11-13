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

"""The configuration provides the runtime requirements of OpenGeode to the
downstream projects, next to its CMake files, and gives pip the requirements of
the Python environments without any geode package (they come from the local
build chain). Arguments: the source requirements.in, the copy next to the
build-tree CMake files, the merged requirements file and the geode names file."""

import sys


def read_lines(path):
    with open(path) as file:
        return [line.strip() for line in file if line.strip()]


if __name__ == "__main__":
    source, provided, merged, geode = sys.argv[1:5]
    if read_lines(provided) != read_lines(source):
        raise ValueError("[Test] The runtime requirements are not provided as they are")
    merged_lines = read_lines(merged)
    if not merged_lines or "numpy" not in merged_lines[0]:
        raise ValueError("[Test] numpy should be required: %s" % merged_lines)
    if any("geode" in line.lower() for line in merged_lines):
        raise ValueError("[Test] No geode package should be required: %s" % merged_lines)
    if read_lines(geode):
        raise ValueError("[Test] OpenGeode requires no geode package")
