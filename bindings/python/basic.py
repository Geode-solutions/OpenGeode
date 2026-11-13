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

import atexit as _atexit
import collections as _collections
import logging as _logging
import threading as _threading
import traceback as _traceback
import typing as _typing

from opengeode_py_basic import *
OpenGeodeBasicLibrary.initialize()


def load_one(objects: list, filename: str) -> _typing.Any:
    """Return the only object of the list given by a load_* function.

    filename is only used in the error message. Raise a RuntimeError if the
    list does not hold exactly one object.
    """
    if len(objects) != 1:
        raise RuntimeError(
            f'[load_one] File "{filename}" holds {len(objects)} objects, expected exactly one'
        )
    return objects[0]


TRACE = 5
if _logging.getLevelName(TRACE) == f"Level {TRACE}":
    _logging.addLevelName(TRACE, "TRACE")

_PYTHON_LEVELS = {
    Logger.Level.trace: TRACE,
    Logger.Level.debug: _logging.DEBUG,
    Logger.Level.info: _logging.INFO,
    Logger.Level.warning: _logging.WARNING,
    Logger.Level.error: _logging.ERROR,
    Logger.Level.critical: _logging.CRITICAL,
}

ProgressEvent = _collections.namedtuple(
    "ProgressEvent",
    ["id", "kind", "level", "message", "current_step", "nb_steps", "timestamp"],
)
ProgressEvent.__doc__ = """Progress event given to the ProgressLogger listeners.

kind is "start", "update", "completed" or "failed"; message is the one given
when the progress started; current_step and nb_steps are 0 when unknown.
"""


class _LogBridge:
    """Forward the C++ log and progress events to Python.

    The C++ clients only queue the events. A daemon thread waits for them
    without the GIL (Logger._wait) and dispatches them to Python, so no
    Python code ever runs on a C++ worker thread.
    """

    _WAIT_TIMEOUT = 1.0

    def __init__(self):
        self._lock = _threading.RLock()
        self._logger = None
        self._console_output = True
        self._listeners = []
        self._thread = None

    def forward(self, logger_name):
        with self._lock:
            if self._logger is None:
                self._console_output = Logger._is_console_output_enabled()
                Logger._set_console_output(False)
            self._logger = _logging.getLogger(logger_name)
            self._configure()

    def stop_forwarding(self):
        with self._lock:
            if self._logger is None:
                return
            self._flush()
            self._logger = None
            self._configure()
            Logger._set_console_output(self._console_output)

    def add_listener(self, listener):
        with self._lock:
            if listener not in self._listeners:
                self._listeners.append(listener)
            self._configure()

    def remove_listener(self, listener):
        with self._lock:
            if listener in self._listeners:
                self._flush()
                self._listeners.remove(listener)
                self._configure()

    def _shutdown(self):
        with self._lock:
            self._flush()
            self.stop_forwarding()
            self._listeners.clear()
            self._configure()

    def _configure(self):
        Logger._set_bridge(self._logger is not None, bool(self._listeners))
        if self._thread is None and (self._logger is not None or self._listeners):
            # Never stopped: idle, it only waits on the C++ queue without the GIL
            self._thread = _threading.Thread(
                target=self._run, name="opengeode-log-bridge", daemon=True
            )
            self._thread.start()
            _atexit.register(self._shutdown)

    def _run(self):
        while True:
            if Logger._wait(self._WAIT_TIMEOUT):
                # Events are only taken under the lock: stop_forwarding and
                # remove_listener flush every event queued before them
                with self._lock:
                    self._flush()

    def _flush(self):
        self._dispatch(Logger._drain(0))

    def _dispatch(self, events):
        for event in events:
            # A raising handler, filter or listener must not stop the bridge
            try:
                if event[0] == "log":
                    self._dispatch_log(*event[1:])
                else:
                    self._dispatch_progress(ProgressEvent(*event[1:]))
            except Exception:
                _traceback.print_exc()

    def _dispatch_log(self, level, message, timestamp):
        logger = self._logger
        if logger is None:
            return
        python_level = _PYTHON_LEVELS[level]
        if not logger.isEnabledFor(python_level):
            return
        record = logger.makeRecord(
            logger.name, python_level, "opengeode", 0, message, None, None
        )
        record.created = timestamp
        record.msecs = (timestamp - int(timestamp)) * 1000
        logger.handle(record)

    def _dispatch_progress(self, event):
        for listener in list(self._listeners):
            try:
                listener(event)
            except Exception:
                _traceback.print_exc()


_log_bridge = _LogBridge()


def _forward_to_python(logger_name: str = "opengeode") -> None:
    """Send the OpenGeode messages to logging.getLogger(logger_name) instead
    of the console. OpenGeode levels map to the logging ones, trace being
    TRACE (5). Calling it again only changes the target logger."""
    _log_bridge.forward(logger_name)


def _stop_forwarding() -> None:
    """Stop sending the OpenGeode messages to Python and restore the console
    output. Does nothing if the messages are not forwarded."""
    _log_bridge.stop_forwarding()


def _add_listener(listener: _typing.Callable[[ProgressEvent], object]) -> None:
    """Call listener(ProgressEvent) for each progress event, from a Python
    daemon thread. Adding the same listener twice has no effect."""
    _log_bridge.add_listener(listener)


def _remove_listener(listener: _typing.Callable[[ProgressEvent], object]) -> None:
    """Stop calling listener. Does nothing if it is not a listener."""
    _log_bridge.remove_listener(listener)


Logger.forward_to_python = staticmethod(_forward_to_python)
Logger.stop_forwarding = staticmethod(_stop_forwarding)
ProgressLogger.add_listener = staticmethod(_add_listener)
ProgressLogger.remove_listener = staticmethod(_remove_listener)
