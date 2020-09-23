# Copyright (c) 2019 - 2020 Geode-solutions
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

message(STATUS "Install ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Binary ${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE SO_LIBRARIES "${CMAKE_INSTALL_PREFIX}/*.so")
file(COPY ${SO_LIBRARIES} DESTINATION "${CMAKE_BINARY_DIR}/wheel/opengeode")

file(GLOB_RECURSE DLL_LIBRARIES "${CMAKE_INSTALL_PREFIX}/*.dll")
file(COPY ${DLL_LIBRARIES} DESTINATION "${CMAKE_BINARY_DIR}/wheel/opengeode")

file(GLOB_RECURSE PYD_LIBRARIES "${CMAKE_INSTALL_PREFIX}/*.pyd")
file(COPY ${PYD_LIBRARIES} DESTINATION "${CMAKE_BINARY_DIR}/wheel/opengeode")

file(GLOB_RECURSE DYLIB_LIBRARIES "${CMAKE_INSTALL_PREFIX}/*.dylib")
file(COPY ${DYLIB_LIBRARIES} DESTINATION "${CMAKE_BINARY_DIR}/wheel/opengeode")
