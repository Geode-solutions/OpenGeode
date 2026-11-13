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

from opengeode_py_mesh import *
OpenGeodeMeshLibrary.initialize()

# The registry of the generic functions (load, save, builder...) is in basic:
# a sibling module in the wheel package, a top-level module in the build tree
from opengeode_py_mesh import _bound_interface

try:
    from .basic import extend_type as _extend_type
    from .basic import register_type as _register_type
except ImportError:
    from basic import extend_type as _extend_type
    from basic import register_type as _register_type


def _register_mesh(cls: type, name: str, dimension: str, builder: "type | None") -> None:
    # Bound functions: load_<name><dimension>, is_<name>_loadable<dimension>...
    module = globals()
    prefix = cls.__name__[: len(cls.__name__) - len(dimension)]
    _register_type(
        cls,
        load=module[f"load_{name}{dimension}"],
        save=module[f"save_{name}{dimension}"],
        is_loadable=module[f"is_{name}_loadable{dimension}"],
        is_saveable=module[f"is_{name}_saveable{dimension}"],
        input_extensions=module[f"{prefix}InputFactory{dimension}"].list_creators,
        output_extensions=module[f"{prefix}OutputFactory{dimension}"].list_creators,
        additional_files=module[f"{name}_additional_files{dimension}"],
        builder=builder,
    )


_register_mesh(VertexSet, "vertex_set", "", VertexSetBuilder)
_register_mesh(Graph, "graph", "", GraphBuilder)
for _dimension in ("2D", "3D"):
    _module = globals()
    for _name, _class in (
        ("point_set", "PointSet"),
        ("edged_curve", "EdgedCurve"),
        ("polygonal_surface", "PolygonalSurface"),
        ("triangulated_surface", "TriangulatedSurface"),
        ("regular_grid", "RegularGrid"),
    ):
        _register_mesh(
            _module[_class + _dimension],
            _name,
            _dimension,
            _module[_class + "Builder" + _dimension],
        )
    _register_mesh(
        _module["LightRegularGrid" + _dimension], "light_regular_grid", _dimension, None
    )
    _register_type(
        _module["SurfaceMesh" + _dimension],
        builder=_module["SurfaceMeshBuilder" + _dimension],
    )
for _name, _class in (
    ("polyhedral_solid", PolyhedralSolid3D),
    ("tetrahedral_solid", TetrahedralSolid3D),
    ("hybrid_solid", HybridSolid3D),
):
    _register_mesh(_class, _name, "3D", globals()[_class.__name__[:-2] + "Builder3D"])
_register_type(SolidMesh3D, builder=SolidMeshBuilder3D)
# Every mesh (e.g. the SurfaceMesh3D of a model component) is handled as its
# most derived bound class (TriangulatedSurface3D...)
_extend_type(VertexSet, resolve=_bound_interface)
del _dimension, _module, _name, _class
