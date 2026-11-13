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
# all copies or substantial portions of the Software.
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
import tempfile
import threading

import geode_test_env

import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh
import opengeode_py_model as model
import opengeode

TIMEOUT = 60
ATTEMPTS = 3

DATA_DIR = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../../tests/data")
)

# The checks below are deterministic thanks to a huge interpreter switch
# interval: the interpreter never forces a GIL hand-over, so a thread keeps the
# GIL until it blocks or exits. If a bound call holds the GIL, the worker thread
# runs the whole call (and everything after it, up to `finished.set()`) before
# any other Python thread can run. If the call releases the GIL, the main
# thread is scheduled as soon as the call is inside C++ and sees the call
# still running. Holding the GIL can therefore never pass; the only possible
# flakiness is a call finishing before the main thread wakes up, hence the
# workloads repeat the call and `ATTEMPTS` retries are allowed.


class HugeSwitchInterval:
    def __enter__(self):
        self.previous = sys.getswitchinterval()
        sys.setswitchinterval(1000)

    def __exit__(self, *args):
        sys.setswitchinterval(self.previous)


def run_in_worker(work):
    """Run `work` in a thread; return (`finished` flag seen by the main thread
    as soon as it could run, result)."""
    ready = threading.Event()
    finished = threading.Event()
    box = {}

    def target():
        ready.set()
        try:
            box["result"] = work()
        except BaseException as error:
            box["error"] = error
        finished.set()

    thread = threading.Thread(target=target)
    thread.start()
    if not ready.wait(TIMEOUT):
        raise ValueError("[Test] Worker thread did not start")
    main_ran_during_call = not finished.is_set()
    thread.join(TIMEOUT)
    if "error" in box:
        raise box["error"]
    return main_ran_during_call, box.get("result")


def assert_gil_released(name, work):
    with HugeSwitchInterval():
        for _ in range(ATTEMPTS):
            main_ran_during_call, result = run_in_worker(work)
            if main_ran_during_call:
                return result
    raise ValueError(f"[Test] {name} kept the GIL during the whole call")


def grid3d(nb_cells):
    grid = mesh.RegularGrid3D.create()
    builder = mesh.RegularGridBuilder3D.create(grid)
    builder.initialize_cartesian_grid(
        geom.Point3D([0, 0, 0]), [nb_cells, nb_cells, nb_cells], 1.0
    )
    return grid


def point_set(nb_points):
    result = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(result)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_vertices(nb_points - 1)
    return result


def triangulated_surface(nb_vertices):
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(nb_vertices)
    for v in range(nb_vertices):
        builder.set_point(v, geom.Point3D([v, v % 7, v % 13]))
    for v in range(nb_vertices - 2):
        builder.create_triangle([v, v + 1, v + 2])
    return surface, builder


def load_brep(name):
    filename = os.path.join(DATA_DIR, name)
    return opengeode.load_one(model.load_brep(filename), filename)


def check_distance_transform():
    def work():
        for i in range(5):
            mesh.euclidean_distance_transform3D(
                grid3d(80), [[0, 0, 0]], f"gil_edt{i}")

    assert_gil_released("euclidean_distance_transform3D", work)


def check_save_load(directory):
    source = point_set(1000000)
    filename = os.path.join(directory, "big.og_pts3d")

    def work():
        loaded = None
        for _ in range(3):
            mesh.save_point_set3D(source, filename)
            loaded = mesh.load_point_set3D(filename)
        return loaded

    loaded = assert_gil_released("save/load_point_set3D", work)
    if len(loaded) != 1 or loaded[0].nb_vertices() != source.nb_vertices():
        raise ValueError("[Test] Wrong vertex count after released round trip")


def check_conversion():
    def work():
        return [mesh.convert_grid_into_tetrahedral_solid(grid3d(20))
                for _ in range(3)]

    solids = assert_gil_released("convert_grid_into_tetrahedral_solid", work)
    for solid in solids:
        if solid.nb_polyhedra() != 20 * 20 * 20 * 6:
            raise ValueError("[Test] Wrong converted solid")


def check_builder_adjacencies():
    surface, builder = triangulated_surface(20000)

    def work():
        for _ in range(5):
            builder.compute_polygon_adjacencies()

    assert_gil_released("compute_polygon_adjacencies", work)
    if surface.polygon_adjacent(mesh.PolygonEdge(0, 1)) != 1:
        raise ValueError("[Test] Wrong adjacency after released computation")


def check_brep_io(directory):
    brep = load_brep("prism_curve.og_brep")
    filename = os.path.join(directory, "released." + brep.native_extension())

    def work():
        reloaded = None
        for _ in range(30):
            model.save_brep(brep, filename)
            reloaded = opengeode.load_one(model.load_brep(filename), filename)
        return reloaded

    reloaded = assert_gil_released("save/load_brep", work)
    if reloaded.nb_blocks() != brep.nb_blocks():
        raise ValueError("[Test] Wrong BRep after released round trip")
    if reloaded.nb_surfaces() != brep.nb_surfaces():
        raise ValueError("[Test] Wrong BRep after released round trip")


def check_concatenation():
    expected = []

    def work():
        results = []
        for _ in range(20):
            brep = load_brep("prism_curve.og_brep")
            other = load_brep("dangling.og_brep")
            expected.append(brep.nb_surfaces() + other.nb_surfaces())
            model.BRepConcatener(brep).concatenate(other)
            results.append(brep.nb_surfaces())
        return results

    results = assert_gil_released("BRepConcatener.concatenate", work)
    if results != expected:
        raise ValueError("[Test] Wrong concatenated BRep")


def check_model_conversion():
    def work():
        breps = []
        for _ in range(5):
            brep = load_brep("layers.og_brep")
            model.triangulate_brep_surface_meshes(brep)
            model.convert_brep_surface_meshes_into_triangulated_surfaces(brep)
            model.convert_brep_block_meshes_into_tetrahedral_solids(brep)
            breps.append(brep)
        return breps

    for brep in assert_gil_released("convert_model_meshes", work):
        for surface in brep.surfaces():
            mesh_ = surface.mesh()
            for p in range(mesh_.nb_polygons()):
                if mesh_.nb_polygon_vertices(p) != 3:
                    raise ValueError("[Test] Surface mesh not triangulated")


def check_nn_search():
    points = [geom.Point3D([i % 50, (i // 50) % 50, i // 2500])
              for i in range(20000)]

    def work():
        mappings = []
        for _ in range(5):
            search = geom.NNSearch3D(points)
            mappings.append(search.radius_colocated_index_mapping(0.5))
        return mappings

    for info in assert_gil_released("NNSearch3D", work):
        if info.nb_unique_points() != len(points):
            raise ValueError("[Test] Wrong colocated mapping")


def check_exception_translated_after_release(directory):
    missing = os.path.join(directory, "does_not_exist.og_tsf3d")
    for call in [
        lambda: mesh.load_triangulated_surface3D(missing),
        lambda: model.load_brep(os.path.join(directory, "missing.og_brep")),
        lambda: opengeode.load_graph("file.unknown_extension"),
    ]:
        try:
            call()
        except opengeode.OpenGeodeDataError as error:
            if error.type != "data" or not error.message:
                raise ValueError("[Test] Wrong exception translated after release")
            continue
        raise ValueError("[Test] The released call should raise OpenGeodeDataError")

    def failing():
        mesh.load_triangulated_surface3D(missing)

    # The exception also crosses a released call made from a worker thread
    with HugeSwitchInterval():
        try:
            run_in_worker(failing)
        except opengeode.OpenGeodeDataError:
            return
    raise ValueError("[Test] Worker thread should have raised OpenGeodeDataError")


def check_loadable_queries(directory):
    surface, _ = triangulated_surface(10)
    filename = os.path.join(directory, "small.og_tsf3d")
    mesh.save_triangulated_surface3D(surface, filename)
    if mesh.is_triangulated_surface_loadable3D(filename).value() <= 0:
        raise ValueError("[Test] og_tsf3d should be loadable")
    if not mesh.is_triangulated_surface_saveable3D(surface, filename):
        raise ValueError("[Test] og_tsf3d should be saveable")
    if mesh.is_triangulated_surface_loadable3D("file.unknown_extension").value() > 0:
        raise ValueError("[Test] Unknown extension should not be loadable")


def check_concurrent_loads(directory):
    surface, _ = triangulated_surface(20000)
    filenames = []
    for i in range(4):
        filename = os.path.join(directory, f"concurrent{i}.og_tsf3d")
        mesh.save_triangulated_surface3D(surface, filename)
        filenames.append(filename)
    results = [None] * len(filenames)
    errors = []

    def work(i):
        try:
            results[i] = mesh.load_triangulated_surface3D(filenames[i])
        except Exception as error:  # pragma: no cover
            errors.append(error)

    threads = [threading.Thread(target=work, args=(i,))
               for i in range(len(filenames))]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join(TIMEOUT)
    if errors:
        raise ValueError(f"[Test] Concurrent loads failed: {errors}")
    for result in results:
        if result is None or result[0].nb_vertices() != surface.nb_vertices():
            raise ValueError("[Test] Wrong concurrent load result")


def check_bridge_delivers_during_released_call():
    """Task 5 bridge: the progress event emitted by a released call reaches
    the Python listener while that call is still running. With the GIL held,
    the listener could only run after the worker thread finished."""
    Logger = opengeode.Logger
    ProgressLogger = opengeode.ProgressLogger
    message = "Compute 3D euclidian distance"
    delivered_during_call = []

    opengeode.forward_logs_to_python("opengeode.gil_test")
    try:
        with HugeSwitchInterval():
            for attempt in range(ATTEMPTS):
                start_received = threading.Event()
                finished = threading.Event()
                seen = []

                def listener(event):
                    if event.message == message and event.kind == "start" and not seen:
                        seen.append(not finished.is_set())
                        start_received.set()

                opengeode.add_progress_listener(listener)
                try:
                    def work():
                        for i in range(5):
                            mesh.euclidean_distance_transform3D(
                                grid3d(80), [[0, 0, 0]], f"gil_bridge{attempt}{i}")
                        finished.set()

                    thread = threading.Thread(target=work)
                    thread.start()
                    if not start_received.wait(TIMEOUT):
                        raise ValueError("[Test] Progress start event never forwarded")
                    thread.join(TIMEOUT)
                finally:
                    opengeode.remove_progress_listener(listener)
                delivered_during_call.append(seen[0])
                if seen[0]:
                    break
    finally:
        opengeode.stop_forwarding_logs()
    if not delivered_during_call[-1]:
        raise ValueError(
            "[Test] The progress event was only delivered after the "
            "released call returned"
        )


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    check_distance_transform()
    check_conversion()
    check_builder_adjacencies()
    check_nn_search()
    check_model_conversion()
    check_concatenation()
    check_bridge_delivers_during_released_call()
    with tempfile.TemporaryDirectory() as directory:
        check_save_load(directory)
        check_brep_io(directory)
        check_concurrent_loads(directory)
        check_exception_translated_after_release(directory)
        check_loadable_queries(directory)
