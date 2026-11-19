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

"""Type checkers infer the results of the generic API from the generated stubs.

    test-py-generic-api-typing.py <check_python_stubs.py>

The stubs are generated as the wheel target does: pybind11-stubgen with the
whole package imported (every type registered), then check_python_stubs.py fix.
mypy then checks snippets using the stubs only, without stubtest_mypy.ini: it
disables the misc errors, which include the reveal_type notes. The errors of
the stubs of the compiled modules (e.g. __eq__ overrides) are ignored, those of
the pure-Python modules are not.
"""

import os
import subprocess
import sys
import tempfile

import geode_test_env

MODULES = [
    "opengeode",
    "basic",
    "geometry",
    "image",
    "mesh",
    "model",
    "gdal",
    "opengeode_py_basic",
    "opengeode_py_geometry",
    "opengeode_py_image",
    "opengeode_py_mesh",
    "opengeode_py_model",
    "opengeode_py_gdal",
]

GENERATE = """
import sys
import opengeode
import pybind11_stubgen
for module in sys.argv[2:]:
    pybind11_stubgen.main(["--exit-code", "-o", sys.argv[1], module])
"""

REVEALED = """
import opengeode as og

surface = og.load(og.TriangulatedSurface3D, "surface.og_tsf3d")
reveal_type(surface)
reveal_type(og.load_all(og.BRep, "model.og_brep"))
reveal_type(og.load(og.RasterImage2D, "image.og_img2d"))
reveal_type(og.load_one(og.load_section("model.og_sctn"), "model.og_sctn"))
reveal_type(og.builder(surface))
reveal_type(og.builder(og.BRep()))
reveal_type(og.builder(og.Graph.create()))
reveal_type(og.builder(og.SolidMesh3D.create()))
reveal_type(og.save(surface, "surface.og_tsf3d"))
reveal_type(og.is_loadable(og.BRep, "model.og_brep"))
reveal_type(og.is_saveable(og.BRep, "model.og_brep"))
reveal_type(og.input_extensions(og.Section))
reveal_type(og.additional_files(og.BRep, "model.og_brep"))
reveal_type(og.builder_type(og.BRep))
reveal_type(og.find_attribute(og.AttributeManager(), "name"))
og.forward_logs_to_python("opengeode")
og.stop_forwarding_logs()


def listener(event: og.ProgressEvent) -> None:
    pass


og.add_progress_listener(listener)
og.remove_progress_listener(listener)
og.register_type(int, load=lambda filename: [1], builder=og.BRepBuilder)
og.register_type(
    og.BRep,
    load=og.load_brep,
    save=og.save_brep,
    is_saveable=og.is_brep_saveable,
    resolve=lambda brep: brep,
    operations={"other": og.save_brep},
)
og.extend_type(og.Section, replace=True, save=og.save_section)
reveal_type(og.object_operation(surface, "save"))
"""

EXPECTED = [
    "opengeode_py_mesh.TriangulatedSurface3D",
    "list[opengeode_py_model.BRep]",
    "opengeode_py_image.RasterImage2D",
    "opengeode_py_model.Section",
    "opengeode_py_mesh.TriangulatedSurfaceBuilder3D",
    "opengeode_py_model.BRepBuilder",
    "opengeode_py_mesh.GraphBuilder",
    "opengeode_py_mesh.SolidMeshBuilder3D",
    "list[str]",
    "opengeode_py_basic.Percentage",
    "bool",
    "list[str]",
    "opengeode_py_basic.AdditionalFiles",
    "type | None",
    "opengeode_py_basic.AttributeBase",
    "def (*Any, **Any) -> Any",
]

# Each line must be reported as an error
WRONG = [
    'model: og.BRep = og.load(og.TriangulatedSurface3D, "surface.og_tsf3d")',
    'og.load(og.BRep, 42)',
    'og.load_all(og.BRep(), "model.og_brep")',
    'builder: og.BRepBuilder = og.builder(og.TriangulatedSurface3D.create())',
    'og.add_progress_listener(42)',
    'og.forward_logs_to_python(42)',
    # Registrations are checked against the registered class
    'og.register_type(og.BRep, load=og.load_section)',
    'og.register_type(og.BRep, save=og.save_section)',
    'og.register_type(og.BRep, is_saveable=og.is_section_saveable)',
    'og.extend_type(og.Section, load=og.load_brep)',
    'og.extend_type(og.Section, replace="yes")',
    'og.register_type(og.BRep, operations={"other": 42})',
]


def generate_stubs(check_script, directory):
    result = subprocess.run(
        [sys.executable, "-c", GENERATE, directory] + MODULES,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    if result.returncode:
        print(result.stdout)
        raise ValueError("[Test] pybind11-stubgen failed")
    for command in ["fix", "stubs"]:
        result = subprocess.run(
            [sys.executable, check_script, command, directory],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        if result.returncode:
            print(result.stdout)
            raise ValueError(f"[Test] check_python_stubs.py {command} failed")


def check_stub_text(directory):
    with open(os.path.join(directory, "basic.pyi"), encoding="utf-8") as stub:
        text = stub.read()
    for expected in [
        '_T = typing.TypeVar("_T")',
        "def load(cls: type[_T], filename: str) -> _T:",
        "def load_all(cls: type[_T], filename: str) -> list[_T]:",
        "def forward_logs_to_python(logger_name: str = 'opengeode') -> None:",
        "def stop_forwarding_logs() -> None:",
        "def add_progress_listener(listener: _typing.Callable[[ProgressEvent], typing.Any]) -> None:",
        "def remove_progress_listener(listener: _typing.Callable[[ProgressEvent], typing.Any]) -> None:",
        "def builder(obj: opengeode_py_mesh.TriangulatedSurface3D) -> opengeode_py_mesh.TriangulatedSurfaceBuilder3D:",
    ]:
        if expected not in text:
            raise ValueError(f"[Test] '{expected}' not in the basic stub")
    if "_T: typing.TypeVar" in text or "__annotate__" in text:
        raise ValueError("[Test] Invalid TypeVar or __annotate__ in the basic stub")


PURE_PYTHON_STUBS = ["opengeode", "basic", "geometry", "image", "mesh", "model", "gdal"]


def run_mypy(directory, source):
    """Return the mypy messages about the snippet and the pure-Python stubs."""
    with tempfile.TemporaryDirectory() as work:
        snippet = os.path.join(work, "snippet.py")
        with open(snippet, "w", encoding="utf-8") as file:
            file.write(source)
        # An explicit configuration: no user or project one is read
        mypy_config = os.path.join(work, "mypy.ini")
        with open(mypy_config, "w", encoding="utf-8") as file:
            file.write("[mypy]\n")
        environment = dict(os.environ, MYPYPATH=directory)
        result = subprocess.run(
            [
                sys.executable,
                "-m",
                "mypy",
                "--config-file",
                mypy_config,
                "--no-incremental",
                "--cache-dir",
                os.path.join(work, "cache"),
                "--no-error-summary",
                snippet,
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=environment,
            cwd=work,
        )
        if "snippet.py" not in result.stdout and "Success" not in result.stdout:
            print(result.stdout)
            raise ValueError("[Test] mypy did not check the snippet")
        checked = ["snippet.py:"] + [
            os.path.join(directory, module + ".pyi:") for module in PURE_PYTHON_STUBS
        ]
        return [
            line
            for line in result.stdout.splitlines()
            if any(line.startswith(prefix) for prefix in checked)
        ]


def check_revealed_types(directory):
    messages = run_mypy(directory, REVEALED)
    revealed = [
        line.split('Revealed type is "', 1)[1].rstrip('"')
        for line in messages
        if "Revealed type is" in line
    ]
    errors = [line for line in messages if ": error:" in line]
    if errors or revealed != EXPECTED:
        print("\n".join(messages))
        raise ValueError(f"[Test] Wrong revealed types: {revealed}")


def check_wrong_uses(directory):
    for line in WRONG:
        messages = run_mypy(directory, "import opengeode as og\n" + line + "\n")
        if not any(m.startswith("snippet.py:2: error:") for m in messages):
            print("\n".join(messages))
            raise ValueError(f"[Test] mypy should reject: {line}")


if __name__ == "__main__":
    check_script = sys.argv[1]
    with tempfile.TemporaryDirectory() as directory:
        generate_stubs(check_script, directory)
        check_stub_text(directory)
        check_revealed_types(directory)
        check_wrong_uses(directory)
