# Upgrade Guide

## Upgrading from OpenGeode v1.x.x to v2.0.0

TBD...

## Upgrading from OpenGeode v0.x.x to v1.0.0

#### Breaking Changes

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

