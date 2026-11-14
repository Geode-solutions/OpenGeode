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

import inspect
import logging
import subprocess
import sys
import threading
import time

import geode_test_env

import basic as opengeode

TIMEOUT = 30


class RecordingHandler(logging.Handler):
    def __init__(self):
        super().__init__(level=1)
        self.records = []
        self.condition = threading.Condition()

    def emit(self, record):
        with self.condition:
            self.records.append(record)
            self.condition.notify_all()

    def messages(self):
        with self.condition:
            return [record.getMessage() for record in self.records]

    def wait_for(self, message):
        with self.condition:
            if not self.condition.wait_for(
                lambda: message in [record.getMessage() for record in self.records],
                TIMEOUT,
            ):
                raise ValueError(f"[Test] Message '{message}' not forwarded")
            return [r for r in self.records if r.getMessage() == message][-1]


def check_log_forwarding(Logger):
    python_logger = logging.getLogger("opengeode.test")
    python_logger.setLevel(1)
    python_logger.propagate = False
    handler = RecordingHandler()
    python_logger.addHandler(handler)
    if not Logger._is_console_output_enabled():
        raise ValueError("[Test] Console output should be enabled by default")
    opengeode.forward_logs_to_python("opengeode.test")
    opengeode.forward_logs_to_python("opengeode.test")
    if Logger._is_console_output_enabled():
        raise ValueError("[Test] Console output should be disabled when forwarding")
    initial_level = Logger.level()
    Logger.set_level(Logger.Level.trace)
    Logger.trace("bridge trace")
    Logger.debug("bridge debug")
    Logger.info("bridge info")
    Logger.warning("bridge warning")
    Logger.error("bridge error")
    Logger.critical("bridge critical")
    Logger.set_level(initial_level)
    expected = {
        "bridge trace": 5,
        "bridge debug": logging.DEBUG,
        "bridge info": logging.INFO,
        "bridge warning": logging.WARNING,
        "bridge error": logging.ERROR,
        "bridge critical": logging.CRITICAL,
    }
    for message, level in expected.items():
        record = handler.wait_for(message)
        if record.levelno != level:
            raise ValueError(f"[Test] Wrong level for '{message}': {record.levelno}")
        if record.name != "opengeode.test":
            raise ValueError("[Test] Wrong Python logger name")
        if logging.getLevelName(5) != "TRACE":
            raise ValueError("[Test] TRACE level name should be registered")
        if abs(record.created - time.time()) > TIMEOUT:
            raise ValueError("[Test] Wrong record timestamp")
    if handler.messages().count("bridge info") != 1:
        raise ValueError("[Test] Message forwarded several times")
    return handler


def check_concurrent_logging(Logger, handler):
    nb_threads = 4
    nb_messages = 200

    def log_messages(thread_id):
        for message_id in range(nb_messages):
            Logger.info(f"thread {thread_id} message {message_id}")

    threads = [
        threading.Thread(target=log_messages, args=(thread_id,))
        for thread_id in range(nb_threads)
    ]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    for thread_id in range(nb_threads):
        handler.wait_for(f"thread {thread_id} message {nb_messages - 1}")
    messages = set(handler.messages())
    for thread_id in range(nb_threads):
        for message_id in range(nb_messages):
            if f"thread {thread_id} message {message_id}" not in messages:
                raise ValueError("[Test] Missing message from a logging thread")


def check_cpp_worker_threads(Logger, handler):
    nb_threads = 4
    nb_messages = 100
    Logger._log_from_threads(Logger.Level.warning, "worker", nb_threads, nb_messages)
    for thread_id in range(nb_threads):
        handler.wait_for(f"worker {thread_id} {nb_messages - 1}")
    messages = set(handler.messages())
    for thread_id in range(nb_threads):
        for message_id in range(nb_messages):
            if f"worker {thread_id} {message_id}" not in messages:
                raise ValueError("[Test] Missing message from a C++ worker thread")


def check_progress_listener(Logger):
    ProgressLogger = opengeode.ProgressLogger
    events = []
    condition = threading.Condition()

    def listener(event):
        with condition:
            events.append(event)
            condition.notify_all()

    def wait_for_kinds(message, kinds):
        def matching():
            return [event.kind for event in events if event.message == message]

        with condition:
            if not condition.wait_for(lambda: matching() == kinds, TIMEOUT):
                raise ValueError(
                    f"[Test] Wrong progress events for '{message}': {matching()}"
                )
            return [event for event in events if event.message == message]

    opengeode.add_progress_listener(listener)
    opengeode.add_progress_listener(listener)
    progress = ProgressLogger(Logger.Level.info, "bridge progress", 2)
    progress.set_refresh_interval(0)
    # The first update is sent at least one second after the start
    time.sleep(1.1)
    progress.increment()
    progress.increment()
    del progress
    completed = wait_for_kinds(
        "bridge progress", ["start", "update", "update", "completed"]
    )
    if completed[0].nb_steps != 2 or completed[2].current_step != 2:
        raise ValueError("[Test] Wrong progress steps")
    if completed[0].level != Logger.Level.info:
        raise ValueError("[Test] Wrong progress level")
    if len({event.id for event in completed}) != 1:
        raise ValueError("[Test] Progress events should share the same id")

    failed = ProgressLogger(Logger.Level.info, "bridge failed progress", 2)
    del failed
    wait_for_kinds("bridge failed progress", ["start", "failed"])

    opengeode.remove_progress_listener(listener)
    opengeode.remove_progress_listener(listener)
    progress = ProgressLogger(Logger.Level.info, "unheard progress", 1)
    progress.increment()
    del progress


EXIT_SCRIPT = """
import logging
import sys

import basic as opengeode

logging.basicConfig(level=1, stream=sys.stdout, format="%(message)s")
opengeode.forward_logs_to_python()
opengeode.add_progress_listener(lambda event: print("listener", event.kind))
progress = opengeode.ProgressLogger(opengeode.Logger.Level.info, "exit progress", 1)
progress.increment()
del progress
opengeode.Logger.info("message before exit")
"""


def check_flush_at_exit():
    # The events still queued at interpreter exit are flushed, without hanging
    expected = [
        "exit progress started",
        "listener start",
        "listener completed",
        "message before exit",
    ]
    for _ in range(10):
        result = subprocess.run(
            [sys.executable, "-c", EXIT_SCRIPT],
            capture_output=True,
            text=True,
            timeout=TIMEOUT,
        )
        if result.returncode != 0:
            raise ValueError(f"[Test] Exit script failed: {result.stderr}")
        for message in expected:
            if message not in result.stdout:
                raise ValueError(
                    f"[Test] '{message}' not flushed at exit: {result.stdout}"
                )


SLOW_EXIT_SCRIPT = """
import logging
import time

import basic as opengeode

logging.getLogger("opengeode").addHandler(logging.NullHandler())
opengeode.forward_logs_to_python()
opengeode.add_progress_listener(lambda event: None)
for index in range(100):
    opengeode.Logger.info("message %d" % index)


class SlowFinalizer:
    # Destroyed during the interpreter finalization: it releases the GIL
    # longer than the bridge thread waits for events, so that a running
    # bridge thread would wake up and take the GIL back during the
    # finalization
    def __del__(self, sleep=time.sleep, delay=2 * opengeode._log_bridge._WAIT_TIMEOUT):
        sleep(delay)


finalizer = SlowFinalizer()
"""


def check_slow_exit():
    # The bridge thread is stopped before the interpreter finalization: a
    # daemon thread taking the GIL back in C++ code (Logger._wait) during the
    # finalization is killed by CPython < 3.14, which aborts the process
    # (std::terminate)
    for run in range(3):
        result = subprocess.run(
            [sys.executable, "-c", SLOW_EXIT_SCRIPT],
            capture_output=True,
            text=True,
            timeout=TIMEOUT,
        )
        if result.returncode != 0 or "terminate" in result.stderr:
            raise ValueError(
                f"[Test] Slow exit {run} failed ({result.returncode}): {result.stderr}"
            )


def check_module_functions():
    # Module functions, typed in the generated stubs (attributes added from
    # Python to the pybind11 classes were invisible to type checkers)
    for name in [
        "forward_logs_to_python",
        "stop_forwarding_logs",
        "add_progress_listener",
        "remove_progress_listener",
    ]:
        function = getattr(opengeode, name)
        signature = inspect.signature(function)
        if signature.return_annotation is not None or any(
            parameter.annotation is inspect.Parameter.empty
            for parameter in signature.parameters.values()
        ):
            raise ValueError(f"[Test] {name} should be fully annotated")
    for owner, name in [
        (opengeode.Logger, "forward_to_python"),
        (opengeode.Logger, "stop_forwarding"),
        (opengeode.ProgressLogger, "add_listener"),
        (opengeode.ProgressLogger, "remove_listener"),
    ]:
        if hasattr(owner, name):
            raise ValueError(f"[Test] {owner.__name__}.{name} should be removed")


def check_namespace():
    for name in ["atexit", "collections", "logging", "threading", "traceback"]:
        if hasattr(opengeode, name):
            raise ValueError(f"[Test] '{name}' should not leak into opengeode")


def add_test_handler(logger_name):
    python_logger = logging.getLogger(logger_name)
    python_logger.setLevel(1)
    python_logger.propagate = False
    handler = RecordingHandler()
    python_logger.addHandler(handler)
    return python_logger, handler


def check_retarget(Logger, handler):
    _, other_handler = add_test_handler("opengeode.other")
    opengeode.forward_logs_to_python("opengeode.other")
    Logger.info("bridge retargeted")
    other_handler.wait_for("bridge retargeted")
    opengeode.forward_logs_to_python("opengeode.test")
    Logger.info("bridge back")
    handler.wait_for("bridge back")
    if "bridge retargeted" in handler.messages():
        raise ValueError("[Test] Message sent to the previous logger")
    if "bridge back" in other_handler.messages():
        raise ValueError("[Test] Message sent to the previous logger")


def check_python_level_filter(Logger, handler):
    python_logger = logging.getLogger("opengeode.test")
    python_logger.setLevel(logging.WARNING)
    Logger.info("bridge filtered info")
    Logger.warning("bridge kept warning")
    handler.wait_for("bridge kept warning")
    python_logger.setLevel(1)
    if "bridge filtered info" in handler.messages():
        raise ValueError("[Test] Message below the Python level should be dropped")


def check_raising_filter(Logger, handler):
    def raising_filter(record):
        if record.getMessage() == "bridge boom":
            raise RuntimeError("[Test] Expected filter error")
        return True

    python_logger = logging.getLogger("opengeode.test")
    python_logger.addFilter(raising_filter)
    Logger.info("bridge boom")
    Logger.info("bridge after boom")
    handler.wait_for("bridge after boom")
    python_logger.removeFilter(raising_filter)


def check_raising_listener(Logger):
    ProgressLogger = opengeode.ProgressLogger
    events = []
    condition = threading.Condition()

    def raising_listener(event):
        raise RuntimeError("[Test] Expected listener error")

    def listener(event):
        with condition:
            events.append(event)
            condition.notify_all()

    opengeode.add_progress_listener(raising_listener)
    opengeode.add_progress_listener(listener)
    for message in ["raising progress", "next raising progress"]:
        progress = ProgressLogger(Logger.Level.info, message, 1)
        progress.increment()
        del progress
    with condition:
        if not condition.wait_for(
            lambda: [event.message for event in events if event.kind == "completed"]
            == ["raising progress", "next raising progress"],
            TIMEOUT,
        ):
            raise ValueError("[Test] Events lost after a raising listener")
    opengeode.remove_progress_listener(raising_listener)
    opengeode.remove_progress_listener(listener)


def check_preserved_console_state(Logger):
    opengeode.stop_forwarding_logs()
    Logger._set_console_output(False)
    opengeode.forward_logs_to_python("opengeode.test")
    opengeode.stop_forwarding_logs()
    if Logger._is_console_output_enabled():
        raise ValueError("[Test] Disabled console output should stay disabled")
    Logger._set_console_output(True)


def check_stop_forwarding(Logger, handler):
    opengeode.stop_forwarding_logs()
    opengeode.stop_forwarding_logs()
    if not Logger._is_console_output_enabled():
        raise ValueError("[Test] Console output should be restored")
    Logger.info("bridge stopped")
    time.sleep(0.5)
    if "bridge stopped" in handler.messages():
        raise ValueError("[Test] Message forwarded after stop_forwarding")
    opengeode.forward_logs_to_python("opengeode.test")
    Logger.info("bridge restarted")
    handler.wait_for("bridge restarted")


if __name__ == "__main__":
    Logger = opengeode.Logger
    handler = check_log_forwarding(Logger)
    check_concurrent_logging(Logger, handler)
    check_cpp_worker_threads(Logger, handler)
    check_progress_listener(Logger)
    check_namespace()
    check_module_functions()
    check_retarget(Logger, handler)
    check_python_level_filter(Logger, handler)
    check_raising_filter(Logger, handler)
    check_raising_listener(Logger)
    check_flush_at_exit()
    check_slow_exit()
    check_stop_forwarding(Logger, handler)
    check_preserved_console_state(Logger)
    opengeode.forward_logs_to_python("opengeode.test")
    # Forwarding still active: interpreter exit must flush and not hang
    Logger.info("bridge exit")
