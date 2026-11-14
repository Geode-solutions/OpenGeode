# Working on the OpenGeode Python bindings

This file is for agents (and people) modifying the bindings. Users of the
package read `help(opengeode)`, whose text is `bindings/python/opengeode_doc.txt`.

## Layout

- `bindings/python/src/<module>/...`: pybind11 C++ code, one `define_*` function
  per bound class or family of classes, declared and called by the
  `PYBIND11_MODULE` of the module (`basic/basic.cpp`, `geometry/geometry.cpp`,
  `mesh/mesh.cpp`, ...). Extension modules are `opengeode_py_basic`,
  `_geometry`, `_image`, `_gdal`, `_mesh`, `_model`; each `src/<module>/CMakeLists.txt` lists
  its sources (`add_geode_python_binding`): add new .cpp files there.
- `bindings/python/{basic,geometry,image,mesh,model,gdal}.py`: pure-Python
  wrappers that re-export the extension module (`from opengeode_py_x import *`),
  initialize the C++ library and register the types in the generic registry.
  `basic.py` also holds the registry and the generic functions (`load`, `save`,
  `builder`, `find_attribute`, `register_type`, ...) and the logging bridge
  (`forward_logs_to_python`, progress listeners).
- `bindings/python/src/common.hpp`: helpers shared by every binding
  (`check_index`, `checked`, `checked_as`, `checked_builder`, `TypedObject`,
  `builder_with_mesh`, `def_readwrite_named`, `define_string_representation`,
  `python_call_repr`, ...). `src/mesh/index_check.hpp` has the index checks of
  meshes; `src/numpy_access.hpp` and `src/mesh/numpy_access.cpp` the numpy bulk
  access.
- `bindings/python/opengeode_doc.txt`: the package docstring (see Documentation).
- `bindings/python/tests/<module>/test-py-*.py`: one script per topic.
- `cmake/PythonTargets.cmake`: `add_geode_python_binding`, `add_geode_python_test`,
  `add_geode_python_stub_test`, `add_geode_python_wheel`; `cmake/check_python_stubs.py`
  (stub gates), `cmake/geode_python_package.py`, `cmake/geode_test_env.py`.
- Package layout: the wheel puts the extension modules at the root of the
  package (`opengeode.opengeode_py_mesh`), the shared libraries in a subfolder,
  and generates `__init__.py` (docstring, then `from .basic import *`, ...).
  In the build tree, the modules are top-level (`import opengeode_py_mesh`) and
  `opengeode` is a flat module generated in `build/<dir>/python_modules`.
  Pure-Python modules import their siblings with `try: from .basic import ...
  except ImportError: from basic import ...`.

## Conventions

- Every `def` has `pybind11::arg("name")` for each parameter (the stub gate
  rejects `arg0`), and a docstring. Name arguments after the C++ ones
  (`triangle_id`, `vertex_id`, `filename`, ...).
- Docstrings are verified against the C++ header comment AND the implementation
  (.cpp): behavior, exceptions, edge cases, units. Never infer from the name.
  Text shared by a macro instantiated for several classes must be true for all
  of them (2D/3D, each mesh type). Constructors and surprising operators are
  documented. An inaccurate docstring is a bug.
- Lifetimes: a function returning a reference or a view of its argument gets
  `pybind11::keep_alive<0, 1>()` (result keeps `self` alive); a function storing
  an argument in `self` gets `keep_alive<1, 2>()`. Builders keep their mesh
  alive and remember it in `_mesh` (`builder_with_mesh`, used by every `create`
  wrapper) so that they can check indices.
- Indices: Python never reaches undefined behavior. Wrap a method with
  `checked< Mesh >("name", &Mesh::method, VertexCheck{}, ...)` (const methods,
  `checked_as` for CRS managers, `checked_builder` for builders), or call
  `check_index(context, what, index, size)`. Out of range raises `IndexError`
  with `[function] vertex 99 out of range [0, 4)`. Unsigned parameters reject
  negative numbers with the usual pybind11 `TypeError`.
- Overloads do not cross classes: a `.def( "name", ... )` on a derived class
  hides every overload of `name` bound on its base classes (pybind11 only
  chains the overloads defined on the same class). Re-bind the base overloads
  on the derived class when it adds one; `test-py-signatures.py`
  (`check_hidden_overloads`) reports a base parameter list no longer accepted.
- Constructors: a class bound without `pybind11::init` gets an `__init__`
  raising `TypeError` with a docstring telling how to get an instance from
  `geode::document_missing_constructors( module )`, called last in each
  `PYBIND11_MODULE` (`test-py-signatures.py` rejects the generic "Initialize
  self" docstring).
- Enums: `pybind11::native_enum<...>(module, "CamelCase", "enum.Enum", doc)`
  with `.value(...)` and `.finalize()`, and no `export_values()`: members are
  used through the class (`Logger.Level.info`).
- Exceptions: C++ `OpenGeodeException` becomes `OpenGeodeDataError`,
  `OpenGeodeInternalError` or `OpenGeodeResultError` (all `OpenGeodeError`, a
  `RuntimeError`), with `type`, `project`, `library`, `message`, `details`, and
  nested errors chained in `__cause__`. Python-side checks raise the standard
  `IndexError`/`ValueError`/`TypeError`; python-side data errors in `basic.py`
  use `_data_error` so that they look like the C++ ones.
- GIL: release it (`call_guard<pybind11::gil_scoped_release>()`, or a scoped
  release) around heavy C++ work (I/O, numpy bulk access, searches). Everything
  touching Python objects (argument conversion, numpy arrays, callbacks) must
  happen with the GIL held: numpy functions validate and copy the arrays first,
  then release the GIL for the C++ work (`numpy_access.hpp`). C++ clients that
  may run on worker threads (the logger, progress logger) only push events in a
  queue; Python drains them from a daemon thread, which the atexit handler
  stops and joins: a daemon thread taking the GIL back in C++ code during the
  interpreter finalization aborts the process on CPython < 3.14. Do not add locks on hot read
  accessors: concurrent reads and modifications of one mesh are unsupported.
- Typed wrappers: functions returning a pybind11 `object` use `TypedObject<T>`
  so that the stubs say `T` instead of `object`.
- Value classes (Point, Vector, uuid, ...) are hashable by value and have an
  eval-able `__repr__` (`define_string_representation`, `python_call_repr`).
  A mutable class with `__eq__` and no `__hash__` is unhashable (pybind11 sets
  `__hash__ = None`): no explicit `__hash__` is needed, `check_python_stubs.py
  fix` makes its stub line (`__hash__: ClassVar[None] = None`) pass mypy.
- Geometry objects built from Points share the Python Point objects (mutating a
  point mutates the geometry); geometry returned by a mesh is a snapshot built
  from copies of the mesh points (`checked_snapshot` in `mesh/index_check.hpp`):
  never return geometry referring to mesh storage, which any edit reallocates.
- Registry: `register_type(cls, load=..., save=..., builder=..., ...)` in the
  pure-Python module of the binding (see `mesh.py`, `model.py`) for each new
  type; `extend_type` to add operations; `resolve` maps an object to its most
  derived bound class (`_bound_interface`); `resolve` is not a separate function to
  register: pass it as the `resolve=` argument of `register_type` or `extend_type`.
  Pass the bound functions: the generic functions only call them. Add the type to the tests of
  `test-py-generic-api.py`.
- Comments and docstrings in the binding code stay short and factual; C++ is
  formatted with the repo `.clang-format`; each file keeps its line endings
  (the repo mixes CRLF and LF: do not rewrite files wholesale).
- No new epsilon or threshold constants; reuse `GLOBAL_EPSILON`.

## Tests

- Tests are plain scripts raising `ValueError("[Test] ...")` (no pytest), first
  importing `geode_test_env`. Register each one in the `CMakeLists.txt` of its
  folder with `add_geode_python_test(SOURCE ... DEPENDENCIES ${PROJECT_NAME}::py_x)`.
- Every fix or feature ships a test that fails before and passes after. Test
  generously: edge cases, error paths (the exact exception class), 2D and 3D,
  every mesh type, builders and models, and any behavior relied on but untested.
  Use fresh mesh objects per case and temporary directories for files.
- `test-py-signatures.py`, `test-py-keyword-arguments.py`, `test-py-naming.py`,
  `test-py-repr.py`, `test-py-docstring-claims.py` check conventions across
  every class; extend their tables when you bind something new.
- Stub gates (`test-py-stubs`, `test-py-wrapper-stubs`, `test-py-stub-gates`,
  `test-py-generic-api-typing`): pybind11-stubgen output must have no `argN`, no
  unresolved or raw C++ type (`geode::`, `absl::`, `std::`), and pass
  `mypy.stubtest`. Use `TypedObject` or explicit types when a signature shows
  `object` or `...`. The stubs of the compiled modules can report mypy errors if
  put on MYPYPATH; the default use from site-packages is silent.
- `test-py-package-docs.py` runs the examples of `opengeode_doc.txt` as doctests:
  change an API shown there and the test tells you; examples must stay
  deterministic and self-contained.

## Build and run

Configure once. A fresh worktree is configured standalone (no superbuild), reusing
the third-party installs of an existing checkout (`<third>` below, e.g.
`<main checkout>/build/third_party`), with a venv providing pybind11:

    python3 -m venv build/venv && build/venv/bin/pip install pybind11==3.0.4
    mkdir -p build/empty_install build/empty_pybind11
    cmake -S . -B build/opengeode -G Ninja -DCMAKE_BUILD_TYPE=Release \
      -DUSE_SUPERBUILD=OFF -DOPENGEODE_WITH_PYTHON=ON -DOPENGEODE_WITH_TESTS=ON \
      -DPYTHON_EXECUTABLE=$PWD/build/venv/bin/python \
      -DPython_EXECUTABLE=$PWD/build/venv/bin/python \
      -Dpybind11_DIR=$(build/venv/bin/python -m pybind11 --cmakedir) \
      -DOpenGeode_PATH_INSTALL=$PWD/build/empty_install \
      -DPYBIND11_INSTALL_PREFIX=$PWD/build/empty_pybind11 \
      -DABSEIL_INSTALL_PREFIX=<third>/abseil/install \
      -DASYNCPLUSPLUS_INSTALL_PREFIX=<third>/asyncplusplus/install \
      -DBITSERY_INSTALL_PREFIX=<third>/bitsery/install \
      -DEARCUT_INSTALL_PREFIX=<third>/earcut/install \
      -DGDAL_INSTALL_PREFIX=<third>/gdal/install \
      -DJSON_INSTALL_PREFIX=<third>/json/install \
      -DMINIZIP_INSTALL_PREFIX=<third>/minizip/install \
      -DNANOFLANN_INSTALL_PREFIX=<third>/nanoflann/install \
      -DPROJ_INSTALL_PREFIX=<third>/proj/install \
      -DSPDLOG_INSTALL_PREFIX=<third>/spdlog/install \
      -DSQLITE_INSTALL_PREFIX=<third>/sqlite/install \
      -DZLIB_INSTALL_PREFIX=<third>/zlib/install \
      -DCMAKE_INSTALL_PREFIX=$PWD/build/opengeode/install

Set `OpenGeode_PATH_INSTALL` and `PYBIND11_INSTALL_PREFIX` to an empty directory
before any `ninja install`: they feed `install(DIRECTORY <dir>/ DESTINATION .)`,
which copies the whole filesystem root when they are empty strings. Never take
`CMAKE_INSTALL_PREFIX` from another build (it would overwrite its install), and
check `grep 'TYPE DIRECTORY' build/opengeode/cmake_install.cmake` before installing.
`build/opengeode/CMakeCache.txt` of an existing build shows the exact options. Then:

    ninja -C build/opengeode -j10
    ctest --test-dir build/opengeode -R py -j8 --output-on-failure

`ctest` runs each test with the interpreter of `build/opengeode/python_test_venv`
and `PYTHONPATH` set to the build tree (`geode_test_env`, `lib`, `python_modules`).
To run a script or an interactive session by hand, take that environment from
`ctest -R <test> -V` (the `Test command` and `PYTHONPATH` lines), e.g.:

    PYTHONPATH=<same as ctest> build/opengeode/python_test_venv/bin/python script.py

CI builds with assertions on (a Release build skips them: run an assertions-on
build before pushing C++ changes). The `wheel` target builds the wheel, generates
the stubs and runs the stub gates on the whole package.
Editing `opengeode_doc.txt` re-runs CMake (the text is copied into the generated
`__init__.py`, which must not contain `"""` or backslashes).

## Documentation

- `help(opengeode)` (package docstring) = `bindings/python/opengeode_doc.txt`: the five
  workflows (build, save, load/inspect, attributes, numpy), generic functions,
  errors, logging, pitfalls. Keep the examples (`>>>`) runnable.
- Every function and class has its own docstring (the reference).
- Commits use only release-triggering types (`fix(Python): ...`, `feat(Python): ...`);
  no `docs`/`test`/`refactor` types, no `BREAKING CHANGE`. Python renames need no aliases.
