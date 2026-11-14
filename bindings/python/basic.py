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
import functools as _functools
import logging as _logging
import threading as _threading
import traceback as _traceback
import typing as _typing

from opengeode_py_basic import *
OpenGeodeBasicLibrary.initialize()


# Unbounded type variable of the generic functions. pybind11-stubgen writes it
# as `_T: typing.TypeVar`: check_python_stubs.py fix rewrites it in the stubs
# as `_T = typing.TypeVar("_T")` (only plain TypeVars are supported).
_T = _typing.TypeVar("_T")


def _data_error(message: str) -> OpenGeodeDataError:
    """OpenGeodeDataError with the attributes of the errors raised by C++."""
    error = OpenGeodeDataError(message)
    error.type = "data"
    error.project = "OpenGeode"
    error.library = "Basic"
    error.message = message
    error.details = (
        "OpenGeodeException of type Data from project OpenGeode and library "
        f"Basic: {message}"
    )
    return error


def load_one(objects: "list[_T]", filename: str) -> "_T":
    """Return the only object of the list given by a load_* function.

    filename is only used in the error message. Raise an OpenGeodeDataError, as
    geode::load_one in C++, if the list does not hold exactly one object.
    """
    if len(objects) != 1:
        raise _data_error(
            f'[load_one] File "{filename}" holds {len(objects)} objects, '
            "expected exactly one"
        )
    return objects[0]


# ---------------------------------------------------------------------------
# Registry of the types handled by the generic functions (load, save,
# builder...). OpenGeode registers its types when its modules are imported;
# downstream libraries register theirs with register_type.
# ---------------------------------------------------------------------------


class _Registry:
    """Registration by class: {operation name: function, "builder": class},
    and the value types of the attributes by C++ type name.

    Not module-level dicts: pybind11-stubgen would write their content in the
    stubs (functions it cannot import, compiler-specific type names), and a
    module-level annotation makes Python 3.14 add a function to the stubs."""

    def __init__(self) -> None:
        self.registrations = {}
        self.attribute_types = {}


_registry = _Registry()

_REGISTRATION_DOC = """
    Every operation is optional; a generic function raises a TypeError for an
    operation that is not registered:
      load(filename) -> list of the cls objects of the file (load, load_all),
      save(obj, filename) -> list of the written files (save),
      is_loadable(filename) -> Percentage (is_loadable),
      is_saveable(obj, filename) -> bool (is_saveable of an object),
      input_extensions() / output_extensions() -> list of the extensions of the
        registered readers / writers (input_extensions, output_extensions,
        is_saveable of a class),
      additional_files(filename) -> AdditionalFiles (additional_files),
      builder: the builder class of cls (builder, builder_type), built with
        builder.create(obj) if it has a create method, else builder(obj),
      resolve(obj) -> obj as the class handling it, called first by the
        functions taking an object (save, is_saveable, builder,
        object_operation) for the instances of cls and of its subclasses: e.g.
        the mesh of a model component, typed SurfaceMesh3D, is resolved as a
        TriangulatedSurface3D (an object of the same class is unchanged),
      operations: other operations by name, for the generic functions of
        other libraries (type_operation, object_operation).
    Pass the bound functions (load_brep, BRepInputFactory.list_creators...):
    the generic functions only call them (the bound ones release the GIL).
    The registration is checked by type checkers (load(filename) must return a
    list of cls objects...). load and load_all are typed by cls for any
    registered type; the typed overloads of builder() are those of the types
    registered when the stubs are generated (the other types get Any).
"""


def _registration_entries(
    function: str,
    cls: object,
    operations: "dict[str, _typing.Any]",
    others: "dict[str, _typing.Any] | None",
) -> "dict[str, _typing.Any]":
    """Checked {name: operation} of a register_type or extend_type call."""
    if not isinstance(cls, type):
        raise TypeError(f"[{function}] {cls!r} is not a class")
    entries = {name: value for name, value in operations.items() if value is not None}
    for name, value in (others or {}).items():
        if not isinstance(name, str) or not name.isidentifier():
            raise TypeError(f"[{function}] Invalid operation name {name!r}")
        if name in operations:
            raise TypeError(
                f"[{function}] {name} of {cls.__name__} must be given as a keyword"
            )
        entries[name] = value
    for name, value in entries.items():
        if name == "builder":
            if not isinstance(value, type):
                raise TypeError(f"[{function}] builder of {cls.__name__} is not a class")
        elif not callable(value):
            raise TypeError(f"[{function}] {name} of {cls.__name__} is not callable")
    if not entries:
        raise TypeError(f"[{function}] No operation given for {cls.__name__}")
    return entries


def register_type(
    cls: "type[_T]",
    *,
    load: "_typing.Callable[[str], list[_T]] | None" = None,
    save: "_typing.Callable[[_T, str], list[str]] | None" = None,
    is_loadable: "_typing.Callable[[str], Percentage] | None" = None,
    is_saveable: "_typing.Callable[[_T, str], bool] | None" = None,
    input_extensions: "_typing.Callable[[], list[str]] | None" = None,
    output_extensions: "_typing.Callable[[], list[str]] | None" = None,
    additional_files: "_typing.Callable[[str], AdditionalFiles] | None" = None,
    builder: "type | None" = None,
    resolve: "_typing.Callable[[_T], object] | None" = None,
    operations: "dict[str, _typing.Callable[..., _typing.Any]] | None" = None,
) -> None:
    """Register cls for the generic functions: the extension point of the
    libraries adding their own types (OpenGeode registers its own ones when
    its modules are imported). Use extend_type to add operations to a
    registered type.
    {doc}
    Raise a ValueError if cls is already registered, a TypeError if cls or
    builder is not a class, an operation is not callable, a name of operations
    is invalid or a keyword, or no operation is given.
    """
    entries = _registration_entries(
        "register_type",
        cls,
        {
            "load": load,
            "save": save,
            "is_loadable": is_loadable,
            "is_saveable": is_saveable,
            "input_extensions": input_extensions,
            "output_extensions": output_extensions,
            "additional_files": additional_files,
            "builder": builder,
            "resolve": resolve,
        },
        operations,
    )
    if cls in _registry.registrations:
        raise ValueError(
            f"[register_type] {cls.__name__} is already registered (use extend_type)"
        )
    _registry.registrations[cls] = entries
    _update_builder_doc()


def extend_type(
    cls: "type[_T]",
    *,
    replace: bool = False,
    load: "_typing.Callable[[str], list[_T]] | None" = None,
    save: "_typing.Callable[[_T, str], list[str]] | None" = None,
    is_loadable: "_typing.Callable[[str], Percentage] | None" = None,
    is_saveable: "_typing.Callable[[_T, str], bool] | None" = None,
    input_extensions: "_typing.Callable[[], list[str]] | None" = None,
    output_extensions: "_typing.Callable[[], list[str]] | None" = None,
    additional_files: "_typing.Callable[[str], AdditionalFiles] | None" = None,
    builder: "type | None" = None,
    resolve: "_typing.Callable[[_T], object] | None" = None,
    operations: "dict[str, _typing.Callable[..., _typing.Any]] | None" = None,
) -> None:
    """Add operations to the registration of cls (see register_type), e.g. an
    operation of another library: extend_type(BRep, operations={"save_viewable":
    save_viewable_brep}).

    An operation already registered for cls is only replaced if replace is
    True. Raise a ValueError if cls is not registered or an operation is
    already registered (and replace is False), a TypeError as register_type
    for invalid operations. Nothing is changed when an error is raised.
    """
    entries = _registration_entries(
        "extend_type",
        cls,
        {
            "load": load,
            "save": save,
            "is_loadable": is_loadable,
            "is_saveable": is_saveable,
            "input_extensions": input_extensions,
            "output_extensions": output_extensions,
            "additional_files": additional_files,
            "builder": builder,
            "resolve": resolve,
        },
        operations,
    )
    registration = _registry.registrations.get(cls)
    if registration is None:
        raise ValueError(
            f"[extend_type] {cls.__name__} is not registered (use register_type)"
        )
    if not replace:
        existing = sorted(name for name in entries if name in registration)
        if existing:
            raise ValueError(
                f"[extend_type] {cls.__name__} already has {', '.join(existing)} "
                "(pass replace=True to replace)"
            )
    registration.update(entries)
    _update_builder_doc()


register_type.__doc__ = register_type.__doc__.replace("    {doc}\n", _REGISTRATION_DOC)


def registered_types() -> "list[type]":
    """Return the registered classes, in registration order."""
    return list(_registry.registrations)


def _type_name(cls: object) -> str:
    return cls.__name__ if isinstance(cls, type) else repr(cls)


def _unregistered(what: str) -> TypeError:
    names = ", ".join(sorted(cls.__name__ for cls in _registry.registrations))
    return TypeError(
        f"{what} is not a registered type (register_type). Registered types: {names}"
    )


def _class_operation(cls: type, operation: str) -> _typing.Any:
    """Operation registered for exactly cls."""
    registration = _registry.registrations.get(cls) if isinstance(cls, type) else None
    if registration is None:
        raise _unregistered(_type_name(cls))
    function = registration.get(operation)
    if function is None:
        raise TypeError(f"{cls.__name__} has no registered {operation}")
    return function


def _closest_registration(cls: type) -> "tuple[type, dict[str, _typing.Any]] | None":
    for base in cls.__mro__:
        registration = _registry.registrations.get(base)
        if registration is not None:
            return base, registration
    return None


def _resolve(obj: object) -> object:
    """obj as the class handling it: the resolve operation of the closest
    registered class of type(obj) that has one (applied once)."""
    for base in type(obj).__mro__:
        registration = _registry.registrations.get(base)
        if registration is not None and "resolve" in registration:
            return registration["resolve"](obj)
    return obj


def _object_operation(obj: object, operation: str) -> "tuple[object, _typing.Any]":
    """(resolved obj, operation of the closest registered class of its type)."""
    resolved = _resolve(obj)
    closest = _closest_registration(type(resolved))
    if closest is None:
        raise _unregistered(type(obj).__name__)
    cls, registration = closest
    function = registration.get(operation)
    if function is None:
        raise TypeError(f"{cls.__name__} has no registered {operation}")
    return resolved, function


def type_operation(cls: type, name: str) -> "_typing.Callable[..., _typing.Any]":
    """Return the operation called name registered for exactly cls (e.g.
    "load", or an operation added by another library). Raise a TypeError if
    cls is not registered or has no such operation."""
    return _class_operation(cls, name)


def object_operation(obj: object, name: str) -> "_typing.Callable[..., _typing.Any]":
    """Return the operation called name of obj, bound to it: the operation of
    the closest registered class of type(obj), obj being resolved first (see
    register_type), e.g. object_operation(mesh, "save")(filename). Raise a
    TypeError if no registered class of obj has such operation."""
    resolved, function = _object_operation(obj, name)
    return _functools.partial(function, resolved)


def _lowercase_extension(filename: str) -> str:
    """Extension used by the C++ factories (geode::detail::lowercase_extension):
    ASCII whitespace stripped, after the last dot of the file name, lowercase."""
    name = filename.strip(" \t\n\v\f\r")
    name = name[max(name.rfind("/"), name.rfind("\\")) + 1 :]
    dot = name.rfind(".")
    if dot <= 0:
        return ""
    return "".join(
        character.lower() if "A" <= character <= "Z" else character
        for character in name[dot + 1 :]
    )


def load_all(cls: "type[_T]", filename: str) -> "list[_T]":
    """Load every cls object of the file and return them as a list.

    The reader is selected by the file extension. Raise an OpenGeodeDataError
    if no reader handles the file or it cannot be read, a TypeError if cls has
    no registered load.
    """
    return list(_class_operation(cls, "load")(filename))


def load(cls: "type[_T]", filename: str) -> "_T":
    """Load the cls object of the file, e.g. load(TriangulatedSurface3D, path).

    The reader is selected by the file extension. Raise an OpenGeodeDataError
    if no reader handles the file, it cannot be read or it does not hold
    exactly one object (use load_all then), a TypeError if cls has no
    registered load.
    """
    return load_one(load_all(cls, filename), filename)


def save(obj: object, filename: str) -> "list[str]":
    """Save obj in the file, whose extension selects the writer, and return
    the list of the written files.

    The writer is the one of the closest registered class of type(obj), obj
    being resolved first (e.g. the SurfaceMesh3D of a model component is saved
    as its TriangulatedSurface3D or PolygonalSurface3D). Raise an
    OpenGeodeDataError if no writer handles the extension (as load does), an
    OpenGeodeInternalError if the writing itself fails, a TypeError if the
    type of obj has no registered save.
    """
    resolved, function = _object_operation(obj, "save")
    return list(function(resolved, filename))


def is_loadable(cls: type, filename: str) -> Percentage:
    """Return a Percentage of how well the file can be loaded as a cls object,
    0 if it cannot. Raise a TypeError if cls has no registered is_loadable."""
    return _class_operation(cls, "is_loadable")(filename)


def is_saveable(obj_or_cls: object, filename: str) -> bool:
    """Return True if the writer of the file extension can save the object.

    For an object, the writer is asked (it may refuse a given object): the
    type of obj (resolved first, see save) needs a registered is_saveable.
    For a class, return True if a
    writer is registered for the extension (case insensitive): cls needs a
    registered output_extensions. Raise a TypeError otherwise.
    """
    if isinstance(obj_or_cls, type):
        extensions = _class_operation(obj_or_cls, "output_extensions")()
        return _lowercase_extension(filename) in extensions
    resolved, function = _object_operation(obj_or_cls, "is_saveable")
    return bool(function(resolved, filename))


def input_extensions(cls: type) -> "list[str]":
    """Return the extensions of the registered readers of cls (other formats
    may be added by IO libraries once imported). Raise a TypeError if cls has
    no registered input_extensions."""
    return list(_class_operation(cls, "input_extensions")())


def output_extensions(cls: type) -> "list[str]":
    """Return the extensions of the registered writers of cls (other formats
    may be added by IO libraries once imported). Raise a TypeError if cls has
    no registered output_extensions."""
    return list(_class_operation(cls, "output_extensions")())


def additional_files(cls: type, filename: str) -> AdditionalFiles:
    """Return the additional files used when loading the file as a cls object.
    Raise an OpenGeodeDataError if no reader handles the file, a TypeError if
    cls has no registered additional_files."""
    return _class_operation(cls, "additional_files")(filename)


def builder_type(cls: type) -> "type | None":
    """Return the builder class registered for the closest registered class
    of cls, or None if it has none. builder() may return a more derived
    builder: it resolves the object first (e.g. a SurfaceMesh3D that is a
    triangulated surface gets a TriangulatedSurfaceBuilder3D). Raise a
    TypeError if neither cls nor any of its bases is registered."""
    if not isinstance(cls, type):
        raise _unregistered(_type_name(cls))
    closest = _closest_registration(cls)
    if closest is None:
        raise _unregistered(cls.__name__)
    return closest[1].get("builder")


def _builder(obj: object) -> _typing.Any:
    resolved, builder_class = _object_operation(obj, "builder")
    create = getattr(builder_class, "create", None)
    if create is not None:
        return create(resolved)
    return builder_class(resolved)


def builder(*args, **kwargs):
    # Documentation set by _update_builder_doc. pybind11-stubgen reads the
    # typed overloads in it because the signature (inspect.getfullargspec) is
    # (*args, **kwargs); inspect.signature follows __wrapped__ and gives the
    # runtime signature (obj: object) -> Any.
    return _builder(*args, **kwargs)


builder.__wrapped__ = _builder


_BUILDER_DOC = """Return the builder modifying obj.

The builder class is the one registered for the closest registered class of
type(obj), obj being resolved first (see save; builder_type gives the class
without resolution): the builder is builder_class.create(obj) if the
class has a create method (mesh builders), else builder_class(obj) (model
builders). Raise a TypeError if no builder is registered for type(obj)."""


def _qualified_name(cls: type) -> str:
    return f"{cls.__module__}.{cls.__qualname__}"


def _update_builder_doc() -> None:
    # Written as a pybind11 overloaded function: pybind11-stubgen reads the
    # signatures of a (*args, **kwargs) function in its docstring, which gives
    # one typed overload per registered builder (most derived classes first)
    entries = [
        (cls, registration["builder"])
        for cls, registration in _registry.registrations.items()
        if "builder" in registration
    ]
    entries.sort(key=lambda entry: -len(entry[0].__mro__))
    signatures = [
        (
            f"builder(obj: {_qualified_name(cls)}) -> {_qualified_name(builder_class)}",
            f"Return the {builder_class.__name__} modifying the {cls.__name__}",
        )
        for cls, builder_class in entries
    ]
    signatures.append(("builder(obj: object) -> typing.Any", _BUILDER_DOC))
    if len(signatures) == 1:
        builder.__doc__ = f"{signatures[0][0]}\n\n{signatures[0][1]}\n"
        return
    lines = ["builder(*args, **kwargs)", "Overloaded function.", ""]
    for number, (signature, doc) in enumerate(signatures, 1):
        lines += [f"{number}. {signature}", "", doc, ""]
    builder.__doc__ = "\n".join(lines)


_update_builder_doc()


# ---------------------------------------------------------------------------
# Attributes by name
# ---------------------------------------------------------------------------

# Suffix of the typed AttributeManager functions (find_attribute_variable_double...)
# and of the attribute classes (VariableAttributeDouble...), by value type.
_ATTRIBUTE_SUFFIXES = {
    "bool": ("Bool", True),
    "int": ("Int", 0),
    "uint": ("UInt", 0),
    "float": ("Float", 0.0),
    "double": ("Double", 0.0),
    "arraydouble2": ("ArrayDouble2", [0.0, 0.0]),
    "arraydouble3": ("ArrayDouble3", [0.0, 0.0, 0.0]),
}


def _attribute_type_names() -> "dict[str, tuple[str, str]]":
    # attribute_type() is the C++ typeid name, which depends on the compiler
    # (e.g. "d" or "double"): learn it from one attribute of each value type
    manager = AttributeManager()
    names = {}
    for suffix, (class_suffix, value) in _ATTRIBUTE_SUFFIXES.items():
        attribute_id = getattr(manager, f"create_attribute_constant_{suffix}")(
            "probe_" + suffix, value
        )
        names[manager.attribute_type(attribute_id)] = (suffix, class_suffix)
    return names


_registry.attribute_types = _attribute_type_names()


def find_attribute(manager: AttributeManager, name: str) -> AttributeBase:
    """Return the attribute called name of the manager, as its typed class
    (e.g. VariableAttributeDouble, SparseAttributeArrayDouble3).

    The value type is given by manager.attribute_type and the kind (constant,
    variable or sparse) by the attribute itself; the attribute is then found
    with the matching find_attribute_<kind>_<type> function. An attribute of
    another kind is returned by find_read_only_attribute_<type>, an attribute
    of another value type by find_generic_attribute. Raise an
    OpenGeodeDataError if no attribute or several attributes (steps of a time
    series: use time_steps) are called name.
    """
    attribute_ids = manager.attribute_ids_matching_name(name)
    if not attribute_ids:
        raise _data_error(f'[find_attribute] No attribute called "{name}"')
    if len(attribute_ids) != 1:
        raise _data_error(
            f'[find_attribute] {len(attribute_ids)} attributes are called "{name}" '
            "(steps of a time series): use time_steps to find them"
        )
    attribute_id = attribute_ids[0]
    attribute = manager.find_generic_attribute(attribute_id)
    names = _registry.attribute_types.get(manager.attribute_type(attribute_id))
    if names is None:
        return attribute
    suffix, class_suffix = names
    for kind in ("constant", "variable", "sparse"):
        attribute_class = globals()[f"{kind.capitalize()}Attribute{class_suffix}"]
        if isinstance(attribute, attribute_class):
            return getattr(manager, f"find_attribute_{kind}_{suffix}")(attribute_id)
    return getattr(manager, f"find_read_only_attribute_{suffix}")(attribute_id)

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
        self._stopped = False

    def forward(self, logger_name):
        with self._lock:
            # Once stopped at interpreter exit, nothing drains the events: the
            # console output stays as it is
            if self._logger is None and not self._stopped:
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
            if not self._stopped:
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
            self._stopped = True
            thread = self._thread
        # The thread must end before the interpreter finalization: a daemon
        # thread taking the GIL back in C++ code (Logger._wait) during the
        # finalization is killed by CPython < 3.14, which aborts the process
        Logger._stop_waiting()
        if thread is not None:
            thread.join(self._WAIT_TIMEOUT)

    def _configure(self):
        if self._stopped:
            # Without the thread, queued events would never be dispatched
            Logger._set_bridge(False, False)
            return
        Logger._set_bridge(self._logger is not None, bool(self._listeners))
        if (
            self._thread is None
            and not self._stopped
            and (self._logger is not None or self._listeners)
        ):
            # Idle, it only waits on the C++ queue without the GIL; stopped
            # at interpreter exit
            self._thread = _threading.Thread(
                target=self._run, name="opengeode-log-bridge", daemon=True
            )
            self._thread.start()
            _atexit.register(self._shutdown)

    def _run(self):
        while not self._stopped:
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


def forward_logs_to_python(logger_name: str = "opengeode") -> None:
    """Send the OpenGeode messages to logging.getLogger(logger_name) instead
    of the console. OpenGeode levels map to the logging ones, trace being
    TRACE (5). Calling it again only changes the target logger. Once the
    forwarding stopped at interpreter exit (from a later atexit handler), it
    has no effect: the messages stay on the console."""
    _log_bridge.forward(logger_name)


def stop_forwarding_logs() -> None:
    """Stop sending the OpenGeode messages to Python and restore the console
    output. Does nothing if the messages are not forwarded."""
    _log_bridge.stop_forwarding()


def add_progress_listener(listener: "_typing.Callable[[ProgressEvent], object]") -> None:
    """Call listener(ProgressEvent) for each progress event, from a Python
    daemon thread. Adding the same listener twice has no effect."""
    _log_bridge.add_listener(listener)


def remove_progress_listener(listener: "_typing.Callable[[ProgressEvent], object]") -> None:
    """Stop calling listener. Does nothing if it is not a listener."""
    _log_bridge.remove_listener(listener)
