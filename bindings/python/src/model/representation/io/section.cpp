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

#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_input.hpp>
#include <geode/model/representation/io/section_output.hpp>

namespace geode
{
    void define_section_io( pybind11::module& module )
    {
        module.def( "save_section", &save_section );
        module.def( "load_section", &load_section );
        module.def(
            "check_section_missing_files", &check_section_missing_files );
        module.def( "is_section_loadable", &is_section_loadable );
        module.def( "is_section_saveable", &is_section_saveable );
        PYTHON_INPUT_CLASS( Section, "Section" );
        PYTHON_FACTORY_CLASS( SectionInputFactory );
        PYTHON_FACTORY_CLASS( SectionOutputFactory );
    }
} // namespace geode
