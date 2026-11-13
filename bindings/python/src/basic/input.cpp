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

#include "../common.hpp"

#include <pybind11/stl_bind.h>

#include <geode/basic/input.hpp>

PYBIND11_MAKE_OPAQUE( std::vector< geode::AdditionalFile > )

namespace geode
{
    void define_input( pybind11::module& module )
    {
        pybind11::class_< AdditionalFile > file( module, "AdditionalFile",
            "File used together with a main file when loading it" );
        file.def( pybind11::init< std::string, bool >(),
            pybind11::arg( "filename" ), pybind11::arg( "is_missing" ),
            "Describe an additional file by its name and whether it is "
            "missing on disk" );
        def_readwrite_named(
            file, "filename", &AdditionalFile::filename, "Path of the file" );
        def_readwrite_named( file, "is_missing", &AdditionalFile::is_missing,
            "True if the file does not exist" );
        // The file lists are bound as opaque containers (not converted to
        // Python lists) so that in-place modifications, like
        // files.optional_files.append(...), act on the actual object.
        pybind11::bind_vector< std::vector< AdditionalFile > >( module,
            "AdditionalFileList",
            "List of AdditionalFile (modified in place by append, etc.)" );
        pybind11::implicitly_convertible< pybind11::list,
            std::vector< AdditionalFile > >();
        pybind11::class_< AdditionalFiles > files( module, "AdditionalFiles",
            "Optional and mandatory additional files of a file format" );
        files.def( pybind11::init<>() )
            .def( "has_additional_files",
                &AdditionalFiles::has_additional_files,
                "Return True if at least one additional file, optional or "
                "mandatory, is missing" );
        def_readwrite_named( files, "optional_files",
            &AdditionalFiles::optional_files,
            "Additional files which can be absent" );
        def_readwrite_named( files, "mandatory_files",
            &AdditionalFiles::mandatory_files,
            "Additional files required to load the main file" );
    }
} // namespace geode
