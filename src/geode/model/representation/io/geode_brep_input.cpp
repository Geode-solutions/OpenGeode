/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/model/representation/io/detail/geode_brep_input.h>

#include <async++.h>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    void OpenGeodeBRepInput::load_brep_files( absl::string_view directory )
    {
        BRepBuilder builder{ brep() };
        absl::FixedArray< async::task< void > > tasks( 5 );
        index_t count{ 0 };
        tasks[count++] = async::spawn(
            [&builder, &directory] { builder.load_identifier( directory ); } );
        tasks[count++] = async::spawn( [&builder, &directory] {
            builder.load_corners( directory );
            builder.load_lines( directory );
            builder.load_surfaces( directory );
            builder.load_blocks( directory );
        } );
        tasks[count++] = async::spawn( [&builder, &directory] {
            builder.load_model_boundaries( directory );
        } );
        tasks[count++] = async::spawn( [&builder, &directory] {
            builder.load_relationships( directory );
        } );
        tasks[count++] = async::spawn( [&builder, &directory] {
            builder.load_unique_vertices( directory );
        } );
        async::when_all( tasks.begin(), tasks.end() )
            .then( [&builder, this] {
                for( const auto& corner : brep().corners() )
                {
                    builder.register_mesh_component( corner );
                }
                for( const auto& line : brep().lines() )
                {
                    builder.register_mesh_component( line );
                }
                for( const auto& surface : brep().surfaces() )
                {
                    builder.register_mesh_component( surface );
                }
                for( const auto& block : brep().blocks() )
                {
                    builder.register_mesh_component( block );
                }
            } )
            .wait();
    }

    void OpenGeodeBRepInput::read()
    {
        const UnzipFile zip_reader{ filename(), uuid{}.string() };
        zip_reader.extract_all();
        load_brep_files( zip_reader.directory() );
    }
} // namespace geode
