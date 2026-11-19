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

# Self-test of geode_test_env.assert_gil_released (and its event variant),
# under full CPU load: a GIL-holding call must fail, a released one must pass.

import os
import subprocess
import sys
import threading
import time

import geode_test_env


def holds_gil():
    # Pure Python: never releases the GIL
    total = 0
    for i in range(200000):
        total += i
    return total


def releases_gil():
    time.sleep(0.005)
    return "released"


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_holding_call_fails():
    try:
        geode_test_env.assert_gil_released(
            holds_gil, deadline=1.0, name="holds_gil"
        )
    except ValueError as error:
        check("holds_gil kept the GIL" in str(error), f"Wrong message: {error}")
        return
    raise ValueError("[Test] A GIL-holding call should fail")


def check_released_call_passes():
    result = geode_test_env.assert_gil_released(
        releases_gil, deadline=30, name="releases_gil"
    )
    check(result == "released", "The result of the call should be returned")


def check_call_error_propagates():
    def failing():
        raise KeyError("boom")

    try:
        geode_test_env.assert_gil_released(failing, deadline=1.0, name="failing")
    except KeyError:
        return
    raise ValueError("[Test] The error of the call should propagate")


def check_event_variant():
    # A thread waiting for the call to start, then setting the event: like a
    # Python listener called from C++, it needs the GIL to proceed
    for call_body, name, passes in [
        (releases_gil, "releasing_listener", True),
        (holds_gil, "holding_listener", False),
    ]:
        started = threading.Event()
        event = threading.Event()

        def listener():
            started.wait()
            event.set()

        thread = threading.Thread(target=listener)
        thread.start()

        def call():
            started.set()
            call_body()

        try:
            geode_test_env.assert_event_during_calls(
                call, event, deadline=30 if passes else 1.0, name=name
            )
        except ValueError as error:
            check(not passes, f"The released call should pass: {error}")
            check(f"{name} kept the GIL" in str(error), str(error))
        else:
            check(passes, "The event cannot be set by a GIL-holding call")
        thread.join()


if __name__ == "__main__":
    busy = [
        subprocess.Popen([sys.executable, "-c", "while True: pass"])
        for _ in range(os.cpu_count())
    ]
    try:
        check_holding_call_fails()
        check_released_call_passes()
        check_call_error_propagates()
        check_event_variant()
    finally:
        for process in busy:
            process.kill()
        for process in busy:
            process.wait()
