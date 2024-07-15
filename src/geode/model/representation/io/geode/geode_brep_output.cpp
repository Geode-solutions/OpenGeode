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

#include <geode/model/representation/io/geode/geode_brep_output.h>

#include <filesystem>
#include <string>
#include <vector>

#include <async++.h>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/core/brep.h>

namespace geode
{
    void OpenGeodeBRepOutput::archive_brep_files(
        const ZipFile& zip_writer ) const
    {
        for( const auto& file :
            std::filesystem::directory_iterator( zip_writer.directory() ) )
        {
            zip_writer.archive_file( file.path().string() );
        }
    }

    void OpenGeodeBRepOutput::save_brep_files(
        const BRep& brep, std::string_view directory ) const
    {
        async::parallel_invoke(
            [&directory, &brep] {
                brep.save_identifier( directory );
            },
            [&directory, &brep] {
                brep.save_relationships( directory );
            },
            [&directory, &brep] {
                brep.save_unique_vertices( directory );
            },
            [&directory, &brep] {
                brep.save_corners( directory );
                brep.save_lines( directory );
                brep.save_surfaces( directory );
                brep.save_blocks( directory );
            },
            [&directory, &brep] {
                brep.save_model_boundaries( directory );
                brep.save_corner_collections( directory );
                brep.save_line_collections( directory );
                brep.save_surface_collections( directory );
                brep.save_block_collections( directory );
            } );
    }

    std::vector< std::string > OpenGeodeBRepOutput::write(
        const BRep& brep ) const
    {
        const ZipFile zip_writer{ filename(), uuid{}.string() };
        save_brep_files( brep, zip_writer.directory() );
        archive_brep_files( zip_writer );
        return { to_string( filename() ) };
    }
} // namespace geode
