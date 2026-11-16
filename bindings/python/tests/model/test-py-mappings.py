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

"""Size, enumeration and modification of the mappings (BijectiveMapping,
GenericMapping and the ModelMapping of the component types)."""

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_mesh as mesh
import opengeode_py_model as model
import opengeode  # initializes the libraries (mesh implementations)


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    except Exception as other:
        raise ValueError("[Test] %s: expected %s, got %r" % (message, error.__name__, other))
    raise ValueError("[Test] %s: expected %s, nothing raised" % (message, error.__name__))


def test_bijective(values, mapping_type, outputs=None):
    a, b, c, d = values
    oa, ob, oc, od = outputs or values
    name = mapping_type.__name__
    mapping = mapping_type()
    check(mapping.size() == 0 and len(mapping) == 0 and not mapping, name + " empty")
    check(mapping.in2out_map() == {} and mapping.out2in_map() == {}, name + " empty maps")
    mapping.map(a, ob)
    mapping.map(input=b, output=oa)
    check(mapping.size() == 2 and len(mapping) == 2, name + " size 2")
    check(mapping.in2out(a) == ob and mapping.out2in(oa) == b, name + " lookups")
    check(mapping.in2out_map() == {a: ob, b: oa}, name + " in2out_map")
    check(list(mapping.in2out_map()) == [a, b], name + " in2out_map order")
    check(mapping.out2in_map() == {ob: a, oa: b}, name + " out2in_map")
    # One-to-one: mapping a again removes its previous pair
    mapping.map(a, oc)
    check(mapping.in2out(a) == oc and not mapping.has_mapping_output(ob), name + " remap input")
    check(list(mapping.in2out_map()) == [a, b], name + " remapped input keeps its place")
    # ... and mapping to a used output removes the pair of that output
    mapping.map(d, oc)
    check(not mapping.has_mapping_input(a) and mapping.out2in(oc) == d, name + " remap output")
    check(len(mapping) == 2, name + " still one-to-one")
    # The dicts are copies
    copy = mapping.in2out_map()
    copy.clear()
    check(len(mapping.in2out_map()) == 2, name + " in2out_map is a copy")
    mapping.erase_in(d)
    check(not mapping.has_mapping_output(oc) and len(mapping) == 1, name + " erase_in")
    mapping.erase_in(input=d)
    mapping.erase_out(od)
    check(len(mapping) == 1, name + " erase of unmapped values does nothing")
    mapping.erase_out(output=oa)
    check(len(mapping) == 0 and not mapping.has_mapping_input(b), name + " erase_out")
    mapping.map(a, oa)
    mapping.map(b, ob)
    moved = mapping_type(mapping)
    check(len(moved) == 2 and len(mapping) == 0, name + " move constructor empties the source")
    moved.clear()
    check(len(moved) == 0 and moved.in2out_map() == {}, name + " clear")
    check_raises(lambda: moved.in2out(a), KeyError, name + " in2out after clear")


def test_generic(values, mapping_type):
    a, b, c, d = values
    name = mapping_type.__name__
    mapping = mapping_type()
    check(mapping.size_in() == 0 and mapping.size_out() == 0, name + " empty")
    check(not hasattr(mapping, "__len__"), name + " has no ambiguous len")
    mapping.map(a, b)
    mapping.map(a, c)
    mapping.map(a, b)
    mapping.map(input=d, output=b)
    check(mapping.size_in() == 2 and mapping.size_out() == 2, name + " sizes")
    check(mapping.in2out(a) == [b, c] and mapping.out2in(b) == [a, d], name + " lookups")
    check(mapping.in2out_map() == {a: [b, c], d: [b]}, name + " in2out_map")
    check(list(mapping.in2out_map()) == [a, d], name + " in2out_map order")
    check(mapping.out2in_map() == {b: [a, d], c: [a]}, name + " out2in_map")
    mapping.unmap(a, b)
    check(mapping.in2out(a) == [c] and mapping.out2in(b) == [d], name + " unmap")
    mapping.unmap(input=a, output=d)
    mapping.unmap(c, c)
    check(mapping.size_in() == 2 and mapping.size_out() == 2, name + " unmap of unmapped pairs")
    mapping.erase_in(a)
    check(not mapping.has_mapping_input(a) and not mapping.has_mapping_output(c),
          name + " erase_in removes the outputs left without input")
    mapping.erase_out(b)
    check(mapping.size_in() == 0 and mapping.size_out() == 0, name + " erase_out")
    mapping.erase_in(a)
    mapping.erase_out(output=b)
    mapping.map(a, b)
    mapping.map(c, d)
    moved = mapping_type(mapping=mapping)
    check(moved.size_in() == 2 and mapping.size_in() == 0, name + " move constructor")
    moved.clear()
    check(moved.size_in() == 0 and moved.out2in_map() == {}, name + " clear")
    check_raises(lambda: moved.out2in(b), KeyError, name + " out2in after clear")


def test_conversion_mappings():
    # The mappings of the conversions to a mesh: sizes and dicts of real data
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    line = builder.add_line()
    line_builder = builder.line_mesh_builder(brep.line(line))
    line_builder.create_vertices(3)
    line_builder.create_edge_with_vertices(0, 1)
    line_builder.create_edge_with_vertices(1, 2)
    curve, mappings = model.convert_brep_into_curve(brep)
    edges = mappings.line_edges_mapping
    check(edges.size() == curve.nb_edges() == 2, "line_edges_mapping size")
    check(sorted(edges.in2out_map().values()) == [0, 1], "line_edges_mapping outputs")
    check(all(isinstance(key, mesh.MeshElement) for key in edges.in2out_map()), "keys")
    check(set(edges.out2in_map().values()) == {mesh.MeshElement(line, 0), mesh.MeshElement(line, 1)},
          "line_edges_mapping inputs")
    check(len(mappings.solid_polyhedra_mapping) == 0, "unfilled mapping is empty")


def test_model_mappings():
    source = model.BRep()
    builder = model.BRepBuilder(source)
    corners = [builder.add_corner(), builder.add_corner()]
    surface = builder.add_surface()
    copy = model.BRepBuilder(model.BRep()).copy(source)
    corner_type = model.Corner3D.component_type_static()
    surface_type = model.Surface3D.component_type_static()
    types = copy.component_types()
    check([type.get() for type in types] == sorted(type.get() for type in types), "sorted types")
    check(corner_type in types and surface_type in types, "copied types listed")
    check(copy.has_mapping_type(corner_type) and copy.has_mapping_type(type=surface_type),
          "has_mapping_type")
    corner_mapping = copy.at(corner_type)
    check(len(corner_mapping) == 2 and list(corner_mapping.in2out_map()) == corners,
          "corner mapping enumerates the corners")
    check(len(copy.at(surface_type)) == 1 and copy.at(surface_type).has_mapping_input(surface),
          "surface mapping")
    # at() returns a copy: the mapping is modified in place by type
    extra = basic.uuid()
    copy.at(corner_type).map(extra, extra)
    check(len(copy.at(corner_type)) == 2, "modifying the copy of at() changes nothing")
    copy.map(corner_type, extra, extra)
    check(len(copy.at(corner_type)) == 3, "map by type")
    check(copy.in2out(corner_type, extra) == extra and copy.out2in(type=corner_type, output=extra) == extra,
          "lookups by type")
    check(copy.has_mapping_input(corner_type, extra) and copy.has_mapping_output(corner_type, extra),
          "has_mapping_input/output by type")
    line_type = model.ComponentType("NoMapping")
    check(not copy.has_mapping_type(line_type), "type without mapping")
    check(not copy.has_mapping_input(line_type, extra) and not copy.has_mapping_output(line_type, extra),
          "no mapping for the type: not mapped")
    check_raises(lambda: copy.in2out(line_type, extra), KeyError, "in2out of a type without mapping")
    check_raises(lambda: copy.out2in(corner_type, basic.uuid()), KeyError, "out2in of an unmapped value")
    copy.erase_in(corner_type, extra)
    check(len(copy.at(corner_type)) == 2, "erase_in by type")
    copy.map(corner_type, extra, extra)
    copy.erase_out(type=corner_type, output=extra)
    check(len(copy.at(corner_type)) == 2, "erase_out by type")
    copy.erase_in(line_type, extra)
    copy.erase_out(line_type, extra)
    check(not copy.has_mapping_type(line_type), "erase on a type without mapping does nothing")
    copy.map(line_type, extra, corners[0])
    check(copy.has_mapping_type(line_type) and copy.in2out(line_type, extra) == corners[0],
          "map creates the mapping of a new type")
    check(not hasattr(copy, "unmap"), "no unmap on a bijective model mapping")
    copy.remove(corner_type)
    check(not copy.has_mapping_type(corner_type), "remove")
    copy.remove(type=corner_type)
    filled = basic.BijectiveMappinguuid()
    filled.map(extra, corners[0])
    copy.emplace(corner_type, filled)
    filled.map(corners[1], corners[1])
    check(len(copy.at(corner_type)) == 1, "emplace copies the mapping")
    check_raises(lambda: copy.emplace(type=corner_type, mapping=filled), basic.OpenGeodeDataError,
                 "emplace of an existing type")
    generic = model.ModelGenericMapping()
    check(generic.component_types() == [] and not generic.has_mapping_type(corner_type),
          "empty generic model mapping")
    uuids = basic.GenericMappinguuid()
    uuids.map(corners[0], corners[1])
    uuids.map(corners[0], extra)
    generic.emplace(corner_type, uuids)
    check(generic.component_types() == [corner_type], "generic component types")
    check(generic.at(corner_type).in2out(corners[0]) == [corners[1], extra], "generic at")
    generic.map(corner_type, corners[0], corners[0])
    check(generic.in2out(corner_type, corners[0]) == [corners[1], extra, corners[0]], "generic map")
    generic.unmap(corner_type, corners[0], extra)
    check(generic.out2in(corner_type, corners[1]) == [corners[0]]
          and generic.in2out(corner_type, corners[0]) == [corners[1], corners[0]], "generic unmap")
    generic.unmap(line_type, corners[0], extra)
    check(not generic.has_mapping_type(line_type), "unmap on a type without mapping does nothing")
    moved = model.ModelGenericMapping(generic)
    check(moved.has_mapping_type(corner_type) and generic.component_types() == [],
          "model mapping move constructor empties the source")


def test_mesh_mappings():
    surface = model.ComponentID(model.Surface3D.component_type_static(), basic.uuid())
    line = model.ComponentID(model.Line3D.component_type_static(), basic.uuid())
    cases = [
        (model.MeshElementMapping, mesh.MeshElement, mesh.MeshElement),
        (model.MeshVertexMapping, mesh.MeshVertex, mesh.MeshVertex),
        (model.ComponentMeshElementMapping, mesh.MeshElement,
         lambda component, index: model.ComponentMeshElement(component, index)),
        (model.ComponentMeshVertexMapping, mesh.MeshVertex,
         lambda component, index: model.ComponentMeshVertex(component, index)),
    ]
    for mapping_type, make_input, make_output in cases:
        name = mapping_type.__name__
        inputs = [make_input(surface.id, index) for index in range(4)]
        if mapping_type in (model.ComponentMeshElementMapping, model.ComponentMeshVertexMapping):
            outputs = [make_output(line if index % 2 else surface, index) for index in range(4)]
        else:
            outputs = [make_input(line.id, index) for index in range(4)]
        mapping = mapping_type()
        mapping.map(inputs[0], outputs[0])
        mapping.map(inputs[0], outputs[1])
        mapping.map(inputs[1], outputs[1])
        check(mapping.size_in() == 2 and mapping.size_out() == 2, name + " sizes")
        check(mapping.in2out(inputs[0]) == outputs[:2], name + " in2out")
        check(mapping.out2in_map() == {outputs[0]: [inputs[0]], outputs[1]: inputs[:2]},
              name + " out2in_map with hashable outputs")
        check(all(type(value) is type(outputs[0]) for value in mapping.in2out_map()[inputs[0]]),
              name + " output type")
        mapping.erase_out(outputs[1])
        check(mapping.size_in() == 1 and mapping.size_out() == 1, name + " erase_out")
    # The typed elements are not interchangeable
    check_raises(lambda: model.MeshVertexMapping().map(mesh.MeshElement(), mesh.MeshVertex()),
                 TypeError, "MeshVertexMapping takes MeshVertex")

    corner_type = model.Corner3D.component_type_static()
    vertices = model.ModelMeshesVertexMapping()
    vertex_mapping = model.MeshVertexMapping()
    vertex_mapping.map(mesh.MeshVertex(surface.id, 0), mesh.MeshVertex(line.id, 1))
    vertices.emplace(corner_type, vertex_mapping)
    check(vertices.component_types() == [corner_type], "ModelMeshesVertexMapping types")
    check(vertices.at(corner_type).size_in() == 1, "ModelMeshesVertexMapping at")
    vertices.map(corner_type, mesh.MeshVertex(surface.id, 2), mesh.MeshVertex(line.id, 1))
    check(vertices.out2in(corner_type, mesh.MeshVertex(line.id, 1))
          == [mesh.MeshVertex(surface.id, 0), mesh.MeshVertex(surface.id, 2)], "mesh mapping by type")
    check_raises(lambda: vertices.map(corner_type, mesh.MeshElement(), mesh.MeshVertex()), TypeError,
                 "ModelMeshesVertexMapping.map takes MeshVertex")
    check("MeshVertexMapping" in model.ModelMeshesVertexMapping.at.__doc__, "at doc")
    elements = model.ModelMeshesElementMapping()
    check(elements.component_types() == [], "ModelMeshesElementMapping empty")
    check("MeshElementMapping" in model.ModelMeshesElementMapping.at.__doc__, "at doc")

    # The added, removed and unchanged components: lists of uuids
    uuids = model.ModelComponentUuidsMapping()
    identifiers = [basic.uuid(), basic.uuid()]
    uuids.emplace(corner_type, identifiers)
    listed = uuids.at(corner_type)
    check(listed == identifiers, "ModelComponentUuidsMapping at")
    listed.append(basic.uuid())
    check(len(uuids.at(corner_type)) == 2, "at returns a copy of the list")
    check("copy" in model.ModelComponentUuidsMapping.at.__doc__
          and "does not change this mapping" in model.ModelComponentUuidsMapping.at.__doc__,
          "at doc says copy")
    check_raises(lambda: uuids.at(model.Line3D.component_type_static()), KeyError, "at KeyError")
    check(not hasattr(uuids, "map") and not hasattr(uuids, "in2out"), "no per-type lookups for lists")

    mappings = model.ModelMappings()
    for field, field_type in (
        ("component_mapping", model.ModelGenericMapping),
        ("added_components", model.ModelComponentUuidsMapping),
        ("removed_components", model.ModelComponentUuidsMapping),
        ("unchanged_components", model.ModelComponentUuidsMapping),
        ("mesh_element_mapping", model.ModelMeshesElementMapping),
        ("mesh_vertices_mapping", model.ModelMeshesVertexMapping),
    ):
        value = getattr(mappings, field)
        check(type(value) is field_type, field + " type")
        check(value.component_types() == [], field + " empty")
        check(getattr(model.ModelMappings, field).__doc__, field + " documented")
    # The fields are the mappings of the struct, not copies
    mappings.added_components.emplace(corner_type, identifiers)
    check(mappings.added_components.at(corner_type) == identifiers, "fields are references")
    mappings.component_mapping.emplace(corner_type, basic.GenericMappinguuid())
    check(mappings.component_mapping.has_mapping_type(corner_type), "component_mapping reference")
    mappings.component_mapping.map(corner_type, identifiers[0], identifiers[1])
    check(mappings.component_mapping.in2out(corner_type, identifiers[0]) == [identifiers[1]],
          "nested modification")


def test_no_dangling_reference():
    # The mappings of the types live in a hash map: emplace (rehash) and
    # remove move them. at() returns a copy, which stays valid.
    for mapping in (model.ModelGenericMapping(), model.ModelMappings().component_mapping):
        first = model.ComponentType("t0")
        uuids = basic.GenericMappinguuid()
        for _ in range(50):
            uuids.map(basic.uuid(), basic.uuid())
        mapping.emplace(first, uuids)
        reference = mapping.at(first)
        for index in range(1, 200):
            mapping.emplace(model.ComponentType("t%d" % index), basic.GenericMappinguuid())
        check(reference.size_in() == 50 and mapping.at(first).size_in() == 50, "after emplace")
        mapping.remove(first)
        check(reference.size_in() == 50 and len(reference.in2out_map()) == 50, "after remove")
    # A moved model mapping: the copy does not depend on the source
    source = model.ModelGenericMapping()
    source.emplace(model.ComponentType("t"), uuids)
    copy = source.at(model.ComponentType("t"))
    moved = model.ModelGenericMapping(source)
    del moved
    check(copy.size_in() == 50, "after move")


def test_generic_bool():
    mapping = basic.GenericMappinguuid()
    check(not mapping, "empty GenericMapping is falsy")
    mapping.map(basic.uuid(), basic.uuid())
    check(bool(mapping), "non-empty GenericMapping is truthy")
    mapping.clear()
    check(not mapping, "cleared GenericMapping is falsy")


if __name__ == "__main__":
    uuids = [basic.uuid() for _ in range(4)]
    test_bijective(uuids, basic.BijectiveMappinguuid)
    test_generic(uuids, basic.GenericMappinguuid)
    elements = [mesh.MeshElement(uuids[0], index) for index in range(4)]
    test_bijective(elements, model.MeshElementToIndexMapping, outputs=[10, 11, 12, 13])
    test_bijective([0, 1, 2, 3], model.IndexToIndexMapping)
    surface_id = basic.uuid()
    test_generic([mesh.MeshElement(surface_id, index) for index in range(4)], model.MeshElementMapping)
    test_generic([mesh.MeshVertex(surface_id, index) for index in range(4)], model.MeshVertexMapping)
    test_conversion_mappings()
    test_model_mappings()
    test_mesh_mappings()
    test_no_dangling_reference()
    test_generic_bool()
