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

#include <absl/strings/ascii.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/library.hpp>

namespace
{
    struct PythonExceptions
    {
        pybind11::object base;
        pybind11::object data;
        pybind11::object internal;
        pybind11::object result;
    };

    PythonExceptions& python_exceptions()
    {
        // Never destroyed on purpose: the Python interpreter may be finalized
        // before the static destructors run.
        static auto* exceptions = new PythonExceptions;
        return *exceptions;
    }

    const pybind11::object& python_class( geode::OpenGeodeException::TYPE type )
    {
        const auto& exceptions = python_exceptions();
        switch( type )
        {
            case geode::OpenGeodeException::TYPE::data:
                return exceptions.data;
            case geode::OpenGeodeException::TYPE::internal:
                return exceptions.internal;
            case geode::OpenGeodeException::TYPE::result:
                return exceptions.result;
            default:
                return exceptions.base;
        }
    }

    pybind11::object to_python( const geode::OpenGeodeException& exception )
    {
        const auto& type = python_class( exception.type() );
        pybind11::object python_exception = type( exception.what() );
        python_exception.attr( "type" ) =
            absl::AsciiStrToLower( exception.type_name() );
        python_exception.attr( "project" ) = std::string{ exception.project() };
        python_exception.attr( "library" ) = std::string{ exception.library() };
        python_exception.attr( "message" ) = std::string{ exception.what() };
        python_exception.attr( "details" ) = exception.string();
        if( exception.has_parent() )
        {
            python_exception.attr( "__cause__" ) =
                to_python( exception.parent() );
        }
        return python_exception;
    }

    void translate( std::exception_ptr pointer )
    {
        if( !pointer )
        {
            return;
        }
        try
        {
            std::rethrow_exception( pointer );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            const auto python_exception = to_python( exception );
            PyErr_SetObject( reinterpret_cast< PyObject* >(
                                 Py_TYPE( python_exception.ptr() ) ),
                python_exception.ptr() );
        }
    }
} // namespace

namespace geode
{
    void define_exception( pybind11::module& module )
    {
        auto& exceptions = python_exceptions();
        const auto make = [&module]( const char* name,
                              const pybind11::object& base, const char* doc ) {
            // Qualified by the name of the module, which is a package member
            // in the wheel (opengeode.opengeode_py_basic)
            const auto qualified =
                module.attr( "__name__" ).cast< std::string >() + "." + name;
            auto* created = PyErr_NewExceptionWithDoc(
                qualified.c_str(), doc, base.ptr(), nullptr );
            if( created == nullptr )
            {
                throw pybind11::error_already_set();
            }
            auto result = pybind11::reinterpret_steal< pybind11::object >(
                reinterpret_cast< PyObject* >( created ) );
            module.attr( name ) = result;
            return result;
        };
        exceptions.base = make( "OpenGeodeError",
            pybind11::reinterpret_borrow< pybind11::object >(
                PyExc_RuntimeError ),
            "Base class of the errors raised by OpenGeode." );
        exceptions.data = make( "OpenGeodeDataError", exceptions.base,
            "OpenGeode error caused by invalid input data." );
        exceptions.internal = make( "OpenGeodeInternalError", exceptions.base,
            "OpenGeode error caused by an internal failure." );
        exceptions.result = make( "OpenGeodeResultError", exceptions.base,
            "OpenGeode error caused by an unusable result." );
        pybind11::register_exception_translator( &translate );
        // Internal, test-only: raises a nested exception chain.
        module.def( "_throw_nested_exception_for_test", [] {
            try
            {
                throw std::runtime_error{ "inner failure" };
            }
            catch( ... )
            {
                geode::throw_lippincott< geode::OpenGeodeBasicException >(
                    geode::OpenGeodeException::TYPE::data,
                    "[Test] outer failure" );
            }
        } );
    }
} // namespace geode
