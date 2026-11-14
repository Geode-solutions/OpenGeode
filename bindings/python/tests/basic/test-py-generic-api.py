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

"""Generic Python API: load/save/builder by class, registry, attributes by name."""

import inspect
import os
import sys
import tempfile
import typing

import geode_test_env

import opengeode as og

# The pure-Python module defining the generic functions (opengeode.basic in
# the wheel, basic in the build tree)
basic = sys.modules[og.load.__module__]

DATA = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "..", "tests", "data"
)


def expect_raises(error_type, function, *args, contains=None, **kwargs):
    try:
        function(*args, **kwargs)
    except error_type as error:
        if contains is not None and contains not in str(error):
            raise ValueError(
                f"[Test] '{contains}' not in the error message: {error}"
            )
        return error
    raise ValueError(
        f"[Test] {getattr(function, '__name__', function)}{args} should raise "
        f"{error_type.__name__}"
    )


# --- Objects of every registered type ----------------------------------------


def point(dimension, coordinates):
    return og.Point2D(coordinates[:2]) if dimension == 2 else og.Point3D(coordinates)


def fill_points(builder, dimension, nb):
    coordinates = [[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1]]
    for vertex in range(nb):
        builder.create_point(point(dimension, coordinates[vertex]))


def make_vertex_set():
    mesh = og.VertexSet.create()
    og.builder(mesh).create_vertices(3)
    return mesh


def make_graph():
    mesh = og.Graph.create()
    builder = og.builder(mesh)
    builder.create_vertices(3)
    builder.create_edge()
    return mesh


def make_point_set(cls, dimension):
    def make():
        mesh = cls.create()
        fill_points(og.builder(mesh), dimension, 3)
        return mesh

    return make


def make_edged_curve(cls, dimension):
    def make():
        mesh = cls.create()
        builder = og.builder(mesh)
        fill_points(builder, dimension, 2)
        builder.create_edge_with_vertices(0, 1)
        return mesh

    return make


def make_polygonal_surface(cls, dimension):
    def make():
        mesh = cls.create()
        builder = og.builder(mesh)
        fill_points(builder, dimension, 3)
        builder.create_polygon([0, 1, 2])
        return mesh

    return make


def make_triangulated_surface(cls, dimension):
    def make():
        mesh = cls.create()
        builder = og.builder(mesh)
        fill_points(builder, dimension, 3)
        builder.create_triangle([0, 1, 2])
        return mesh

    return make


def make_regular_grid(cls, dimension):
    def make():
        grid = cls.create()
        og.builder(grid).initialize_grid(
            point(dimension, [0, 0, 0]), [2] * dimension, [1.0] * dimension
        )
        return grid

    return make


def make_polyhedral_solid():
    mesh = og.PolyhedralSolid3D.create()
    builder = og.builder(mesh)
    fill_points(builder, 3, 4)
    builder.create_polyhedron([0, 1, 2, 3], [[0, 1, 2], [0, 1, 3], [1, 2, 3], [0, 2, 3]])
    return mesh


def make_tetrahedral_solid(cls):
    def make():
        mesh = cls.create()
        builder = og.builder(mesh)
        fill_points(builder, 3, 4)
        builder.create_tetrahedron([0, 1, 2, 3])
        return mesh

    return make


def make_light_regular_grid(cls, dimension):
    def make():
        return cls(point(dimension, [0, 0, 0]), [2] * dimension, [1.0] * dimension)

    return make


def make_raster_image(cls, dimension):
    def make():
        return cls([2] * dimension)

    return make


def make_brep():
    brep = og.BRep()
    builder = og.builder(brep)
    builder.add_corner()
    builder.add_corner()
    return brep


def make_section():
    section = og.Section()
    builder = og.builder(section)
    builder.add_corner()
    builder.add_corner()
    return section


def nb_vertices(mesh):
    return mesh.nb_vertices()


def nb_cells(grid):
    return grid.nb_cells()


def nb_corners(model):
    return model.nb_corners()


# class: (native extension, maker, size function, builder class or None)
TYPES = {
    og.VertexSet: ("og_vts", make_vertex_set, nb_vertices, og.VertexSetBuilder),
    og.Graph: ("og_grp", make_graph, nb_vertices, og.GraphBuilder),
    og.PointSet2D: ("og_pts2d", make_point_set(og.PointSet2D, 2), nb_vertices, og.PointSetBuilder2D),
    og.PointSet3D: ("og_pts3d", make_point_set(og.PointSet3D, 3), nb_vertices, og.PointSetBuilder3D),
    og.EdgedCurve2D: ("og_edc2d", make_edged_curve(og.EdgedCurve2D, 2), nb_vertices, og.EdgedCurveBuilder2D),
    og.EdgedCurve3D: ("og_edc3d", make_edged_curve(og.EdgedCurve3D, 3), nb_vertices, og.EdgedCurveBuilder3D),
    og.PolygonalSurface2D: ("og_psf2d", make_polygonal_surface(og.PolygonalSurface2D, 2), nb_vertices, og.PolygonalSurfaceBuilder2D),
    og.PolygonalSurface3D: ("og_psf3d", make_polygonal_surface(og.PolygonalSurface3D, 3), nb_vertices, og.PolygonalSurfaceBuilder3D),
    og.TriangulatedSurface2D: ("og_tsf2d", make_triangulated_surface(og.TriangulatedSurface2D, 2), nb_vertices, og.TriangulatedSurfaceBuilder2D),
    og.TriangulatedSurface3D: ("og_tsf3d", make_triangulated_surface(og.TriangulatedSurface3D, 3), nb_vertices, og.TriangulatedSurfaceBuilder3D),
    og.RegularGrid2D: ("og_rgd2d", make_regular_grid(og.RegularGrid2D, 2), nb_cells, og.RegularGridBuilder2D),
    og.RegularGrid3D: ("og_rgd3d", make_regular_grid(og.RegularGrid3D, 3), nb_cells, og.RegularGridBuilder3D),
    og.PolyhedralSolid3D: ("og_pso3d", make_polyhedral_solid, nb_vertices, og.PolyhedralSolidBuilder3D),
    og.TetrahedralSolid3D: ("og_tso3d", make_tetrahedral_solid(og.TetrahedralSolid3D), nb_vertices, og.TetrahedralSolidBuilder3D),
    og.HybridSolid3D: ("og_hso3d", make_tetrahedral_solid(og.HybridSolid3D), nb_vertices, og.HybridSolidBuilder3D),
    og.LightRegularGrid2D: ("og_lrgd2d", make_light_regular_grid(og.LightRegularGrid2D, 2), nb_cells, None),
    og.LightRegularGrid3D: ("og_lrgd3d", make_light_regular_grid(og.LightRegularGrid3D, 3), nb_cells, None),
    og.RasterImage1D: ("og_img1d", make_raster_image(og.RasterImage1D, 1), nb_cells, None),
    og.RasterImage2D: ("og_img2d", make_raster_image(og.RasterImage2D, 2), nb_cells, None),
    og.RasterImage3D: ("og_img3d", make_raster_image(og.RasterImage3D, 3), nb_cells, None),
    og.BRep: ("og_brep", make_brep, nb_corners, og.BRepBuilder),
    og.Section: ("og_sctn", make_section, nb_corners, og.SectionBuilder),
}


# --- Registry content ---------------------------------------------------------


def check_registered_types():
    registered = og.registered_types()
    for cls in list(TYPES) + [og.ExternalTimeSeries]:
        if cls not in registered:
            raise ValueError(f"[Test] {cls.__name__} should be registered")
    # Builder-only registrations (meshes of an abstract type)
    for cls in [og.SurfaceMesh2D, og.SurfaceMesh3D, og.SolidMesh3D]:
        if cls not in registered:
            raise ValueError(f"[Test] {cls.__name__} should be registered")
    if len(set(registered)) != len(registered):
        raise ValueError("[Test] registered_types() should not repeat a type")


def check_extensions():
    for cls, (extension, *_rest) in TYPES.items():
        if extension not in og.input_extensions(cls):
            raise ValueError(f"[Test] {extension} should be an input of {cls.__name__}")
        if extension not in og.output_extensions(cls):
            raise ValueError(f"[Test] {extension} should be an output of {cls.__name__}")
    native = og.ExternalTimeSeries.native_extension_static()
    if og.input_extensions(og.ExternalTimeSeries) != [native]:
        raise ValueError("[Test] Wrong ExternalTimeSeries input extensions")
    if og.output_extensions(og.ExternalTimeSeries) != [native]:
        raise ValueError("[Test] Wrong ExternalTimeSeries output extensions")


# --- Round trips --------------------------------------------------------------


def check_round_trip(directory, cls):
    extension, make, size, _ = TYPES[cls]
    original = make()
    if size(original) == 0:
        raise ValueError(f"[Test] Empty {cls.__name__} built for the round trip")
    filename = os.path.join(directory, f"generic.{extension}")
    if not og.is_saveable(original, filename):
        raise ValueError(f"[Test] {cls.__name__} should be saveable as {extension}")
    if not og.is_saveable(cls, filename):
        raise ValueError(f"[Test] {cls.__name__} class should be saveable as {extension}")
    if og.is_saveable(original, os.path.join(directory, "generic.unknown")):
        raise ValueError(f"[Test] {cls.__name__} should not be saveable as .unknown")
    if og.is_saveable(cls, os.path.join(directory, "generic.unknown")):
        raise ValueError(f"[Test] {cls.__name__} class should not be saveable as .unknown")
    # The extension is case insensitive and the filename stripped, as in C++
    upper = os.path.join(directory, f"generic.{extension.upper()}  ")
    if not og.is_saveable(cls, upper) or not og.is_saveable(original, upper):
        raise ValueError(f"[Test] {cls.__name__} should be saveable as {upper!r}")
    written = og.save(original, filename)
    if not isinstance(written, list) or filename not in written:
        raise ValueError(f"[Test] save should return the written files, got {written}")
    percentage = og.is_loadable(cls, filename)
    if not isinstance(percentage, og.Percentage) or percentage.value() <= 0:
        raise ValueError(f"[Test] {cls.__name__} file should be loadable: {percentage}")
    if og.is_loadable(cls, os.path.join(directory, "generic.unknown")).value() != 0:
        raise ValueError("[Test] Unknown extension should not be loadable")
    if not isinstance(og.additional_files(cls, filename), og.AdditionalFiles):
        raise ValueError("[Test] additional_files should return AdditionalFiles")
    loaded = og.load(cls, filename)
    if not isinstance(loaded, cls):
        raise ValueError(f"[Test] load should return a {cls.__name__}, got {type(loaded)}")
    if size(loaded) != size(original):
        raise ValueError(f"[Test] {cls.__name__} round trip changed the object")
    loaded_all = og.load_all(cls, filename)
    if not isinstance(loaded_all, list) or len(loaded_all) != 1:
        raise ValueError(f"[Test] load_all should return a list of one {cls.__name__}")
    if not isinstance(loaded_all[0], cls) or size(loaded_all[0]) != size(original):
        raise ValueError(f"[Test] load_all returned a wrong {cls.__name__}")


def check_external_time_series_round_trip(directory):
    filename = os.path.join(
        directory, "series." + og.ExternalTimeSeries.native_extension_static()
    )
    written = og.save(og.ExternalTimeSeries(), filename)
    if written != [filename]:
        raise ValueError(f"[Test] Wrong ExternalTimeSeries written files: {written}")
    if not isinstance(og.load(og.ExternalTimeSeries, filename), og.ExternalTimeSeries):
        raise ValueError("[Test] load should return an ExternalTimeSeries")
    # No is_loadable/is_saveable/additional_files/builder in C++ for this type
    expect_raises(TypeError, og.is_loadable, og.ExternalTimeSeries, filename, contains="is_loadable")
    expect_raises(TypeError, og.is_saveable, og.ExternalTimeSeries(), filename, contains="is_saveable")
    expect_raises(TypeError, og.additional_files, og.ExternalTimeSeries, filename, contains="additional_files")
    expect_raises(TypeError, og.builder, og.ExternalTimeSeries(), contains="builder")
    expect_raises(og.OpenGeodeDataError, og.save, og.ExternalTimeSeries(), filename + ".wrong")


# --- Builders -----------------------------------------------------------------


def check_builders():
    for cls, (_, make, _, builder_class) in TYPES.items():
        obj = make()
        if builder_class is None:
            expect_raises(TypeError, og.builder, obj, contains=cls.__name__)
            continue
        builder = og.builder(obj)
        if type(builder) is not builder_class:
            raise ValueError(
                f"[Test] builder of {cls.__name__} should be {builder_class.__name__}, got {type(builder)}"
            )
    # The builder keeps the object alive and modifies it
    builder = og.builder(og.TriangulatedSurface3D.create())
    builder.create_point(og.Point3D([0, 0, 0]))
    # Most derived registered type wins over its registered bases
    surface = og.TriangulatedSurface3D.create()
    if type(og.builder(surface)) is not og.TriangulatedSurfaceBuilder3D:
        raise ValueError("[Test] Builder of a TriangulatedSurface3D should not be a SurfaceMeshBuilder3D")
    # The abstract meshes have a builder (used by objects typed as them)
    for mesh, builder_class in [
        (og.SurfaceMesh2D, og.SurfaceMeshBuilder2D),
        (og.SurfaceMesh3D, og.SurfaceMeshBuilder3D),
        (og.SolidMesh3D, og.SolidMeshBuilder3D),
    ]:
        if og.builder_type(mesh) is not builder_class:
            raise ValueError(f"[Test] {mesh.__name__} builder should be {builder_class.__name__}")
    for cls, (_, _, _, builder_class) in TYPES.items():
        if og.builder_type(cls) is not builder_class:
            raise ValueError(f"[Test] Wrong builder_type of {cls.__name__}")


# --- Error paths --------------------------------------------------------------


def check_errors(directory):
    unknown = os.path.join(directory, "file.unknown_extension")
    error = expect_raises(og.OpenGeodeDataError, og.load, og.TriangulatedSurface3D, unknown, contains="unknown_extension")
    if error.type != "data":
        raise ValueError("[Test] Unknown extension should be a data error")
    expect_raises(og.OpenGeodeDataError, og.load_all, og.BRep, unknown)
    # An unknown extension is a data error when saving too, as when loading
    error = expect_raises(og.OpenGeodeDataError, og.save, og.TriangulatedSurface3D.create(), unknown, contains="unknown_extension")
    if error.type != "data":
        raise ValueError("[Test] Unknown extension should be a data error when saving")
    expect_raises(og.OpenGeodeDataError, og.save, og.BRep(), unknown, contains="unknown_extension")
    expect_raises(og.OpenGeodeDataError, og.additional_files, og.BRep, unknown)
    # Wrong types
    expect_raises(TypeError, og.load, int, unknown, contains="int")
    expect_raises(TypeError, og.load, "BRep", unknown)
    expect_raises(TypeError, og.load_all, object, unknown)
    expect_raises(TypeError, og.is_loadable, int, unknown)
    expect_raises(TypeError, og.input_extensions, int)
    expect_raises(TypeError, og.output_extensions, int)
    expect_raises(TypeError, og.save, 42, unknown, contains="int")
    expect_raises(TypeError, og.is_saveable, 42, unknown)
    expect_raises(TypeError, og.is_saveable, int, unknown)
    expect_raises(TypeError, og.builder, 42, contains="int")
    # Builder-only types cannot be loaded nor saved
    expect_raises(TypeError, og.load, og.SurfaceMesh3D, unknown, contains="load")
    expect_raises(TypeError, og.input_extensions, og.SurfaceMesh3D)
    # A load of a base class does not accept a subclass object
    expect_raises(TypeError, og.load, og.Grid3D, unknown)
    # The error message lists the registered types
    expect_raises(TypeError, og.load, int, unknown, contains="TriangulatedSurface3D")


# --- load_one -----------------------------------------------------------------


def check_load_one():
    for objects in [[], [1, 2]]:
        error = expect_raises(og.OpenGeodeDataError, og.load_one, objects, "some.file")
        if not isinstance(error, RuntimeError) or not isinstance(error, og.OpenGeodeError):
            raise ValueError("[Test] load_one error should be an OpenGeodeError")
        if error.type != "data" or error.project != "OpenGeode" or error.library != "Basic":
            raise ValueError("[Test] Wrong load_one error attributes")
        expected = f'[load_one] File "some.file" holds {len(objects)} objects, expected exactly one'
        if error.message != expected or str(error) != expected:
            raise ValueError(f"[Test] Wrong load_one message: {error.message}")
        if expected not in error.details or error.__cause__ is not None:
            raise ValueError("[Test] Wrong load_one details")
    if og.load_one([42], "file") != 42:
        raise ValueError("[Test] load_one should return the only object")


# --- Registry extension by a downstream type ---------------------------------


class FakeModel:
    """Downstream type stored as a text file: one line per object."""

    def __init__(self, name="fake"):
        self.name = name


class FakeModelBuilder:
    def __init__(self, model):
        self.model = model

    def set_name(self, name):
        self.model.name = name


class FakeMeshBuilder:
    @staticmethod
    def create(mesh):
        builder = FakeMeshBuilder()
        builder.mesh = mesh
        return builder


class FakeMesh:
    pass


class DerivedFakeModel(FakeModel):
    pass


def fake_load(filename):
    with open(filename) as file:
        return [FakeModel(line.strip()) for line in file if line.strip()]


def fake_save(model, filename):
    with open(filename, "w") as file:
        file.write(model.name + "\n")
    return [filename]


def check_registry_extension(directory):
    og.register_type(
        FakeModel,
        load=fake_load,
        save=fake_save,
        is_loadable=lambda filename: og.Percentage(1 if filename.endswith(".fake") else 0),
        is_saveable=lambda model, filename: filename.endswith(".fake"),
        input_extensions=lambda: ["fake"],
        output_extensions=lambda: ["fake"],
        additional_files=lambda filename: og.AdditionalFiles(),
        builder=FakeModelBuilder,
    )
    og.register_type(FakeMesh, builder=FakeMeshBuilder)
    if FakeModel not in og.registered_types() or FakeMesh not in og.registered_types():
        raise ValueError("[Test] Fake types should be registered")
    filename = os.path.join(directory, "model.fake")
    model = FakeModel()
    builder = og.builder(model)
    if not isinstance(builder, FakeModelBuilder):
        raise ValueError("[Test] Wrong fake builder")
    builder.set_name("downstream")
    if og.save(model, filename) != [filename]:
        raise ValueError("[Test] Wrong fake save result")
    loaded = og.load(FakeModel, filename)
    if not isinstance(loaded, FakeModel) or loaded.name != "downstream":
        raise ValueError("[Test] Wrong fake load result")
    if og.is_loadable(FakeModel, filename).value() != 1:
        raise ValueError("[Test] Fake file should be loadable")
    if not og.is_saveable(model, filename) or not og.is_saveable(FakeModel, filename):
        raise ValueError("[Test] Fake file should be saveable")
    if og.is_saveable(FakeModel, "model.other"):
        raise ValueError("[Test] Fake class should not be saveable as .other")
    if og.input_extensions(FakeModel) != ["fake"] or og.output_extensions(FakeModel) != ["fake"]:
        raise ValueError("[Test] Wrong fake extensions")
    # Builders built with a static create (mesh builders) or the constructor
    mesh = FakeMesh()
    if og.builder(mesh).mesh is not mesh:
        raise ValueError("[Test] Mesh builder should be built with create")
    # Subclass instances use the registration of the closest registered base
    derived = DerivedFakeModel()
    if not isinstance(og.builder(derived), FakeModelBuilder):
        raise ValueError("[Test] Subclass should use its base builder")
    og.save(derived, filename)
    expect_raises(TypeError, og.load, DerivedFakeModel, filename)
    # A file holding several objects (or none) is not one object
    with open(filename, "w") as file:
        file.write("first\nsecond\n")
    error = expect_raises(og.OpenGeodeDataError, og.load, FakeModel, filename, contains="holds 2 objects")
    if error.type != "data":
        raise ValueError("[Test] Several objects should be a data error")
    if len(og.load_all(FakeModel, filename)) != 2:
        raise ValueError("[Test] load_all should return every object")
    with open(filename, "w") as file:
        file.write("")
    expect_raises(og.OpenGeodeDataError, og.load, FakeModel, filename, contains="holds 0 objects")
    # Unsupported operations of a partial registration
    expect_raises(TypeError, og.load, FakeMesh, filename, contains="load")
    expect_raises(TypeError, og.save, mesh, filename, contains="save")
    expect_raises(TypeError, og.is_saveable, FakeMesh, filename, contains="output_extensions")
    # Registration errors
    expect_raises(ValueError, og.register_type, FakeModel, load=fake_load, contains="already registered")
    expect_raises(TypeError, og.register_type, "FakeModel", load=fake_load)
    class Unregistered:
        pass

    expect_raises(TypeError, og.register_type, Unregistered, builder=42)
    expect_raises(TypeError, og.register_type, Unregistered, load="not callable")
    expect_raises(TypeError, og.register_type, Unregistered, contains="No operation")
    if Unregistered in og.registered_types():
        raise ValueError("[Test] A failed registration should register nothing")


# --- Attributes by name -------------------------------------------------------

ATTRIBUTE_VALUES = {
    "bool": (True, False, "Bool"),
    "int": (-3, 5, "Int"),
    "uint": (3, 7, "UInt"),
    "float": (1.5, 2.5, "Float"),
    "double": (2.25, 4.5, "Double"),
    "arraydouble2": ([1.0, 2.0], [3.0, 4.0], "ArrayDouble2"),
    "arraydouble3": ([1.0, 2.0, 3.0], [4.0, 5.0, 6.0], "ArrayDouble3"),
}


def check_attributes_by_name():
    manager = og.AttributeManager()
    manager.resize(3)
    for suffix, (default, other, class_suffix) in ATTRIBUTE_VALUES.items():
        for kind in ["constant", "variable", "sparse"]:
            name = f"{kind}_{suffix}"
            getattr(manager, f"create_attribute_{kind}_{suffix}")(name, default)
            attribute = og.find_attribute(manager, name)
            expected = getattr(og, f"{kind.capitalize()}Attribute{class_suffix}")
            if type(attribute) is not expected:
                raise ValueError(f"[Test] {name} should be a {expected.__name__}, got {type(attribute)}")
            if attribute.name() != name:
                raise ValueError(f"[Test] Wrong attribute name for {name}")
            if attribute.value(0) != default:
                raise ValueError(f"[Test] Wrong value of {name}")
            # The typed attribute is a mutable view of the stored one
            if kind == "constant":
                attribute.set_value(other)
            else:
                attribute.set_value(1, other)
            index = 0 if kind == "constant" else 1
            if og.find_attribute(manager, name).value(index) != other:
                raise ValueError(f"[Test] {name} should be modified in place")
    # On a mesh attribute manager
    mesh = og.TriangulatedSurface3D.create()
    og.builder(mesh).create_vertices(2)
    vertex_manager = mesh.vertex_attribute_manager()
    vertex_manager.create_attribute_variable_double("height", 1.0)
    if type(og.find_attribute(vertex_manager, "height")) is not og.VariableAttributeDouble:
        raise ValueError("[Test] Mesh attribute should be found by name")
    # Errors
    expect_raises(og.OpenGeodeDataError, og.find_attribute, manager, "missing", contains="missing")
    for time in [0.0, 1.0]:
        manager.create_time_step_attribute_variable_double(
            "series", time, og.AttributeValuesDouble(), og.AttributeProperties()
        )
    expect_raises(og.OpenGeodeDataError, og.find_attribute, manager, "series", contains="time_steps")
    # A single time step is found by its name
    manager.create_time_step_attribute_variable_double(
        "single", 0.0, og.AttributeValuesDouble(), og.AttributeProperties()
    )
    if type(og.find_attribute(manager, "single")) is not og.VariableAttributeDouble:
        raise ValueError("[Test] Single step should be found by name")


# --- Logger functions ---------------------------------------------------------


# --- Meshes typed by their base class (model components) -------------------


def check_component_meshes(directory, model, components):
    checked = set()
    for component in components:
        mesh = component.mesh()
        concrete = getattr(og, mesh.type_name().get())
        builder = og.builder(mesh)
        expected_builder = og.builder_type(concrete)
        if type(builder) is not expected_builder:
            raise ValueError(
                f"[Test] Builder of a {type(mesh).__name__} that is a {concrete.__name__} "
                f"should be a {expected_builder.__name__}, got {type(builder)}"
            )
        extension = og.output_extensions(concrete)[0]
        filename = os.path.join(directory, f"component.{extension}")
        if not og.is_saveable(mesh, filename):
            raise ValueError(f"[Test] {type(mesh).__name__} mesh should be saveable as {extension}")
        if og.is_saveable(mesh, os.path.join(directory, "component.unknown")):
            raise ValueError("[Test] Component mesh should not be saveable as .unknown")
        if og.save(mesh, filename) != [filename]:
            raise ValueError("[Test] Wrong files written for a component mesh")
        loaded = og.load(concrete, filename)
        if loaded.nb_vertices() != mesh.nb_vertices():
            raise ValueError(f"[Test] {concrete.__name__} component mesh round trip failed")
        # The bound operations of other libraries get the resolved object too
        if og.object_operation(mesh, "save")(filename) != [filename]:
            raise ValueError("[Test] object_operation should resolve the mesh")
        checked.add((type(mesh), concrete))
    return checked


def check_base_typed_meshes(directory):
    brep = og.load(og.BRep, os.path.join(DATA, "box_brep.og_brep"))
    section = og.load(og.Section, os.path.join(DATA, "quad.og_sctn"))
    checked = set()
    for model in [brep, section]:
        for components in [model.corners(), model.lines(), model.surfaces()]:
            checked |= check_component_meshes(directory, model, components)
    checked |= check_component_meshes(directory, brep, brep.blocks())
    # The component meshes are typed by their base class: the case resolved
    for base in [og.SurfaceMesh3D, og.SolidMesh3D, og.SurfaceMesh2D]:
        if not any(mesh_type is base and concrete is not base for mesh_type, concrete in checked):
            raise ValueError(f"[Test] No component mesh typed {base.__name__} was checked: {checked}")
    # The resolution keeps the component mesh alive and modifies it
    surface = brep.surfaces()[0].mesh()
    nb_vertices = surface.nb_vertices()
    builder = og.builder(surface)
    del surface
    builder.create_point(og.Point3D([0, 0, 0]))
    if brep.surfaces()[0].mesh().nb_vertices() != nb_vertices + 1:
        raise ValueError("[Test] The builder should modify the component mesh")


# --- Extension of a registration ----------------------------------------------


def check_extend_type(directory):
    def describe(model):
        return f"model {model.name}"

    og.extend_type(FakeModel, operations={"describe": describe})
    model = FakeModel("extended")
    if og.object_operation(model, "describe")() != "model extended":
        raise ValueError("[Test] Wrong extended operation")
    if og.type_operation(FakeModel, "describe") is not describe:
        raise ValueError("[Test] type_operation should return the operation")
    # Subclass instances use their closest registered base
    if og.object_operation(DerivedFakeModel("derived"), "describe")() != "model derived":
        raise ValueError("[Test] object_operation should use the closest base")
    expect_raises(TypeError, og.type_operation, DerivedFakeModel, "describe")
    expect_raises(TypeError, og.type_operation, FakeModel, "missing", contains="missing")
    expect_raises(TypeError, og.object_operation, model, "missing", contains="missing")
    expect_raises(TypeError, og.object_operation, 42, "save", contains="int")
    # An existing operation is only replaced on request
    expect_raises(ValueError, og.extend_type, FakeModel, operations={"describe": str}, contains="replace=True")
    expect_raises(ValueError, og.extend_type, FakeModel, save=fake_save, is_loadable=fake_load, contains="is_loadable, save")
    if og.type_operation(FakeModel, "describe") is not describe:
        raise ValueError("[Test] A refused extension should change nothing")
    og.extend_type(FakeModel, replace=True, operations={"describe": lambda model: "replaced"})
    if og.object_operation(model, "describe")() != "replaced":
        raise ValueError("[Test] replace=True should replace the operation")
    # A builder added to a registered type is used (and typed in the stubs)
    og.extend_type(FakeMesh, load=lambda filename: [FakeMesh()])
    if not isinstance(og.load(FakeMesh, "any.file"), FakeMesh):
        raise ValueError("[Test] Extended load should be used")
    # Other libraries add their operations to the OpenGeode types
    og.extend_type(og.BRep, operations={"nb_corners_of": lambda brep: brep.nb_corners()})
    if og.object_operation(make_brep(), "nb_corners_of")() != 2:
        raise ValueError("[Test] Operation added to BRep should be used")
    expect_raises(ValueError, og.extend_type, og.BRep, builder=og.BRepBuilder, contains="already has builder")
    # Errors
    class Unregistered:
        pass

    expect_raises(ValueError, og.extend_type, Unregistered, load=fake_load, contains="not registered")
    expect_raises(TypeError, og.extend_type, FakeModel, contains="No operation")
    expect_raises(TypeError, og.extend_type, FakeModel, operations={"load": fake_load}, contains="keyword")
    expect_raises(TypeError, og.extend_type, FakeModel, operations={"not valid": fake_load}, contains="Invalid")
    expect_raises(TypeError, og.extend_type, FakeModel, operations={"other": 42}, contains="not callable")
    expect_raises(TypeError, og.extend_type, "FakeModel", load=fake_load)
    expect_raises(TypeError, og.register_type, Unregistered, operations={"load": fake_load})
    # A custom operation can also be given at registration
    og.register_type(Unregistered, operations={"describe": lambda obj: "unregistered"})
    if og.object_operation(Unregistered(), "describe")() != "unregistered":
        raise ValueError("[Test] Custom operation given at registration")


def check_resolve():
    class Handle:
        """Downstream object standing for a concrete one, as a base-typed mesh."""

        def __init__(self, target):
            self.target = target

    class Concrete:
        pass

    class ConcreteBuilder:
        def __init__(self, concrete):
            self.concrete = concrete

    og.register_type(Concrete, builder=ConcreteBuilder, operations={"name": lambda obj: "concrete"})
    og.register_type(Handle, resolve=lambda handle: handle.target)
    concrete = Concrete()
    if og.builder(Handle(concrete)).concrete is not concrete:
        raise ValueError("[Test] builder should use the resolved object")
    if og.object_operation(Handle(concrete), "name")() != "concrete":
        raise ValueError("[Test] object_operation should use the resolved object")
    # Resolution is applied once: an unregistered result is an error
    expect_raises(TypeError, og.builder, Handle(42), contains="Handle")
    if og.builder_type(Handle) is not None:
        raise ValueError("[Test] builder_type does not resolve")


# --- Runtime signatures -------------------------------------------------------


def check_signatures():
    signature = inspect.signature(og.builder)
    parameters = list(signature.parameters.values())
    if [p.name for p in parameters] != ["obj"] or parameters[0].annotation is not object:
        raise ValueError(f"[Test] Wrong builder signature: {signature}")
    if signature.return_annotation is not typing.Any:
        raise ValueError(f"[Test] Wrong builder return annotation: {signature}")
    # The documentation keeps the typed overloads
    if "builder(obj: opengeode_py_mesh.TriangulatedSurface3D)" not in og.builder.__doc__.replace(
        og.TriangulatedSurface3D.__module__, "opengeode_py_mesh"
    ):
        raise ValueError("[Test] builder docstring should list the typed overloads")
    for name in [
        "load", "load_all", "save", "is_loadable", "is_saveable", "input_extensions",
        "output_extensions", "additional_files", "builder_type", "register_type",
        "extend_type", "registered_types", "type_operation", "object_operation",
        "find_attribute", "load_one",
    ]:
        function = getattr(og, name)
        for parameter in inspect.signature(function).parameters.values():
            if parameter.kind in (parameter.VAR_POSITIONAL, parameter.VAR_KEYWORD):
                raise ValueError(f"[Test] {name} should show its real parameters")


def check_attribute_fallbacks():
    # A value type without typed functions (geode::Point<3>): the generic one
    mesh = og.PointSet3D.create()
    og.builder(mesh).create_point(og.Point3D([1, 2, 3]))
    points = og.find_attribute(mesh.vertex_attribute_manager(), "points")
    if type(points) is not og.AttributeBase or points.name() != "points":
        raise ValueError(f"[Test] Unknown value type should give AttributeBase, got {type(points)}")
    # A kind without typed functions: find_read_only_attribute_<type>. No such
    # kind exists in OpenGeode: hide the variable class from the dispatch
    manager = og.AttributeManager()
    manager.resize(2)
    manager.create_attribute_variable_double("height", 1.5)
    hidden = basic.VariableAttributeDouble

    class Unknown:
        pass

    basic.VariableAttributeDouble = Unknown
    try:
        attribute = og.find_attribute(manager, "height")
    finally:
        basic.VariableAttributeDouble = hidden
    if not isinstance(attribute, og.ReadOnlyAttributeDouble) or attribute.value(1) != 1.5:
        raise ValueError(f"[Test] Unknown kind should give the read-only attribute, got {type(attribute)}")


def check_logger_functions():
    for name in [
        "forward_logs_to_python",
        "stop_forwarding_logs",
        "add_progress_listener",
        "remove_progress_listener",
    ]:
        function = getattr(og, name, None)
        if not callable(function) or not function.__doc__:
            raise ValueError(f"[Test] {name} should be a documented module function")
    for owner, name in [
        (og.Logger, "forward_to_python"),
        (og.Logger, "stop_forwarding"),
        (og.ProgressLogger, "add_listener"),
        (og.ProgressLogger, "remove_listener"),
    ]:
        if hasattr(owner, name):
            raise ValueError(f"[Test] {owner.__name__}.{name} is replaced by a module function")


if __name__ == "__main__":
    check_registered_types()
    check_extensions()
    with tempfile.TemporaryDirectory() as directory:
        for cls in TYPES:
            check_round_trip(directory, cls)
        check_external_time_series_round_trip(directory)
        check_errors(directory)
        check_registry_extension(directory)
        check_extend_type(directory)
        check_base_typed_meshes(directory)
    check_resolve()
    check_signatures()
    check_attribute_fallbacks()
    check_builders()
    check_load_one()
    check_attributes_by_name()
    check_logger_functions()
