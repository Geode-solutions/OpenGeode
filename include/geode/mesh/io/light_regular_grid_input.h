/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <fstream>

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geometry/point.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( LightRegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an LightRegularGrid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    LightRegularGrid< dimension > load_light_regular_grid(
        absl::string_view filename );

    template < index_t dimension >
    class LightRegularGridInput : public Input< LightRegularGrid< dimension > >
    {
    public:
        using Base = Input< LightRegularGrid< dimension > >;
        using Base::InputData;
        using Base::MissingFiles;

        LightRegularGridInput( absl::string_view filename ) : Base{ filename }
        {
        }

        LightRegularGrid< dimension > read() final
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
            std::array< double, dimension > cells_length;
            for( const auto d : LRange{ dimension } )
            {
                cells_number[d] = 1;
                cells_length[d] = 1;
            }
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
    };
    ALIAS_2D_AND_3D( LightRegularGridInput );

    template < index_t dimension >
    typename LightRegularGridInput< dimension >::MissingFiles
        check_light_regular_grid_missing_files( absl::string_view filename );

    template < index_t dimension >
    bool is_light_regular_grid_loadable( absl::string_view filename );

    template < index_t dimension >
    using LightRegularGridInputFactory = Factory< std::string,
        LightRegularGridInput< dimension >,
        absl::string_view >;
    ALIAS_2D_AND_3D( LightRegularGridInputFactory );
} // namespace geode
