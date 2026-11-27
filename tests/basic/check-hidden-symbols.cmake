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

# Fails if LIBRARY exports a zlib or minizip-ng symbol (they must stay private)
execute_process(
    COMMAND "${NM}" -D --defined-only "${LIBRARY}"
    OUTPUT_VARIABLE symbols
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "[Test] nm failed on ${LIBRARY}")
endif()
string(REGEX MATCHALL
    "[ \t](crc32|crc32_z|inflate|inflateInit2_|deflate|deflateInit2_|mz_[A-Za-z0-9_]+)\n"
    exported "${symbols}\n"
)
if(exported)
    string(REGEX REPLACE "[ \t\n]" "" exported "${exported}")
    message(FATAL_ERROR "[Test] ${LIBRARY} exports zlib/minizip-ng symbols: ${exported}")
endif()
message(STATUS "[Test] no zlib/minizip-ng symbol exported by ${LIBRARY}")
