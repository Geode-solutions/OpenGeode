# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Header-only helpers shared by the bindings, installed by every configure
# (whether or not OPENGEODE_WITH_PYTHON is ON, without pybind11) so that the
# OpenGeode packages always provide them to the bindings of the downstream
# projects: #include <geode/bindings/python/common.hpp>
cmake_minimum_required(VERSION 3.15)
if(NOT PROJECT_NAME)
    project(OpenGeodeBindingsHelpers NONE)
endif()
include(GNUInstallDirs)

set(helpers_source "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(helpers_destination "${CMAKE_INSTALL_INCLUDEDIR}/geode/bindings/python")
install(FILES "${helpers_source}/common.hpp" "${helpers_source}/numpy_access.hpp"
    DESTINATION "${helpers_destination}"
    COMPONENT public
)
install(FILES "${helpers_source}/basic/factory.hpp" "${helpers_source}/basic/mapping.hpp"
    DESTINATION "${helpers_destination}/basic"
    COMPONENT public
)
install(FILES "${helpers_source}/mesh/index_check.hpp"
    DESTINATION "${helpers_destination}/mesh"
    COMPONENT public
)
install(FILES "${helpers_source}/model/model_vertex_check.hpp" "${helpers_source}/model/model_mapping.hpp"
    DESTINATION "${helpers_destination}/model"
    COMPONENT public
)
