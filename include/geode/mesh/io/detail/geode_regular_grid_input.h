/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/regular_grid.h>
#include <geode/mesh/io/regular_grid_input.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeRegularGridInput : public RegularGridInput< dimension >
    {
    public:
        OpenGeodeRegularGridInput( absl::string_view filename )
            : RegularGridInput< dimension >( filename )
        {
            auto mesh = bitsery::Access::create< RegularGrid< dimension > >();
            mesh_.reset( new RegularGrid< dimension >{ std::move( mesh ) } );
        }

        void read() final
        {
            std::ifstream file{ this->filename().data(),
                std::ifstream::binary };
            TContext context{};
            register_basic_deserialize_pcontext( std::get< 0 >( context ) );
            register_geometry_deserialize_pcontext( std::get< 0 >( context ) );
            register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *mesh_ );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Bitsery::read] Error while reading file: ",
                this->filename() );
        }

        std::unique_ptr< RegularGrid< dimension > > regular_grid() override
        {
            return std::move( mesh_ );
        }

    private:
        std::unique_ptr< RegularGrid< dimension > > mesh_;
    };
    ALIAS_2D_AND_3D( OpenGeodeRegularGridInput );
} // namespace geode
