# Upgrade Guide

## Upgrading from OpenGeode v4.x.x to v5.0.0

### Motivations

TODO

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

Remove the mesh parameter and get the retrun value

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
