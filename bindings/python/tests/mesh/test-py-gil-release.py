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
import tempfile
import threading
import time

import geode_test_env

import opengeode
import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh

TIMEOUT = 60
# A call holding the GIL still lets the counter thread run for about one
# interpreter switch interval (around 5 ms) while it is entered. A released
# call lets it run at full speed for the whole duration, so we require a
# fraction of the idle counting rate over the call duration.
MIN_RATE_FRACTION = 0.3
# Guards against a test workload too small to separate both cases
MIN_CALL_DURATION = 0.1


class CounterThread:
    """Python thread incrementing a counter as fast as the GIL lets it."""

    def __init__(self):
        self.count = 0
        self._stop = threading.Event()
        self._started = threading.Event()
        self._thread = threading.Thread(target=self._run, daemon=True)

    def _run(self):
        self._started.set()
        while not self._stop.is_set():
            self.count += 1

    def __enter__(self):
        self._thread.start()
        if not self._started.wait(TIMEOUT):
            raise ValueError("[Test] Counter thread did not start")
        while self.count == 0:
            time.sleep(0.001)
        return self

    def __exit__(self, *args):
        self._stop.set()
        self._thread.join(TIMEOUT)


def idle_counting_rate():
    with CounterThread() as counter:
        before = counter.count
        start = time.perf_counter()
        time.sleep(0.2)
        duration = time.perf_counter() - start
        return (counter.count - before) / duration


def run_while_counting(name, call):
    """Run `call` and check the counter thread kept running meanwhile."""
    rate = idle_counting_rate()
    with CounterThread() as counter:
        before = counter.count
        start = time.perf_counter()
        result = call()
        duration = time.perf_counter() - start
        increments = counter.count - before
    if duration < MIN_CALL_DURATION:
        raise ValueError(f"[Test] {name} too fast ({duration}s) to be meaningful")
    if increments < MIN_RATE_FRACTION * rate * duration:
        raise ValueError(
            f"[Test] {name} blocked the other Python threads "
            f"({increments} increments in {duration}s, idle rate {rate}/s)"
        )
    return result


def big_grid3d(nb_cells):
    grid = mesh.RegularGrid3D.create()
    builder = mesh.RegularGridBuilder3D.create(grid)
    builder.initialize_cartesian_grid(
        geom.Point3D([0, 0, 0]), [nb_cells, nb_cells, nb_cells], 1.0
    )
    return grid


def big_point_set(nb_points):
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_vertices(nb_points - 1)
    return point_set


def small_triangulated_surface(nb_vertices):
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(nb_vertices)
    for v in range(nb_vertices):
        builder.set_point(v, geom.Point3D([v, v % 7, v % 13]))
    for v in range(nb_vertices - 2):
        builder.create_triangle([v, v + 1, v + 2])
    return surface


def check_distance_transform_releases_gil():
    grid = big_grid3d(250)
    run_while_counting(
        "euclidean_distance_transform3D",
        lambda: mesh.euclidean_distance_transform3D(
            grid, [[0, 0, 0]], "gil_edt"),
    )


def check_save_load_release_gil(directory):
    point_set = big_point_set(6000000)
    filename = os.path.join(directory, "big.og_pts3d")
    run_while_counting(
        "save_point_set3D",
        lambda: mesh.save_point_set3D(point_set, filename),
    )
    loaded = run_while_counting(
        "load_point_set3D",
        lambda: mesh.load_point_set3D(filename),
    )
    if len(loaded) != 1 or loaded[0].nb_vertices() != point_set.nb_vertices():
        raise ValueError("[Test] Wrong vertex count after released round trip")


def check_conversion_releases_gil():
    grid = big_grid3d(45)
    solid = run_while_counting(
        "convert_grid_into_tetrahedral_solid",
        lambda: mesh.convert_grid_into_tetrahedral_solid(grid),
    )
    if solid.nb_polyhedra() != 45 * 45 * 45 * 6:
        raise ValueError("[Test] Wrong converted solid")


def check_concurrent_loads(directory):
    surface = small_triangulated_surface(20000)
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

    threads = [threading.Thread(target=work, args=(i,)) for i in range(len(filenames))]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join(TIMEOUT)
    if errors:
        raise ValueError(f"[Test] Concurrent loads failed: {errors}")
    for result in results:
        if result is None or result[0].nb_vertices() != surface.nb_vertices():
            raise ValueError("[Test] Wrong concurrent load result")


def check_exception_translated_after_release(directory):
    missing = os.path.join(directory, "does_not_exist.og_tsf3d")
    for call in [
        lambda: mesh.load_triangulated_surface3D(missing),
        lambda: opengeode.load_graph("file.unknown_extension"),
    ]:
        try:
            call()
        except opengeode.OpenGeodeDataError as error:
            if error.type != "data" or not error.message:
                raise ValueError("[Test] Wrong exception translated after release")
            continue
        raise ValueError("[Test] The released call should raise OpenGeodeDataError")
    # The GIL must be held again: ordinary Python code still works
    if sum(range(10)) != 45:
        raise ValueError("[Test] Unreachable")


def check_loadable_queries_release_gil(directory):
    surface = small_triangulated_surface(10)
    filename = os.path.join(directory, "small.og_tsf3d")
    mesh.save_triangulated_surface3D(surface, filename)
    if mesh.is_triangulated_surface_loadable3D(filename).value() <= 0:
        raise ValueError("[Test] og_tsf3d should be loadable")
    if not mesh.is_triangulated_surface_saveable3D(surface, filename):
        raise ValueError("[Test] og_tsf3d should be saveable")
    if mesh.is_triangulated_surface_loadable3D("file.unknown_extension").value() > 0:
        raise ValueError("[Test] Unknown extension should not be loadable")


def check_bridge_delivers_during_released_call():
    """Task 5 bridge: a progress event emitted by a released call reaches
    the Python listener before that call returns."""
    Logger = opengeode.Logger
    ProgressLogger = opengeode.ProgressLogger
    message = "Compute 3D euclidian distance"
    start_received = threading.Event()
    finished = threading.Event()
    received_before_finish = []

    def listener(event):
        if event.message == message and event.kind == "start":
            received_before_finish.append(not finished.is_set())
            start_received.set()

    Logger.forward_to_python("opengeode.gil_test")
    ProgressLogger.add_listener(listener)
    try:

        grid = big_grid3d(300)

        def work():
            mesh.euclidean_distance_transform3D(grid, [[0, 0, 0]], "gil_bridge")
            finished.set()

        thread = threading.Thread(target=work)
        thread.start()
        if not start_received.wait(TIMEOUT):
            raise ValueError("[Test] Progress start event never forwarded")
        thread.join(TIMEOUT)
    finally:
        ProgressLogger.remove_listener(listener)
        Logger.stop_forwarding()
    if not received_before_finish or received_before_finish[0] is not True:
        raise ValueError(
            "[Test] The progress event was only delivered after the "
            "released call returned"
        )


if __name__ == "__main__":
    check_distance_transform_releases_gil()
    check_conversion_releases_gil()
    check_bridge_delivers_during_released_call()
    with tempfile.TemporaryDirectory() as directory:
        check_save_load_release_gil(directory)
        check_concurrent_loads(directory)
        check_exception_translated_after_release(directory)
        check_loadable_queries_release_gil(directory)
