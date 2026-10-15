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

#include <geode/geometry/information.hpp>

#include <string>

namespace geode
{
    namespace detail
    {
        local_index_t position_to_index( POSITION position )
        {
            switch( position )
            {
                case POSITION::vertex0:
                case POSITION::edge0:
                case POSITION::facet0:
                case POSITION::edge01:
                    return 0;
                case POSITION::vertex1:
                case POSITION::edge1:
                case POSITION::facet1:
                case POSITION::edge02:
                    return 1;
                case POSITION::vertex2:
                case POSITION::edge2:
                case POSITION::facet2:
                case POSITION::edge03:
                    return 2;
                case POSITION::vertex3:
                case POSITION::facet3:
                case POSITION::edge12:
                    return 3;
                case POSITION::edge13:
                    return 4;
                case POSITION::edge23:
                    return 5;
                default:
                    return NO_LID;
            }
        }

        std::string position_to_string( POSITION position )
        {
            switch( position )
            {
                case POSITION::vertex0:
                    return "vertex0";
                case POSITION::vertex1:
                    return "vertex1";
                case POSITION::vertex2:
                    return "vertex2";
                case POSITION::vertex3:
                    return "vertex3";
                case POSITION::edge0:
                    return "edge0";
                case POSITION::edge1:
                    return "edge1";
                case POSITION::edge2:
                    return "edge2";
                case POSITION::facet0:
                    return "facet0";
                case POSITION::facet1:
                    return "facet1";
                case POSITION::facet2:
                    return "facet2";
                case POSITION::facet3:
                    return "facet3";
                case POSITION::edge01:
                    return "edge01";
                case POSITION::edge02:
                    return "edge02";
                case POSITION::edge03:
                    return "edge03";
                case POSITION::edge12:
                    return "edge12";
                case POSITION::edge13:
                    return "edge13";
                case POSITION::edge23:
                    return "edge23";
                default:
                    return "NO_LID";
            }
        }
    } // namespace detail
} // namespace geode
