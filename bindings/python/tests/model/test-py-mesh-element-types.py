# -*- coding: utf-8 -*-
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
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""MeshVertex, MeshEdge and MeshPolygon: the typed MeshElement of the mesh
vertex mappings of the models (ComponentMeshVertex.mesh_vertex,
MeshVertexMapping...)."""

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_mesh as mesh
import opengeode_py_model as model

TYPES = (mesh.MeshVertex, mesh.MeshEdge, mesh.MeshPolygon)
NAMESPACE = {"uuid": basic.uuid}
NAMESPACE.update({cls.__name__: cls for cls in TYPES + (mesh.MeshElement,)})


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def test_construction():
    identifier = basic.uuid()
    for cls in TYPES:
        name = cls.__name__
        check(issubclass(cls, mesh.MeshElement), name + " is a MeshElement")
        element = cls(identifier, 4)
        check(element.mesh_id == identifier and element.element_id == 4, name + " ids")
        keyword = cls(mesh_id=identifier, element_id=5)
        check(keyword.element_id == 5, name + " keyword constructor")
        default = cls()
        check(default.element_id == basic.NO_ID, name + " default index is NO_ID")
        check(default.mesh_id != cls().mesh_id, name + " default uuid is newly generated")
        element.element_id = 7
        element.mesh_id = default.mesh_id
        check(element == cls(default.mesh_id, 7), name + " modifiable fields")
        kind = name[len("Mesh"):].lower()
        check(kind in cls.__doc__, name + " documented")
        check(kind in cls.__init__.__doc__, name + " constructors documented")
        try:
            cls(identifier)
        except TypeError:
            pass
        else:
            raise ValueError("[Test] " + name + " needs both ids")


def test_repr_and_str():
    identifier = basic.uuid()
    for cls in TYPES:
        element = cls(identifier, 8)
        text = "%s(mesh_id=uuid('%s'), element_id=8)" % (cls.__name__, identifier.string())
        check(repr(element) == text, "repr " + repr(element))
        rebuilt = eval(repr(element), NAMESPACE)
        check(type(rebuilt) is cls and rebuilt == element, cls.__name__ + " repr roundtrip")
        check(str(element) == element.string(), cls.__name__ + " str is string()")
        check(element.string() == "[%s 8]" % identifier.string(), cls.__name__ + " string")


def test_comparison_and_hash():
    identifier = basic.uuid()
    for cls in TYPES:
        first, second, other = cls(identifier, 1), cls(identifier, 1), cls(identifier, 2)
        check(first == second and not first != second, cls.__name__ + " equal")
        check(first != other and not first == other, cls.__name__ + " different")
        check(first < other and not other < first, cls.__name__ + " ordered")
        check(hash(first) == hash(second), cls.__name__ + " equal hashes")
        check(len({first, second, other}) == 2, cls.__name__ + " in a set")
        check({first: 1}[second] == 1, cls.__name__ + " as dict key")
        # As in C++, the comparison is the one of MeshElement: the type is ignored
        base = mesh.MeshElement(identifier, 1)
        check(first == base and hash(first) == hash(base), cls.__name__ + " == MeshElement")
    check(mesh.MeshVertex(identifier, 3) == mesh.MeshEdge(identifier, 3), "type ignored")
    check(sorted([mesh.MeshPolygon(identifier, 5), mesh.MeshPolygon(identifier, 2)])[0].element_id == 2,
          "sortable")


def test_component_mesh_vertex():
    component = model.ComponentID(model.ComponentType("Surface"), basic.uuid())
    vertex = model.ComponentMeshVertex(component, 6)
    mesh_vertex = vertex.mesh_vertex()
    check(type(mesh_vertex) is mesh.MeshVertex, "mesh_vertex returns a MeshVertex")
    check(mesh_vertex.mesh_id == component.id and mesh_vertex.element_id == 6,
          "mesh_vertex is (component uuid, vertex)")
    check("MeshVertex" in model.ComponentMeshVertex.mesh_vertex.__doc__, "mesh_vertex documented")


def test_component_mesh_element():
    component = model.ComponentID(model.ComponentType("Surface"), basic.uuid())
    other = model.ComponentID(model.ComponentType("Line"), component.id)
    first = model.ComponentMeshElement(component, 3)
    second = model.ComponentMeshElement(component_id=component, element_id=3)
    check(first.component_id == component and first.element_id == 3, "fields")
    check(first == second and not first != second, "equal")
    check(hash(first) == hash(second) and len({first, second}) == 1, "hash")
    check({first: 1}[second] == 1, "dict key")
    for different in (
        model.ComponentMeshElement(component, 4),
        # Same uuid, another component type: different, as in C++
        model.ComponentMeshElement(other, 3),
    ):
        check(first != different and not first == different, "different " + repr(different))
    check(len({first, model.ComponentMeshElement(component, 4)}) == 2, "distinct in a set")
    element = first.mesh_element()
    check(type(element) is mesh.MeshElement, "mesh_element returns a MeshElement")
    check(element == mesh.MeshElement(component.id, 3), "mesh_element is (uuid, element)")
    first.element_id = 9
    first.component_id = other
    check(first == model.ComponentMeshElement(other, 9), "modifiable fields")
    default = model.ComponentMeshElement()
    check(default.element_id == basic.NO_ID, "default element is NO_ID")
    check(default.component_id.type == model.ComponentType("undefined"), "default type")


if __name__ == "__main__":
    test_construction()
    test_repr_and_str()
    test_comparison_and_hash()
    test_component_mesh_vertex()
    test_component_mesh_element()
