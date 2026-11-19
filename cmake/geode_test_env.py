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

# Imported by the Python tests, before any binding, to find the DLLs they need.
# Since Python 3.8, Windows does not search the PATH for the DLLs of an extension
# module: only the directories of the test dependencies are added, given by
# add_geode_python_test in GEODE_DLL_DIRECTORIES. Adding the whole PATH would let
# DLLs of other software (an older MSVC runtime, for instance) be loaded first.

import os
import platform
import sys
import threading
import time

if platform.system() == "Windows":
    for directory in os.environ.get("GEODE_DLL_DIRECTORIES", "").split(os.pathsep):
        if directory:
            os.add_dll_directory(directory)


# Deterministic check that a bound call releases the GIL.
#
# The interpreter switch interval is set huge, so a thread never gives the GIL
# away unless it blocks or exits. A worker thread repeats the call until the
# main thread has run, or until the deadline. The main thread, once the worker
# started, only needs the GIL to set an event: if the call releases the GIL,
# the main thread gets it while the call runs (or between two calls) whatever
# the machine load and the duration of the call; if the call holds the GIL,
# the main thread cannot run until the worker is done. No retry, no workload
# tuning: the call just has to release the GIL once.


def _repeat_until_event(call, event, deadline, name, set_from_main):
    previous = sys.getswitchinterval()
    sys.setswitchinterval(1000)
    try:
        ready = threading.Event()
        box = {}

        def target():
            ready.set()
            end = time.monotonic() + deadline
            try:
                while not event.is_set() and time.monotonic() < end:
                    box["result"] = call()
                # Sampled by the worker itself: the main thread may run (and
                # set the event) right after the worker is done
                box["event_seen"] = event.is_set()
            except BaseException as error:
                box["error"] = error

        thread = threading.Thread(target=target, daemon=True)
        thread.start()
        if not ready.wait(deadline + 60):
            raise ValueError(f"[Test] Worker thread of {name} did not start")
        if set_from_main:
            event.set()
        thread.join(deadline + 60)
        if thread.is_alive():
            raise ValueError(f"[Test] {name} did not return after the deadline")
    finally:
        sys.setswitchinterval(previous)
    if "error" in box:
        raise box["error"]
    return box.get("event_seen", False), box.get("result")


def assert_gil_released(call, *, deadline=30.0, name):
    """Fail unless `call` (a callable without argument) releases the GIL.

    `call` runs repeatedly in a worker thread, under a huge switch interval,
    until the main thread has run or `deadline` seconds elapsed; a call that
    releases the GIL lets the main thread run, so this is robust under load,
    and `call` can be as short as wanted. Returns the result of the
    last call. Raises ValueError, naming `name`, if the main thread never ran
    (the call held the GIL), and re-raises what `call` raised. `deadline` only
    bounds the time of a failure: a released call returns as soon as the main
    thread ran."""
    event = threading.Event()
    seen, result = _repeat_until_event(call, event, deadline, name, True)
    if not seen:
        raise ValueError(
            f"[Test] {name} kept the GIL: the main thread could not run "
            f"during {deadline} s of repeated calls"
        )
    return result


def assert_event_during_calls(call, event, *, name, deadline=30.0):
    """Like assert_gil_released, but the awaited proof is `event` (a
    threading.Event set by Python code that needs the GIL, such as a listener
    called from C++ while `call` runs) instead of the main thread running."""
    seen, result = _repeat_until_event(call, event, deadline, name, False)
    if not seen:
        raise ValueError(
            f"[Test] {name} kept the GIL: the event was not set during "
            f"{deadline} s of repeated calls"
        )
    return result
