# Upgrade Guide

## Upgrading from OpenGeode v13.x.x to v14.0.0

### Motivations

OpenGeode ecosystem has grown and needs a refresh on some of our software conventions to ease and simplify future extensions.

### Breaking Changes

- **Library**: the library naming convention changed. The `OPENGEODE_LIBRARY` and `OPENGEODE_LIBRARY_IMPLEMENTATION` automatically add a `Library` suffix. For example, the library defined by `OPENGEODE_LIBRARY( opengeode_basic_api, OpenGeodeBasic );` is usable with `OpenGeodeBasicLibrary::initialize()`.

- **Python**: the inside of the python wheels has been changed. There is now a folder folder containing the libraries (named bin or lib or lib64 acording the compiling OS) and an optional folder for addiontal data (named share). All other python files stay at the wheel root. all of this is hanlde by the `add_geode_python_wheel` cmake function.

## Upgrading from OpenGeode v12.x.x to v13.0.0

### Motivations

This release introduces a new angular epsilon to increase robustness of computations such as triangle normal.
These operations now return an optional of their previous return type.

The second important feature is the new Image library that introduces the `RasterImage` data model.

### Breaking Changes

- **Basic**: method `extension_from_filename` is now located in `filename.h`

- **AABB**: intersection functors now returns a boolean to indicate if the search should be stopped

- **MeshHelpers**: `merge_surface_mesh` has been removed

- **MeshHelpers**: Add a parameter to choose the coordinate value of the added axis in mesh (PointSet, EdgedCurve, SurfaceMesh) converter functions from 2D to 3D.

- **ModelHelpers**: Add a parameter to choose the coordinate value of the added axis in Section to BRep converter function.

- **Point**: `inexact_equal` method only take the other Point as parameter

- **Bitsery**: remove `DefaultGrowable`, use `Growable` instead

- **Mesh**: OpenGeode mesh implementations (datamodel and builder) are moved into a subfolder `geode` (i.e. file like `geode_point_set.h` and `geode_point_set_builer.h`)

- **IO**: base classes for input and output file are moved into the basic library

- **Grid**: `Grid` has been refactored using the new `CellArray` class

- **AddComponents**: `AddComponents` and `AddComponentsBuilder` have been removed. VisualStudio cannot compile move constructor using these helpers.

- **Triangle/SurfaceMesh**: all methods related to normal computation now return an optional.

- **GenericInfiniteLine**: `GenericInfiniteLine` has been renamed `GenericLine`

- **Distance**: `point_segment_distance`, `point_line_distance` and `point_line_signed_distance` now return only the distance. Use projection functions to know the closest point.


## Upgrading from OpenGeode v11.x.x to v12.0.0

### Motivations

The goal is to provide a build system with full static linking support. This way, a single executable can embed the entire OpenGeode ecosystem without having to deploy the numerous shared libraries.

To allow this feature, each library needs to be explicitly initialized at the begining of the execution (for registering into factories for example).

### Breaking Changes

- **CMake**: Minimum required version bump to 3.15

- **Library**: Each library/OpenGeode module needs to use the new macro pairs `OPENGEODE_LIBRARY`/`OPENGEODE_LIBRARY_IMPLEMENTATION` defined in the basic/library.h header file. They replace the macro `OPENGEODE_LIBRARY_INITIALIZE`. Examples can be found in the common.h and common.cpp of each OpenGeode library.

- **Mapping**: Header file `<geode/model/representation/builder/copy_mapping.h>` no longer exists, use `<geode/model/representation/core/mapping.h>` instead. Moreover, the class `ModelCopyMapping` cannot be forward declared anymore. Add the corresponding include in your files.

## Upgrading from OpenGeode v10.x.x to v11.0.0

### Motivations

Some cleanup was required in several classes to improve naming and clarify usage.
A lot of new features were added, very few with breaking changes, and all of them documented in the release changelog.

The largest breaking change is the refactoring and integration of the RegularGrid into the Mesh hierarchy.
The whole design of this class has been reshaped to be more aligned with the other Mesh classes including abstract interface, builder...

### Breaking Changes

- **BasicObjects**: `Tetra` and related classes have been renamed `Tetrahedron`. The header `basic_objets.h` has been removed and replaced by several files located in the `geode/geometry/basic_objects` folder. Methods containing the shorthand `tetra` have been renamed `tetrahedron` too (e.g. `point_tetra_distance` in `point_tetrahedron_distance`).

- **VertexIdentifier**: `MeshComponentVertex` has been renamed `ComponentMeshVertex` and its methods `mesh_component_vertices` has been renamed `component_mesh_vertices`.

- **PointsSort**: `morton_sort` has been renamed `morton_mapping`.

- **IO**: Input factories return the loaded Object and Output factories take the Object as parameter.

- **Geometry**: `sign_mensuration.h` has been renamed `mensuration.h`.

- **MeshView**: all MeshView and related classes have been removed, use other classic Mesh instead.

- **ComponentMeshQueries**: `component_mesh_queries.h` has been renamed `component_mesh_polygons.h` and `surface_polygon_unique_vertices` has been renamed `polygon_unique_vertices`.

- **SolidMesh**: `polyhedra_from_facet` has been renamed `polyhedra_from_facet_vertices` and `PolyhedraAroundFacet` is storing `PolyhedronFacet` instead of `index_t`.

## Upgrading from OpenGeode v9.x.x to v10.0.0

### Motivations

This release is mostly to upgrade Microsoft Visual Studio version from 2017 to 2019. VS2022 is also part of the integration workflow.
The other important change is the shift of the `global_epsilon` value from 1E-8 to 1E-6. This change was made after heavy testing on numerical approximation limits which showed 1E-8 was too low.
The `RegularGrid` interface was also upgraded for a simplified and more efficient use.

### Breaking Changes

- **MeshBuilder**: `MeshBuilder::delete_vertices` has been removed for all mesh classes except `VertexSet` and `PointSet`. To remove vertices, use `MeshBuilder::delete_isolated_vertices` instead.

- **Relationships**: Remove the `AttributeManger` on the component and related methods.

- **RelationshipsBuilder**: Remove the `register_component` methods, the registration is now done automatically. Methods to create relations take a `ComponentID` instead of an `uuid`.

- **RegularGrid**: The grid accessors structures were renamed: `GridCellIndices` to `GridCellsAroundVertex`, `GridCellIndex` to `GridCellIndices`, `GridVertexIndex` to `GridVertexIndices`. The following functions were renamed accordingly: `cell_index(index_t)` to `cell_indices(index_t)`, `vertex_index(index_t)` to `vertex_indices(index_t)`. The deprecated `Index` and `Indices` structures were removed, as well as the deprecated function cell_size(index_t). The following functions were removed to avoid multiple definitions: is_vertex_on_border(index_t) and cell_vertex_index(index_t). In order to remove optionals of containers, the `cell(Point)` function now returns a `GridCellsAroundVertex` object instead of an optional to this object, and was renamed `cells(Point)`.

- **global_epsilon**: The `global_epsilon` value has been changed from 1E-8 to 1E-6 to better find numerical approximation. Also, `global_epsilon2` and `global_epsilon3` have been removed because they make no practical sense. Use  `global_epsilon` instead.

- **Mesh**: The methods `isolated_XXX` (e.g. `isolated_vertex`) have been renamed `is_XXX_isolated`.

- **SolidMesh**: Static method `type_name_static` has been removed in this class since `SolidMesh` is not a complete type.

## Upgrading from OpenGeode v8.x.x to v9.0.0

### Motivations

This release includes a revised version of the `Relationships` model mixin.
The goal is to provide `Attributes` to ̀`Component` level, both on each `Component` (via `component_attribute_manager`) and each relationship between two `Components` (via `relation_attribute_manager`).

Another addition is the `Identifier` class holding a `uuid` and a name. 
This new class is now the parent class of `VertexSet`, `Component`, `BRep` and ̀`Section`.
All these derived classes can now be named and identified with a `uuid`.

### Breaking Changes

- **ModelBuilder**: The `copy_component_relationships` method in both `BRepBuilder` and `SectionBuilder` have been removed in favor of the `copy_relationships` in the new `IdentifierBuilder`.

- **ModelCopyMapping**: The `ModelCopyMapping` has been move from `geode::detail` namespace to only `geode`.


## Upgrading from OpenGeode v7.x.x to v8.0.0

### Motivations

The main goal of this release is to reduce memory consumption by introducting two new features: 
1) a new type `local_index_t` which can be used for indexing inferior to 255. This type is now used in PolygonVertex, PolygonEdge, PolyhedronVertex and all other struct similar to these ones. 
2) a new MeshType `HybridSolid` which can only stores tetrahedra, hexahedra, prisms and pyramids. This restriction allows several storage optimizations which highly reduce memory usage and increase performance.

### Breaking Changes

- **Mesh**: The introdcution of the `local_index_t` in all the struct can imply some changes in the client code to update the new type usage.

- **Mesh helpers**: The `aabb_triangulated_surface_helpers` h/cpp files are renamed `aabb_surface_helpers`.


## Upgrading from OpenGeode v6.x.x to v7.0.0

### Motivations

There are two main goals in this new major release: 1) Simplifying the data model of SurfaceMesh and Solid by extract the edges and facets into separated, and now optional, classes; 2) Ensure backward compatibility of native OpenGeode files while allowing new custom types to be added.

### Breaking Changes

- **Mesh**: SurfaceMesh and SolidMesh API corresponding to edges and facets have been moved into SurfaceEdges, SolidEdges and SolidFacets classes. They can be accessed with the methods `edges()` and `facets()`. By default, edges and facets are now disabled. Accessing disabled edges or facets will raise an exception. If you want to use them, you can call `enable_edges()` and/or `enable_facets()`.

- **Files**: native files saved before v7 are no longer compatible. The new serialize framework will now prevent any breaking change in the future.

- **Model loading**: model loading functions are now returning the model loaded from the filename.


## Upgrading from OpenGeode v5.x.x to v6.0.0

### Motivations

This new major release is to formalize PyPI support for OpenGeode.

### Breaking Changes

- **CMake**: CMake minimum requirement has been upgraded to 3.14 to ease import of PyPI modules.


## Upgrading from OpenGeode v4.x.x to v5.0.0

### Motivations

The main change of this release is a redesign of the Mesh class hierarchy. PolygonalSurface and TriangulatedSurface are now inherited from a new abstract class: SurfaceMesh. Same change for PolyhedralSolid and TetrahedralSolid with SolidMesh. This modification will ease addition of new mesh type classes.

A second change is to remove all usage of constant expression NO_ID in the API. If a value is not supposed to be returned or initialized, we now return an optional value (e.g. polygon/polyhedron adjacent index is either the adjacent value or nothing).

### Breaking Changes

- **Model copy**: mappings between model components have been modified to open design to arbitrary collections. It means that you can add additional information to these mappings for your own copy functions.

**How to upgrade**

`ComponentMapping` has been moved out from `geode::BRep` and `geode::Section` and renamed `geode::detail::ModelCopyMapping` declares and defined in `include/geode/model/representation/builder/detail/copy.h`. 

`geode::detail::ModelCopyMapping` works as a map whose keys are `geode::ComponentType`. It implies to get access to a specific mapping using for example:

```
const auto& corner_mapping = mappings.at( Corner3D::component_type_static() );
```

To add a mapping between two components, use:
```
auto& corner_mapping = mappings.at( Corner3D::component_type_static() );
corner_mapping.emplace( uuid1, uuid2 );
```

To get a mapping between two components, use:
```
const auto& corner_mapping = mappings.at( Corner3D::component_type_static() );
corner_mapping.in2out( uuid1 );
```

- **PolygonsAroundVertex**: use PolygonVertex instead of PolygonEdge: `using PolygonsAroundVertex = absl::InlinedVector< PolygonVertex, 10 >`

**How to upgrade**

When using element of `PolygonsAroundVertex` (before `PolygonEdge`, now `PolygonVertex`), use `vertex_id` instead of `edge_id`.

- **Mesh loading**: mesh loading functions are now returning the mesh loaded from the filename.

**How to upgrade**

Remove the mesh parameter and get the return value. You can still use the MeshImpl to select which data structure to instanciate.

Example

from

```
auto new_edged_curve = geode::EdgedCurve3D::create();
or
auto new_edged_curve = geode::EdgedCurve3D::create( geode::OpenGeodeEdgedCurve3D::type_name_static() );

load_edged_curve( *new_edged_curve, filename );
```

to 

```
auto new_edged_curve = geode::load_edged_curve< 3 >( filename );
or
auto new_edged_curve = geode::load_edged_curve< 3 >( geode::OpenGeodeEdgedCurve3D::type_name_static(), filename );
```

- **Mesh & Builder factories**: Mesh and MeshBuilder factories has been merged into one.

**How to upgrade**

Example for PointSet:

from

```
PointSetFactory2D::register_creator< OpenGeodePointSet2D >( OpenGeodePointSet2D::type_name_static() );

and

PointSetBuilderFactory2D::register_creator< OpenGeodePointSetBuilder2D >( OpenGeodePointSet2D::type_name_static() );
```

to 

```
MeshFactory::register_mesh< OpenGeodePointSet2D >( PointSet2D::type_name_static(), OpenGeodePointSet2D::impl_name_static() );
or
MeshFactory::register_default_mesh< OpenGeodePointSet2D >( PointSet2D::type_name_static(), OpenGeodePointSet2D::impl_name_static() );

and

MeshBuilderFactory::register_mesh_builder< OpenGeodePointSetBuilder2D >( OpenGeodePointSet2D::impl_name_static() );
```
- **Embedding relationship**: renaming `BRep`and `Section` item-related methods for removing overloading of derived class method in the aim to simplify client code syntax

**How to upgrade**

For example, replace in `Section` and `BRep`: `geode::Section::items(...)` by `geode::Section::model_boundary_items(...)` 


- **Embedding relationship**: renaming embedded relationship to embedding relationship for better meaning

**How to upgrade**

For example, replace in `Section` and `BRep`: `geode::Section::embedded_surfaces(...)` by `geode::Section::embedding_surfaces(...)` 

## Upgrading from OpenGeode v3.x.x to v4.0.0

### Motivations

This new release is about improving performance.
After many profiling, we decided to introduce Abseil (https://abseil.io) as one of our dependency.
This project (maintained and used by Google) has an important focus on performance.
By adding it to OpenGeode, we allow us to use tailored containers and algorithms to improve our efficiency.

CMake minimum required version is bumped to 3.11 to ease third parties integration. Moreover, our CMake functions were using unconventional design, they have been rewritten for easier usage.

### Breaking Changes

- **absl::optional**: equivalent of std::optional (c++17) provided by Abseil and used for return values that may not always return a value

**How to upgrade**

Follow reference guide on how to use `optional` https://en.cppreference.com/w/cpp/utility/optional.

Example

from

```
bool found;	    
geode::PolygonEdge polygon_edge;	
std::tie( found, polygon_edge ) = polygonal_surface.polygon_edge_from_vertices( 3, 5 );
if( found )
{
    // do something with polygon_edge
}
```

to 

```
auto polygon_edge = polygonal_surface.polygon_edge_from_vertices( 3, 5 );
if( polygon_edge )
{
    // do something with polygon_edge.value() or polygon_edge->XXX
}
```

- **absl::InlinedVector**: advanced container to be used like a `std::vector`

- **absl::FixedArray**: advanced container to be used like a `std::array` with size known at runtime

- **absl::string_view**: equivalent of std::string_view (c++17) provided by Abseil and used as replacement for a reference to a `std::string` or `char*`.

**How to upgrade**

Follow reference guide on how to use `string_view` https://en.cppreference.com/w/cpp/string/basic_string_view.
Be careful on the lifetime of the `std::string` or `char*` you referenced in the `string_view`.

- **Section**: methods accessing iterators for embedded Components are renamed more explicitly.

**How to upgrade**

Replace in `Section`: `geode::Section::embeddings(...)` by `geode::Section::embedded_surfaces(...)` 

- **CMake**: redesign of CMake function `add_geode_library`

**How to upgrade**

Example

from

```
add_geode_library(geode/basic)
// with a CMakeLists.txt in src/geode/basic defining some variables and the dependencies
set(sources
    "${lib_source_dir}/assert.cpp"
    "${lib_source_dir}/attribute_manager.cpp"
    ...
)
set(public_headers
    "${lib_include_dir}/algorithm.h"
    "${lib_include_dir}/assert.h"
    ...
)
set(advanced_headers
    "${lib_include_dir}/detail/mapping_after_deletion.h"
    ...
)
target_link_libraries(${target_name}
    PUBLIC	    
        Bitsery::bitsery	       
    PRIVATE	   
        spdlog::spdlog_header_only	     
        ghcFilesystem::ghc_filesystem	
        MINIZIP::minizip	       
)
```

to 

```
add_subdirectory(src/geode/basic)
// with a CMakeLists.txt in src/geode/basic defining the library
add_geode_library(
    NAME basic
    FOLDER "geode/basic"
    SOURCES
        "assert.cpp"
        "attribute_manager.cpp"
        ...
    PUBLIC_HEADERS
        "algorithm.h"
        "assert.h"
        ...
    ADVANCED_HEADERS
        "detail/mapping_after_deletion.h"
        ...
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery	       
    PRIVATE_DEPENDENCIES
        spdlog::spdlog_header_only	 
        ghcFilesystem::ghc_filesystem
        MINIZIP::minizip	       
)
```

- **CMake**: redesign of CMake function `add_geode_test`

**How to upgrade**

Example

from

```
add_geode_test(test-algorithm.cpp ${PROJECT_NAME}::basic)
```

to 

```
add_geode_test(
    SOURCE "test-algorithm.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
)
```

- **CMake**: remove `copy_windows_binaries` function

**How to upgrade**

For testing, there is nothing to upgrade, just run CTest or RUN_TESTS on Visual Studio as usual.

For running a single executable, add the environment variables correponding to your operating system (e.g. PATH, LD_LIBRARY_PATH...).


## Upgrading from OpenGeode v2.x.x to v3.0.0

### Motivations

OpenGeode evolves and will keep evolving. 
To support this evolution, we introduce a full backward compatibility system to all our native file formats (.og_*). 
Any file saved since v3 will be loadable in any new OpenGeode version.

To increase the component relationships design flexibility, we allow relationships between any component types.
For example, we can register a relation between a `Line` inside a `Block` representing a well path.

Attribute interface has been revised to segregate read-only from write modifications.
We provide new ways to modify in-place an attribute value.

A new library named `geometry` was created to improve organization of C++ objects related to geometry concepts.


### Breaking Changes

- **Serialization**: all native file formats (.og_*) saved before v3 are no longer compatible, don't worry it will be the last time ;-)

- **BRep / Section**: methods accessing iterators for internal/embedded Components are renamed more explicitly.

**How to upgrade**

Replace in `BRep` and `Section`:
- `geode::BRep::internals(...)` by either `geode::BRep::internal_corners(...)` or `geode::BRep::internal_lines(...)` or `geode::BRep::internal_surfaces(...)`
- `geode::BRep::embeddings(...)` by either `geode::BRep::embedded_surfaces(...)` or `geode::BRep::embedded_blocks(...)`

- **Relationships**: replace the component registration key from uuid to ComponentID.

**How to upgrade**

Replace `id0` and `id1` in  `geode::RelationshipsBuilder::register_component( id0, id1 )` by their ComponentIDs.

- **Solid Facets & Surface Edges**: Edge/Facet indices are used as parameters of methods like `PolygonalSurface< dimension >::edge_length`, `PolyhedralSolid< dimension >::facet_barycenter`.

**How to upgrade**

Example

from

```
PolygonEdge polygon_edge{ 0, 0 };
auto edge_length = surface.polygon_edge_length( polygon_edge );
```

to 

```
index_t edge_id = polygon_edge( { 0, 0 } );
auto edge_length = surface.edge_length( edge_id );
```

- **Geometry**: new library called geometry gathering files related to geometry: bounding_box, nn_search, point and vector.

**How to upgrade**

Add `OpenGeode::geometry` to use this library. Update the path of OpenGeode files you include.

- **Attributes**: force Attribute API for writing by removing the reference access to attribute values

**How to upgrade**

Example using `VariableAttribute< double >` and modifying the `index_t id` value to `double new_value`
```
attribute.value( id ) = new_value;
```

to 

```
attribute.set_value( id, new_value );
```

Example using `VariableAttribute< std::vector< double > >` and modifying the `index_t id` value by adding `double new_value`
```
attribute.value( id ).push_back( new_value );
```

to 

```
attribute.modify_value( id, [&new_value]( std::vector< double >& values ) { values.push_back( new_value ); } );
```

- **BRepBuilder / SectionBuilder**: methods adding relationships between model Components have now explicitely the relationship type in their name.

**How to upgrade**

Replace:
-`geode::SectionBuilder::add_corner_line_relationship(...)` by `geode::SectionBuilder::add_corner_line_boundary_relationship(...)`
-`geode::SectionBuilder::add_line_surface_relationship(...)` by `geode::SectionBuilder::add_line_surface_boundary_relationship(...)`
-`geode::BRepBuilder::add_corner_line_relationship(...)` by `geode::BRepBuilder::add_corner_line_boundary_relationship(...)`
-`geode::BRepBuilder::add_line_surface_relationship(...)` by `geode::BRepBuilder::add_line_surface_boundary_relationship(...)`
-`geode::BRepBuilder::add_surface_block_relationship(...)` by `geode::BRepBuilder::add_surface_block_boundary_relationship(...)`


## Upgrading from OpenGeode v1.x.x to v2.0.0

### Motivations

In this new major version of OpenGeode, we aim at easing model objects conception. We organize basic features in independant mixins. Model conception is therefore very flexible ans extensible by combining mixins and/or defining new mixins.


### Breaking Changes

- **Model**: the library georepresentation has been renamed into model and files have been reorganized by adding two folders: mixin and representation.

**How to upgrade**

Replace `OpenGeode::georepresentation` by `OpenGeode::model` if you link with this library. Update the path of OpenGeode files you include.

- **Relationships and VertexIdentifier**: `RelationshipManager` has been renamed into `Relationships`. Methods for registering and unregistering components in `Relationships` and in `VertexIdentifier` have been renamed.  This two classes are provided with associated Builders that are able to modify them: `RelationshipsBuilder` and `VertexIdentifierBuilder`

**How to upgrade**

Replace:
- `geode::RelationshipManager` by `geode::Relationships`. 
-`geode::RelationshipManager::add_component(...)` by `geode::RelationshipsBuilder::register_component(...)`
-`geode::RelationshipManager::remove_component(...)` by `geode::RelationshipsBuilder::unregister_component(...)`
- `geode::VertexIdentifier::register_component(...)` by `geode::VertexIdentifierBuilder::register_mesh_component(...)`
- `geode::VertexIdentifier::remove_component(...)` by `geode::VertexIdentifierBuilder::unregister_mesh_component(...)`

You must call these methods only from Builders since they modify either `Relationships` or `VertexIdentifier`. You can also called them from classes `BRepBuilder` and `SectionBuilder` that inherits from `RelationshipsBuilder` and `VertexIdentifierBuilder` from `TopologyBuilder`.

- **BRep and Section**:

`GeoRepresentation` no longer exists. BRep and Section are now inheriting from `Topology` and `AddComponents`. BRep and Section have no more methods `relationships()` and `unique_vertex`. Moreover, overloaded methods `add_boundary_relation(...)` has been renamed and are not overloaded anymore.

**How to upgrade**

BRep and Section have no more methods `relationships()` and `unique_vertex()`, but you can call (public) methods of `Relationships` and `VertexIdentifier` directly from an instance of `BRep` or `Section`, since they inherit from them. See `BRep` and `Section` definitions to have examples of `AddComponents` mixin usage. 

Replace `add_boundary_relation( ... )` by `add_x_y_relationships( const X& x, const Y& y)` with `x` and `y` being `corner`, `line`, `surface` or `block` depending of the case.

## Upgrading from OpenGeode v0.x.x to v1.0.0

### Breaking Changes

- **Bitsery 5.0.0**: upgrading to Bitsery 5.0.0 (serialization library). Previously saved OpenGeode objects may potentially not be reloaded. 

- **VertexIdentifier**: the signature of VertexIdentifier::remove_component has changed from

```
void remove_component( const uuid& component_id )
```

to 

```
template < typename MeshComponent > 
void remove_component( const MeshComponent& component )
```
**How to upgrade**

To upgrade to OpenGeode v1.0.0. you have to modify your code as done in the following example.

*v0.x.x:*

```
const geode::Line3D& line = get_a_line();
vertex_identifier.remove_component( line.id() );
```

*v1.0.0:*

```
const geode::Line3D& line = get_a_line();
vertex_identifier.remove_component( line );
```
