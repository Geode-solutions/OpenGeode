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


# Tests of the stub quality gates (cmake/check_python_stubs.py) on synthetic
# stubs: the gates must fail on what they are meant to reject, and only on it.
# The script path is given as first argument.

import os
import subprocess
import sys
import tempfile
import textwrap

CHECKER = os.path.abspath(sys.argv[1])
MYPY_CONFIG = os.path.join(os.path.dirname(CHECKER), "stubtest_mypy.ini")


def run(arguments, environment=None):
    result = subprocess.run(
        [sys.executable, CHECKER] + arguments,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    return result.returncode, result.stdout


def check_stub(content, extra_arguments=()):
    with tempfile.TemporaryDirectory() as directory:
        with open(os.path.join(directory, "module.pyi"), "w") as stub:
            stub.write(textwrap.dedent(content))
        return run(["stubs", directory] + list(extra_arguments))


def expect_failure(content, message, extra_arguments=()):
    status, output = check_stub(content, extra_arguments)
    if status == 0:
        raise ValueError("[Test] The checker should fail on:\n" + content)
    if message not in output:
        raise ValueError(
            "[Test] Expected '%s' in the checker output:\n%s" % (message, output)
        )


def expect_success(content, extra_arguments=()):
    status, output = check_stub(content, extra_arguments)
    if status != 0:
        raise ValueError(
            "[Test] The checker should accept:\n%s\n%s" % (content, output)
        )


def test_unnamed_parameter():
    expect_failure(
        """
        def f(arg0: int) -> None: ...
        """,
        "unnamed parameter 'arg0'",
    )
    expect_failure(
        """
        class A:
            def method(self, name: str, arg12: float) -> None: ...
        """,
        "A.method: unnamed parameter 'arg12'",
    )
    expect_failure(
        """
        def f(*, arg1: int) -> None: ...
        """,
        "unnamed parameter 'arg1'",
    )


def test_named_parameters_close_to_argN():
    expect_success(
        """
        def f(argument: int, arg: int, arg_0: int, my_arg0: int) -> None: ...
        """
    )


def test_unresolved_annotation():
    expect_failure(
        """
        def f(point: ...) -> None: ...
        """,
        "parameter 'point': unresolved `...`",
    )
    expect_failure(
        """
        def f() -> ...: ...
        """,
        "return type: unresolved `...`",
    )
    expect_failure(
        """
        import typing
        def f(points: typing.Sequence[...]) -> None: ...
        """,
        "unresolved `...`",
    )
    expect_failure(
        """
        class A:
            value: ...
        """,
        "attribute annotation: unresolved `...`",
    )


def test_raw_cpp_type_name():
    expect_failure(
        """
        def f(value: "geode::Point<3>") -> None: ...
        """,
        "raw C++ type name 'geode::Point<3>'",
    )
    expect_failure(
        """
        def f() -> "absl::InlinedVector<int, 2>": ...
        """,
        "raw C++ type name",
    )
    expect_failure(
        """
        def f(values: "std::vector<int>") -> None: ...
        """,
        "raw C++ type name",
    )
    expect_failure(
        """
        def f(value: int = "geode::NO_ID") -> None: ...
        """,
        "default value: raw C++ type name",
    )


def test_invalid_stub():
    expect_failure("def f(:\n", "invalid stub")


def test_default_value_ellipsis_is_accepted():
    # stubgen writes `= ...` for a default it cannot represent
    expect_success(
        """
        def f(properties: int = ..., name: str = "") -> None: ...
        """
    )


def test_docstring_ellipsis_is_accepted():
    expect_success(
        '''
        def f(axis: int) -> int:
            """
            Return the coordinate (0 for x, 1 for y...) std::vector
            """
        '''
    )


BIND_VECTOR = """
    class List:
        def __init__(self, arg0: List) -> None: ...
        def __getitem__(self, arg0: int) -> int: ...
        def __setitem__(self, arg0: int, arg1: int) -> None: ...
        def __delitem__(self, arg0: int) -> None: ...
        def append(self, x: int) -> None: ...
        def extend(self, L: List) -> None: ...
        def pop(self) -> int: ...
"""


def test_bind_vector_members_are_the_only_exception():
    expect_success(BIND_VECTOR)
    # a method of the vector class that is not generated by bind_vector
    expect_failure(
        BIND_VECTOR + "        def other(self, arg0: int) -> None: ...\n",
        "List.other: unnamed parameter 'arg0'",
    )
    # the same dunder methods on a class that is not a bind_vector one
    expect_failure(
        """
        class Container:
            def __getitem__(self, arg0: int) -> int: ...
        """,
        "Container.__getitem__: unnamed parameter 'arg0'",
    )
    # a free function is never exempted
    expect_failure(
        """
        def __getitem__(arg0: int) -> int: ...
        """,
        "unnamed parameter 'arg0'",
    )


def test_explicit_allow():
    stub = """
        class Redirect:
            def __enter__(self, arg0: int) -> None: ...
    """
    expect_failure(stub, "Redirect.__enter__: unnamed parameter 'arg0'")
    expect_success(stub, ["--allow", "Redirect.__enter__"])
    expect_failure(stub, "unnamed parameter", ["--allow", "Redirect.other"])


def test_no_stub_found():
    with tempfile.TemporaryDirectory() as directory:
        status, output = run(["stubs", directory])
    if status == 0 or "no .pyi stub found" not in output:
        raise ValueError("[Test] An empty directory should fail:\n" + output)


def test_typing_ellipsis_is_accepted():
    # The `...` of Callable[..., R] and tuple[T, ...] is a valid annotation
    expect_success(
        """
        import typing
        def f(callback: typing.Callable[..., typing.Any]) -> tuple[int, ...]: ...
        def g(callbacks: dict[str, typing.Callable[..., int]] | None = None) -> None: ...
        """
    )
    # Elsewhere it is not
    expect_failure(
        """
        import typing
        def f(callback: typing.Callable[[int], ...]) -> None: ...
        """,
        "unresolved `...`",
    )
    expect_failure(
        """
        def f(values: tuple[..., int]) -> None: ...
        """,
        "unresolved `...`",
    )
    expect_failure(
        """
        def f(values: list[...]) -> None: ...
        """,
        "unresolved `...`",
    )


def test_typevar_annotation_is_rejected():
    # What pybind11-stubgen writes for a module-level TypeVar
    expect_failure(
        """
        import typing
        _T: typing.TypeVar
        def load(cls: type[_T]) -> _T: ...
        """,
        "TypeVar written as an annotation",
    )
    expect_success(
        """
        import typing
        _T = typing.TypeVar("_T")
        def load(cls: type[_T]) -> _T: ...
        """
    )


TYPEVAR_RUNTIME = """
import typing as _typing
_T = _typing.TypeVar("_T")
Key = _typing.TypeVar("Key")


def load(cls: "type[_T]", filename: str) -> "_T":
    return cls()


def first(keys: "list[Key]") -> "Key":
    return keys[0]
"""


def test_fix_rewrites_typevars():
    stub = """
        from __future__ import annotations
        import typing
        import typing as _typing
        __all__: list[str] = ['Key', 'first', 'load']
        def first(keys: list[Key]) -> Key:
            ...
        def load(cls: type[_T], filename: str) -> _T:
            ...
        Key: typing.TypeVar
        _T: typing.TypeVar
        """
    with tempfile.TemporaryDirectory() as directory:
        path = os.path.join(directory, "synthetic_stub_module.pyi")
        with open(path, "w") as file:
            file.write(textwrap.dedent(stub))
        status, output = run(["fix", directory])
        if status != 0:
            raise ValueError("[Test] fix failed:\n" + output)
        with open(path) as file:
            fixed = file.read()
        for expected in ['Key = typing.TypeVar("Key")', '_T = typing.TypeVar("_T")']:
            if expected not in fixed:
                raise ValueError("[Test] fix did not write %s:\n%s" % (expected, fixed))
        if ": typing.TypeVar" in fixed:
            raise ValueError("[Test] fix left a TypeVar annotation:\n" + fixed)
        status, output = run(["stubs", directory])
        if status != 0:
            raise ValueError("[Test] Fixed stub rejected:\n" + output)
        # A second fix changes nothing
        run(["fix", directory])
        with open(path) as file:
            if file.read() != fixed:
                raise ValueError("[Test] fix should be idempotent")
        with open(os.path.join(directory, "synthetic_stub_module.py"), "w") as file:
            file.write(TYPEVAR_RUNTIME)
        environment = dict(os.environ)
        environment["MYPYPATH"] = directory
        environment["PYTHONPATH"] = directory + os.pathsep + environment.get(
            "PYTHONPATH", ""
        )
        status, output = run(
            ["stubtest", "synthetic_stub_module", "--mypy-config-file", MYPY_CONFIG],
            environment,
        )
        if status != 0:
            raise ValueError("[Test] Fixed stub fails stubtest:\n" + output)
    with tempfile.TemporaryDirectory() as directory:
        status, output = run(["fix", directory])
    if status == 0 or "no .pyi stub found" not in output:
        raise ValueError("[Test] fix of an empty directory should fail:\n" + output)


UNHASHABLE_RUNTIME = """
class Value:
    def __eq__(self, other: object) -> bool:
        return True
class Derived(Value):
    pass
"""


def test_fix_unhashable_class():
    # pybind11 sets __hash__ to None for a class defining __eq__ without
    # __hash__; stubgen writes it as a ClassVar that mypy rejects
    stub = """
        from __future__ import annotations
        import typing
        __all__: list[str] = ['Derived', 'Value']
        class Value:
            __hash__: typing.ClassVar[None] = None
            def __eq__(self, other: object) -> bool:
                ...
        class Derived(Value):
            pass
        """
    with tempfile.TemporaryDirectory() as directory:
        path = os.path.join(directory, "synthetic_stub_module.pyi")
        with open(path, "w") as file:
            file.write(textwrap.dedent(stub))
        with open(os.path.join(directory, "synthetic_stub_module.py"), "w") as file:
            file.write(UNHASHABLE_RUNTIME)
        environment = dict(os.environ)
        environment["MYPYPATH"] = directory
        environment["PYTHONPATH"] = directory + os.pathsep + environment.get(
            "PYTHONPATH", ""
        )
        arguments = ["stubtest", "synthetic_stub_module", "--mypy-config-file", MYPY_CONFIG]
        status, output = run(arguments, environment)
        if status == 0:
            raise ValueError("[Test] The unfixed stub should fail mypy:\n" + output)
        status, output = run(["fix", directory])
        if status != 0:
            raise ValueError("[Test] fix failed:\n" + output)
        with open(path) as file:
            fixed = file.read()
        expected = "__hash__: typing.ClassVar[None] = None  # type: ignore[assignment]"
        if fixed.count(expected) != 1:
            raise ValueError("[Test] fix did not ignore the __hash__ error:\n" + fixed)
        run(["fix", directory])
        with open(path) as file:
            if file.read() != fixed:
                raise ValueError("[Test] fix should be idempotent:\n" + fixed)
        status, output = run(["stubs", directory])
        if status != 0:
            raise ValueError("[Test] Fixed stub rejected:\n" + output)
        status, output = run(arguments, environment)
        if status != 0:
            raise ValueError("[Test] Fixed unhashable stub fails stubtest:\n" + output)
    # The bare ClassVar form is handled too, other attributes are untouched
    with tempfile.TemporaryDirectory() as directory:
        path = os.path.join(directory, "other.pyi")
        with open(path, "w") as file:
            file.write(
                "from typing import ClassVar\nclass A:\n"
                "    __hash__: ClassVar[None]\n    other: ClassVar[None] = None\n"
            )
        run(["fix", directory])
        with open(path) as file:
            fixed = file.read()
        if "__hash__: ClassVar[None]  # type: ignore[assignment]" not in fixed or (
            "other: ClassVar[None] = None\n" not in fixed
        ):
            raise ValueError("[Test] Wrong fix of the bare ClassVar:\n" + fixed)


def test_imported_modules_without_stubs():
    # A module alone, whose stub imports other extension modules without
    # stubs (the upstream modules of a downstream project in a build tree):
    # their stubs are generated so that mypy resolves the types
    status, output = run(
        ["modules", "opengeode_py_mesh", "--mypy-config-file", MYPY_CONFIG]
    )
    if status != 0:
        raise ValueError("[Test] opengeode_py_mesh alone rejected:\n" + output)
    if "Checked 1 stub files" not in output:
        raise ValueError("[Test] Only the given module should be checked:\n" + output)


def test_real_binding_stubs():
    # The stubs of a real binding are accepted ...
    status, output = run(["modules", "opengeode_py_basic", "--mypy-config-file", MYPY_CONFIG])
    if status != 0:
        raise ValueError("[Test] opengeode_py_basic stubs rejected:\n" + output)


def stubtest_module(runtime, stub):
    with tempfile.TemporaryDirectory() as directory:
        with open(os.path.join(directory, "synthetic_stub_module.py"), "w") as file:
            file.write(textwrap.dedent(runtime))
        with open(os.path.join(directory, "synthetic_stub_module.pyi"), "w") as file:
            file.write(textwrap.dedent(stub))
        environment = dict(os.environ)
        environment["MYPYPATH"] = directory
        environment["PYTHONPATH"] = directory + os.pathsep + environment.get(
            "PYTHONPATH", ""
        )
        return run(
            ["stubtest", "synthetic_stub_module", "--mypy-config-file", MYPY_CONFIG],
            environment,
        )


def test_stubtest_detects_a_wrong_stub():
    runtime = """
    def f(value: int) -> int:
        return value
    class A:
        def method(self, name: str) -> str:
            return name
    """
    status, output = stubtest_module(runtime, runtime)
    if status != 0:
        raise ValueError("[Test] A matching stub should pass stubtest:\n" + output)
    status, output = stubtest_module(
        runtime,
        """
        def f(value_renamed: int) -> int: ...
        class A:
            def method(self, name: str) -> str: ...
        """,
    )
    if status == 0 or "synthetic_stub_module.f" not in output:
        raise ValueError("[Test] stubtest should reject a renamed parameter:\n" + output)
    status, output = stubtest_module(
        runtime,
        """
        def f(value: int) -> int: ...
        class A:
            def method(self, name: str) -> str: ...
            def missing_at_runtime(self) -> None: ...
        """,
    )
    if status == 0 or "missing_at_runtime" not in output:
        raise ValueError("[Test] stubtest should reject an extra method:\n" + output)


def test_stubtest_that_cannot_run_fails():
    # A stubtest which does not run (mypy missing, crash...) checked nothing:
    # it must not pass. A fake mypy.stubtest stands for each case.
    cases = {
        "crash": "import sys; print('Traceback: boom'); sys.exit(1)",
        "silent": "import sys; sys.exit(1)",
        "nothing": "pass",
    }
    for name, body in cases.items():
        with tempfile.TemporaryDirectory() as directory:
            package = os.path.join(directory, "mypy")
            os.makedirs(package)
            open(os.path.join(package, "__init__.py"), "w").close()
            with open(os.path.join(package, "stubtest.py"), "w") as file:
                file.write(body + "\n")
            environment = dict(os.environ)
            environment["PYTHONPATH"] = directory + os.pathsep + environment.get(
                "PYTHONPATH", ""
            )
            status, output = run(["stubtest", "os"], environment)
        if status == 0 or "0 error(s)" in output:
            raise ValueError(
                "[Test] A stubtest that cannot run should fail (%s):\n%s"
                % (name, output)
            )
    # without mypy at all
    with tempfile.TemporaryDirectory() as directory:
        with open(os.path.join(directory, "mypy.py"), "w") as file:
            file.write("raise ImportError('no mypy')\n")
        environment = dict(os.environ)
        environment["PYTHONPATH"] = directory + os.pathsep + environment.get(
            "PYTHONPATH", ""
        )
        status, output = run(["stubtest", "os"], environment)
    if status == 0:
        raise ValueError("[Test] Missing mypy should fail stubtest:\n" + output)


if __name__ == "__main__":
    test_stubtest_that_cannot_run_fails()
    test_unnamed_parameter()
    test_named_parameters_close_to_argN()
    test_unresolved_annotation()
    test_raw_cpp_type_name()
    test_invalid_stub()
    test_default_value_ellipsis_is_accepted()
    test_docstring_ellipsis_is_accepted()
    test_bind_vector_members_are_the_only_exception()
    test_explicit_allow()
    test_no_stub_found()
    test_typing_ellipsis_is_accepted()
    test_typevar_annotation_is_rejected()
    test_fix_rewrites_typevars()
    test_fix_unhashable_class()
    test_stubtest_detects_a_wrong_stub()
    test_real_binding_stubs()
    test_imported_modules_without_stubs()
