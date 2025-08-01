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

#include "../../../basic/factory.hpp"
#include "../../../basic/input.hpp"
#include "../../../common.hpp"

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_output.hpp>

namespace geode
{
    void define_brep_io( pybind11::module& module )
    {
        module.def( "save_brep", &save_brep );
        module.def( "load_brep", &load_brep );
        module.def( "brep_additional_files", &brep_additional_files );
        module.def( "brep_object_priority", &brep_object_priority );
        module.def( "is_brep_loadable", &is_brep_loadable );
        module.def( "is_brep_saveable", &is_brep_saveable );
        PYTHON_INPUT_CLASS( BRep, "BRep" );
        PYTHON_FACTORY_CLASS( BRepInputFactory );
        PYTHON_FACTORY_CLASS( BRepOutputFactory );
    }
} // namespace geode
