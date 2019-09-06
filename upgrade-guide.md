# Upgrade Guide

## Upgrading from OpenGeode v1.x.x to v2.0.0

### Motivations

Faciliter la fabrication de model exploser/organiser les functionnalités en mixins.Flexible. Chaque mixins est indépendant

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

`GeoRepresentation` no long exists. BRep and Section are now inheriting from `Topology` and `AddComponents`. BRep and Section have nomore methods `relationships()` and `unique_vertex`. Moreover, overloaded methods `add_boundary_relation(...)` has been renamed and are not overloaded anymore.

**How to upgrade**

BRep and Section have nomore methods `relationships()` and `unique_vertex()`, but you can call (public) methods of `Relationships` and `VertexIdentifier` directly from an instance of `BRep` or `Section`, since they inherit from them. See `BRep` and `Section` definitions to have examples of `AddComponents` mixin usage. 

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

