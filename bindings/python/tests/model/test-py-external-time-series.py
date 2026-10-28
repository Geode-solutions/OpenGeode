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

import os
import sys
import platform

if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ["PATH"].split(";") if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_mesh as mesh
import opengeode_py_model as model

PRESSURE = "pressure"
VELOCITY = "velocity"
TIMES = [0.0, 10.0]


def pressure(unique_vertex, time):
    return unique_vertex + time


def velocity(polyhedron):
    return [float(polyhedron), 1.0, 2.0]


def create_unique_vertices(brep):
    vertices = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(vertices)
    unique_vertices = []
    visited = [False] * brep.nb_unique_vertices()
    for block in brep.blocks():
        block_mesh = block.mesh()
        for vertex in range(block_mesh.nb_vertices()):
            unique_vertex = brep.unique_vertex(
                model.ComponentMeshVertex(block.component_id(), vertex)
            )
            if visited[unique_vertex]:
                continue
            visited[unique_vertex] = True
            builder.create_point(block_mesh.point(vertex))
            unique_vertices.append(unique_vertex)
    manager = vertices.vertex_attribute_manager()
    values = basic.AttributeValuesDouble()
    for time in TIMES:
        attribute_id = manager.create_time_step_attribute_variable_double(
            PRESSURE, time, values, basic.AttributeProperties()
        )
        attribute = manager.find_attribute_variable_double(attribute_id)
        for point, unique_vertex in enumerate(unique_vertices):
            attribute.set_value(point, pressure(unique_vertex, time))
    return vertices


def create_cell_barycenters(brep, block_id, translation):
    barycenters = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(barycenters)
    block_mesh = brep.block(block_id).mesh()
    manager = barycenters.vertex_attribute_manager()
    values = basic.AttributeValuesArrayDouble3()
    values.default_value = [0.0, 0.0, 0.0]
    values.no_value = [0.0, 0.0, 0.0]
    attribute_id = manager.create_time_step_attribute_variable_arraydouble3(
        VELOCITY, TIMES[1], values, basic.AttributeProperties()
    )
    attribute = manager.find_attribute_variable_arraydouble3(attribute_id)
    for polyhedron in range(block_mesh.nb_polyhedra()):
        point = builder.create_point(
            block_mesh.polyhedron_barycenter(polyhedron) + translation
        )
        attribute.set_value(point, velocity(polyhedron))
    return barycenters


def create_time_series(brep, block_id, translation=geometry.Point3D()):
    time_series = model.ExternalTimeSeries()
    time_series.set_unique_vertices(create_unique_vertices(brep))
    time_series.set_cell_barycenters(
        create_cell_barycenters(brep, block_id, translation)
    )
    return time_series


def check_pressures(brep):
    for block in brep.blocks():
        block_mesh = block.mesh()
        series = basic.AttributeTimeSeriesDouble(
            block_mesh.vertex_attribute_manager(), PRESSURE
        )
        if series.nb_time_steps() != len(TIMES):
            raise ValueError("[Test] Wrong number of pressure time steps")
        for step, time in enumerate(TIMES):
            if series.time(step) != time:
                raise ValueError("[Test] Wrong pressure time")
            for vertex in range(block_mesh.nb_vertices()):
                unique_vertex = brep.unique_vertex(
                    model.ComponentMeshVertex(block.component_id(), vertex)
                )
                if series.value(step, vertex) != pressure(unique_vertex, time):
                    raise ValueError("[Test] Wrong pressure on vertex")


def check_velocities(brep, block_id):
    block_mesh = brep.block(block_id).mesh()
    series = basic.AttributeTimeSeriesArrayDouble3(
        block_mesh.polyhedron_attribute_manager(), VELOCITY
    )
    if series.nb_time_steps() != 1 or series.time(0) != TIMES[1]:
        raise ValueError("[Test] Wrong velocity time steps")
    for polyhedron in range(block_mesh.nb_polyhedra()):
        if series.value(0, polyhedron) != velocity(polyhedron):
            raise ValueError("[Test] Wrong velocity on polyhedron")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    filename = os.path.join(data_dir, "test_mesh3.og_brep")
    brep = model.load_brep(filename)
    block_id = next(iter(brep.blocks())).id()
    time_series = create_time_series(brep, block_id)
    output = "python_time_series." + model.ExternalTimeSeries.native_extension_static()
    model.save_external_time_series(time_series, output)
    model.load_external_time_series(output).apply_to(brep)
    check_pressures(brep)
    check_velocities(brep, block_id)

    other = model.load_brep(filename)
    model.load_brep_time_series(other, output)
    check_pressures(other)

    wrong = create_time_series(
        brep, block_id, geometry.Point3D([100 * basic.GLOBAL_EPSILON, 0.0, 0.0])
    )
    try:
        wrong.apply_to(model.load_brep(filename))
    except Exception:
        pass
    else:
        raise ValueError("[Test] Translated barycenters should be rejected")
