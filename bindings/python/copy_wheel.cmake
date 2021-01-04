# Copyright (c) 2019 - 2021 Geode-solutions
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

file(GLOB_RECURSE SO_LIBRARIES "${SOURCE_DIR}/*.so")
file(COPY ${SO_LIBRARIES} DESTINATION "${DEST_DIR}")

file(GLOB_RECURSE DLL_LIBRARIES "${SOURCE_DIR}/*.dll")
file(COPY ${DLL_LIBRARIES} DESTINATION "${DEST_DIR}")

file(GLOB_RECURSE PYD_LIBRARIES "${SOURCE_DIR}/*.pyd")
file(COPY ${PYD_LIBRARIES} DESTINATION "${DEST_DIR}")

file(GLOB_RECURSE DYLIB_LIBRARIES "${SOURCE_DIR}/*.dylib")
file(COPY ${DYLIB_LIBRARIES} DESTINATION "${DEST_DIR}")
