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

#include <geode/basic/input.hpp>

#define PYTHON_INPUT_CLASS( type, name )                                       \
    pybind11::class_< Input< type >::AdditionalFile >(                         \
        module, absl::StrCat( "AdditionalFile", name ).c_str() )               \
        .def_readwrite( "filename", &Input< type >::AdditionalFile::filename ) \
        .def_readwrite(                                                        \
            "is_missing", &Input< type >::AdditionalFile::is_missing );        \
    pybind11::class_< Input< type >::AdditionalFiles >(                        \
        module, absl::StrCat( "AdditionalFiles", name ).c_str() )              \
        .def( "has_additional_files",                                          \
            &Input< type >::AdditionalFiles::has_additional_files )            \
        .def_readwrite( "optional_files",                                      \
            &Input< type >::AdditionalFiles::optional_files )                  \
        .def_readwrite( "mandatory_files",                                     \
            &Input< type >::AdditionalFiles::mandatory_files )

#define PYTHON_INPUT_MESH_CLASS( type, name )                                  \
    pybind11::class_< Input< type, MeshImpl >::AdditionalFile >(               \
        module, absl::StrCat( "AdditionalFile", name ).c_str() )               \
        .def_readwrite(                                                        \
            "filename", &Input< type, MeshImpl >::AdditionalFile::filename )   \
        .def_readwrite( "is_missing",                                          \
            &Input< type, MeshImpl >::AdditionalFile::is_missing );            \
    pybind11::class_< Input< type, MeshImpl >::AdditionalFiles >(              \
        module, absl::StrCat( "AdditionalFiles", name ).c_str() )              \
        .def( "has_additional_files",                                          \
            &Input< type, MeshImpl >::AdditionalFiles::has_additional_files )  \
        .def_readwrite( "optional_files",                                      \
            &Input< type, MeshImpl >::AdditionalFiles::optional_files )        \
        .def_readwrite( "mandatory_files",                                     \
            &Input< type, MeshImpl >::AdditionalFiles::mandatory_files )
