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

#pragma once

#include <optional>
#include <string_view>

#include <geode/basic/detail/file_content.hpp>
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
        [[nodiscard]] std::vector< LightRegularGrid< dimension > >
            read() override
        {
            const auto content = detail::map_file_content( this->filename() );
            OpenGeodeMeshException::check_exception( content.has_value(),
                nullptr, OpenGeodeException::TYPE::data,
                "[LightRegularGridInput] Failed to open file: ",
                this->filename() );
            return read_grid( content->span() );
        }

        [[nodiscard]] std::optional<
            std::vector< LightRegularGrid< dimension > > >
            read_bytes( ByteSpan content ) override
        {
            return read_grid( content );
        }

        AdditionalFiles additional_files() const override
        {
            return {};
        }

        index_t object_priority() const override
        {
            return 0;
        }

        Percentage is_loadable() const final
        {
            return Percentage{ 1 };
        }

    private:
        [[nodiscard]] std::vector< LightRegularGrid< dimension > > read_grid(
            ByteSpan content ) const
        {
            Point< dimension > origin;
            std::array< index_t, dimension > cells_number;
            cells_number.fill( 1 );
            std::array< double, dimension > cells_length;
            cells_length.fill( 1 );
            LightRegularGrid< dimension > grid{ origin, cells_number,
                cells_length };
            bool success{ false };
            try
            {
                success = detail::deserialize_bytes( content, grid );
            }
            catch( const OpenGeodeException& )
            {
                throw;
            }
            catch( const std::exception& exception )
            {
                throw OpenGeodeMeshException{ nullptr,
                    OpenGeodeException::TYPE::data,
                    "[Bitsery::read] Error while reading file: ",
                    this->filename(), " (", exception.what(), ")" };
            }
            OpenGeodeMeshException::check_exception( success, nullptr,
                OpenGeodeException::TYPE::internal,
                "[Bitsery::read] Error while reading file: ",
                this->filename() );
            std::vector< LightRegularGrid< dimension > > grids;
            grids.emplace_back( std::move( grid ) );
            return grids;
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeLightRegularGridInput );
} // namespace geode
