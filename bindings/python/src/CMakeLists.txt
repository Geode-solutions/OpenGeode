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

add_subdirectory(basic)
add_subdirectory(geometry)
add_subdirectory(image)
add_subdirectory(mesh)
add_subdirectory(model)
add_subdirectory(gdal)

# Helpers shared by the bindings, installed for the bindings of the downstream
# projects: #include <geode/bindings/python/common.hpp> (the include directory
# of the OpenGeode targets)
set(helpers_destination "${CMAKE_INSTALL_INCLUDEDIR}/geode/bindings/python")
install(FILES "common.hpp" "numpy_access.hpp"
    DESTINATION "${helpers_destination}"
    COMPONENT public
)
install(FILES "basic/factory.hpp"
    DESTINATION "${helpers_destination}/basic"
    COMPONENT public
)
install(FILES "mesh/index_check.hpp"
    DESTINATION "${helpers_destination}/mesh"
    COMPONENT public
)
install(FILES "model/model_vertex_check.hpp"
    DESTINATION "${helpers_destination}/model"
    COMPONENT public
)
