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

#pragma once

#include <string_view>

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( LightRegularGrid );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodeLightRegularGridInput
        : public LightRegularGridInput< dimension >
    {
    public:
        explicit OpenGeodeLightRegularGridInput( std::string_view filename )
            : LightRegularGridInput< dimension >{ filename }
        {
        }

    protected:
        [[nodiscard]] LightRegularGrid< dimension > read() override
        {
            std::ifstream file{ to_string( this->filename() ),
                std::ifstream::binary };
            OPENGEODE_EXCEPTION( file,
                "[LightRegularGridInput] Failed to open file: ",
                to_string( this->filename() ) );
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            Point< dimension > origin;
            std::array< index_t, dimension > cells_number;
            cells_number.fill( 1 );
            std::array< double, dimension > cells_length;
            cells_length.fill( 1 );
            LightRegularGrid< dimension > grid{ origin, cells_number,
                cells_length };
            archive.object( grid );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Bitsery::read] Error while reading file: ",
                this->filename() );
            return grid;
        }

        typename LightRegularGridInput< dimension >::AdditionalFiles
            additional_files() const override
        {
            return {};
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeLightRegularGridInput );
} // namespace geode
