# Upgrade Guide

## Upgrading from OpenGeode v2.x.x to v3.0.0

### Motivations

TODO


### Breaking Changes

- **BRep / Section**: methods accessing iterators for internal/embeddined Components are renamed more explicitly.

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

