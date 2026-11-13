/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "../numpy_access.hpp"
#include "../common.hpp"

#include <array>

#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

namespace geode::numpy_bindings
{
    using Index = geode::index_t;
    using IndexArray = pybind11::array_t< Index >;
    using OffsetArray = pybind11::array_t< uint64_t >;
    using ssize = pybind11::ssize_t;

    /*!
     * Return the mesh remembered by the builder (see builder_with_mesh()).
     * The bulk functions need it to check the indices before modifying
     * anything, so a builder without mesh is an error.
     */
    template < typename Mesh, typename Builder >
    const Mesh& builder_mesh(
        std::string_view context, const geode::TypedObject< Builder >& self )
    {
        if( !pybind11::hasattr( self, "_mesh" ) )
        {
            throw std::runtime_error( absl::StrCat( "[", context,
                "] the builder does not know its mesh: create it with "
                "Builder.create(mesh)" ) );
        }
        return self.attr( "_mesh" ).template cast< const Mesh& >();
    }

    /*!
     * Coordinates of the vertices of a mesh, (nb_vertices, dimension)
     */
    template < Index dimension, typename Mesh >
    pybind11::array_t< double > points_array( const Mesh& mesh )
    {
        const auto& crs = static_cast<
            const geode::CoordinateReferenceSystemManagers< dimension >& >(
            mesh );
        const auto nb_vertices = mesh.nb_vertices();
        return geode::make_array< double >(
            { static_cast< ssize >( nb_vertices ),
                static_cast< ssize >( dimension ) },
            [&]( double* data ) {
                for( const auto v : geode::Range{ nb_vertices } )
                {
                    const auto& point = crs.point( v );
                    for( const auto d : geode::LRange{ dimension } )
                    {
                        data[static_cast< size_t >( v ) * dimension + d] =
                            point.value( d );
                    }
                }
            } );
    }

    /*!
     * Raise a ValueError if the array has more rows than indices, before
     * any copy of the array
     */
    void check_nb_rows( std::string_view context,
        std::string_view name,
        const geode::ArrayLike& array )
    {
        if( static_cast< size_t >( array.shape( 0 ) )
            >= static_cast< size_t >( std::numeric_limits< Index >::max() ) )
        {
            throw pybind11::value_error(
                absl::StrCat( "[", context, "] too many ", name ) );
        }
    }

    /*!
     * Create the vertices of the given coordinates, (n, dimension), and
     * return the index of the first one.
     */
    template < Index dimension, typename Builder >
    Index create_points(
        geode::TypedObject< Builder > self, const geode::ArrayLike& points )
    {
        constexpr auto context = "create_points";
        if( geode::is_empty_input( points ) )
        {
            // An empty array, whatever its dtype and shape (e.g. []), creates
            // nothing
            return self.template cast< Builder& >().create_vertices( 0 );
        }
        geode::check_array_shape( context, "points", points,
            { -1, static_cast< ssize >( dimension ) } );
        check_nb_rows( context, "points", points );
        const auto coordinates =
            geode::copy_reals< double >( context, "points", points );
        const auto nb_points = static_cast< size_t >( points.shape( 0 ) );
        auto& builder = self.template cast< Builder& >();
        auto& crs_builder = static_cast<
            geode::CoordinateReferenceSystemManagersBuilder< dimension >& >(
            builder );
        pybind11::gil_scoped_release release;
        const auto first =
            builder.create_vertices( static_cast< Index >( nb_points ) );
        for( const auto i : geode::Range{ nb_points } )
        {
            std::array< double, dimension > values;
            for( const auto d : geode::LRange{ dimension } )
            {
                values[d] =
                    coordinates[static_cast< size_t >( i ) * dimension + d];
            }
            crs_builder.set_point( first + static_cast< Index >( i ),
                geode::Point< dimension >{ values } );
        }
        return first;
    }

    /*!
     * Vertices of each element of a mesh with the same number of vertices
     * per element, (nb_elements, nb_vertices)
     */
    template < Index nb_vertices, typename Mesh, typename Vertex >
    IndexArray elements_array(
        const Mesh& mesh, Index nb_elements, Vertex vertex_of )
    {
        return geode::make_array< Index >(
            { static_cast< ssize >( nb_elements ),
                static_cast< ssize >( nb_vertices ) },
            [&]( Index* data ) {
                for( const auto e : geode::Range{ nb_elements } )
                {
                    for( const auto v : geode::LRange{ nb_vertices } )
                    {
                        data[static_cast< size_t >( e ) * nb_vertices + v] =
                            vertex_of( mesh, e, v );
                    }
                }
            } );
    }

    /*!
     * Create the elements of the given vertices, (n, nb_vertices), and
     * return the index of the first one. The vertices are checked against the
     * vertices of the mesh before any element is created.
     */
    template < Index nb_vertices,
        typename Mesh,
        typename Builder,
        typename Count,
        typename Create >
    Index create_elements( std::string_view context,
        std::string_view name,
        geode::TypedObject< Builder > self,
        const geode::ArrayLike& elements,
        Count count,
        Create create )
    {
        const auto& mesh = builder_mesh< Mesh >( context, self );
        if( geode::is_empty_input( elements ) )
        {
            // An empty array, whatever its dtype and shape (e.g. []), creates
            // nothing
            return count( mesh );
        }
        geode::check_array_shape( context, name, elements,
            { -1, static_cast< ssize >( nb_vertices ) } );
        check_nb_rows( context, name, elements );
        const auto vertices = geode::copy_indices(
            context, name, "vertex", elements, mesh.nb_vertices() );
        const auto nb_elements = static_cast< Index >( elements.shape( 0 ) );
        const auto first = count( mesh );
        auto& builder = self.template cast< Builder& >();
        pybind11::gil_scoped_release release;
        for( const auto e : geode::Range{ nb_elements } )
        {
            std::array< Index, nb_vertices > element;
            for( const auto v : geode::LRange{ nb_vertices } )
            {
                element[v] =
                    vertices[static_cast< size_t >( e ) * nb_vertices + v];
            }
            create( builder, element );
        }
        return first;
    }

    /*!
     * Vertices of the polygons of a surface: the concatenated vertices of
     * all the polygons and the offsets of each polygon in them
     */
    template < Index dimension >
    std::tuple< IndexArray, OffsetArray > polygons_array(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        const auto nb_polygons = mesh.nb_polygons();
        std::vector< uint64_t > offsets(
            static_cast< size_t >( nb_polygons ) + 1 );
        {
            pybind11::gil_scoped_release release;
            for( const auto p : geode::Range{ nb_polygons } )
            {
                offsets[p + 1] = offsets[p] + mesh.nb_polygon_vertices( p );
            }
        }
        auto vertices = geode::make_array< Index >(
            { static_cast< ssize >( offsets.back() ) }, [&]( Index* data ) {
                for( const auto p : geode::Range{ nb_polygons } )
                {
                    for( const auto v :
                        geode::LRange{ mesh.nb_polygon_vertices( p ) } )
                    {
                        data[offsets[p] + v] = mesh.polygon_vertex( { p, v } );
                    }
                }
            } );
        auto result_offsets = geode::make_array< uint64_t >(
            { static_cast< ssize >( offsets.size() ) }, [&]( uint64_t* data ) {
                std::copy( offsets.begin(), offsets.end(), data );
            } );
        return { std::move( vertices ), std::move( result_offsets ) };
    }

    /*!
     * Create the polygons of a polygonal surface from the concatenated
     * vertices of all the polygons and the offsets of each polygon in them
     */
    template < Index dimension >
    Index create_polygons(
        geode::TypedObject< geode::PolygonalSurfaceBuilder< dimension > > self,
        const geode::ArrayLike& vertices,
        const geode::ArrayLike& offsets )
    {
        constexpr auto context = "create_polygons";
        geode::check_array_shape( context, "offsets", offsets, { -1 } );
        if( offsets.shape( 0 ) == 0 )
        {
            throw pybind11::value_error( absl::StrCat( "[", context,
                "] offsets must have at least one value (0 for no polygon)" ) );
        }
        const auto& mesh = builder_mesh< geode::PolygonalSurface< dimension > >(
            context, self );
        std::vector< Index > ids;
        // No vertex, whatever the dtype and shape of the array (e.g. [])
        if( vertices.size() != 0 )
        {
            geode::check_array_shape( context, "vertices", vertices, { -1 } );
            ids = geode::copy_indices(
                context, "vertices", "vertex", vertices, mesh.nb_vertices() );
        }
        const auto limits =
            geode::copy_integers< uint64_t >( context, "offsets", offsets );
        if( limits.front() != 0 || limits.back() != ids.size() )
        {
            throw pybind11::value_error( absl::StrCat( "[", context,
                "] offsets must start at 0 and end at the number of vertices (",
                ids.size(), "), got ", limits.front(), " and ",
                limits.back() ) );
        }
        for( const auto i : geode::Range{ limits.size() - 1 } )
        {
            if( limits[i + 1] < limits[i] || limits[i + 1] - limits[i] < 3
                || limits[i + 1] - limits[i] >= geode::NO_LID )
            {
                throw pybind11::value_error( absl::StrCat( "[", context,
                    "] polygon ", i, " must have between 3 and ",
                    geode::NO_LID - 1, " vertices, got offsets ", limits[i],
                    " and ", limits[i + 1] ) );
            }
        }
        const auto first = mesh.nb_polygons();
        auto& builder = self.template cast<
            geode::PolygonalSurfaceBuilder< dimension >& >();
        pybind11::gil_scoped_release release;
        for( const auto i : geode::Range{ limits.size() - 1 } )
        {
            builder.create_polygon( absl::MakeConstSpan( ids ).subspan(
                limits[i], limits[i + 1] - limits[i] ) );
        }
        return first;
    }

    template < typename Class >
    pybind11::class_< Class > bound_class(
        pybind11::module& module, const std::string& name )
    {
        return pybind11::reinterpret_borrow< pybind11::class_< Class > >(
            module.attr( name.c_str() ) );
    }

    constexpr auto points_doc =
        "Return the coordinates of all the vertices in the active coordinate "
        "reference system, as a new numpy array of float64 with shape "
        "(nb_vertices, dimension). The array is a copy: later changes of the "
        "mesh do not modify it";

    constexpr auto create_points_doc =
        "Create one vertex per row of points, an array of shape (n, "
        "dimension) of floats or integers of any dtype, set its coordinates "
        "in the active coordinate reference system and return the index of "
        "the first new vertex (the number of vertices before the call). An "
        "empty array, whatever its dtype and shape (for example []), creates "
        "nothing. Raise ValueError if the array does not have this shape or "
        "dtype or has 2**32 - 1 rows or more, in which case the mesh is not "
        "modified";

    std::string create_elements_doc(
        std::string_view singular, std::string_view plural, Index nb_vertices )
    {
        return absl::StrCat( "Create one ", singular, " per row of ", plural,
            ", an array of shape (n, ", nb_vertices,
            ") of integers of any dtype holding the indices of existing "
            "vertices, and return the index of the first new ",
            singular, " (the number of ", plural,
            " before the call). An empty array, whatever its dtype and shape "
            "(for "
            "example []), creates nothing. Raise ValueError if the array does "
            "not have this shape or dtype or has 2**32 - 1 rows or more, "
            "IndexError if a vertex is negative "
            "or not in [0, nb_vertices), in which case the mesh is not "
            "modified, and RuntimeError if the builder does not know its mesh "
            "(it was not given by Builder.create(mesh))" );
    }

    template < Index dimension >
    void define_numpy_access_dimension( pybind11::module& module )
    {
        const auto suffix = std::to_string( dimension ) + "D";
        bound_class< geode::PointSet< dimension > >(
            module, "PointSet" + suffix )
            .def( "points_array",
                &points_array< dimension, geode::PointSet< dimension > >,
                points_doc );
        bound_class< geode::EdgedCurve< dimension > >(
            module, "EdgedCurve" + suffix )
            .def( "points_array",
                &points_array< dimension, geode::EdgedCurve< dimension > >,
                points_doc );
        bound_class< geode::SurfaceMesh< dimension > >(
            module, "SurfaceMesh" + suffix )
            .def( "points_array",
                &points_array< dimension, geode::SurfaceMesh< dimension > >,
                points_doc )
            .def( "polygons_array", &polygons_array< dimension >,
                "Return the vertices of all the polygons as a pair "
                "(vertices, offsets) of new numpy arrays: vertices is a "
                "uint32 array with the vertices of the polygon 0, then the "
                "ones of the polygon 1... ; offsets is a uint64 array with "
                "nb_polygons + 1 values, the polygon i being "
                "vertices[offsets[i]:offsets[i + 1]] (offsets[0] is 0 and "
                "offsets[-1] is len(vertices)). The arrays are copies: later "
                "changes of the mesh do not modify them" );
        bound_class< geode::TriangulatedSurface< dimension > >(
            module, "TriangulatedSurface" + suffix )
            .def(
                "triangles_array",
                []( const geode::TriangulatedSurface< dimension >& mesh ) {
                    return elements_array< 3 >( mesh, mesh.nb_polygons(),
                        []( const auto& surface, Index triangle,
                            Index vertex ) {
                            return surface.polygon_vertex(
                                { triangle, static_cast< geode::local_index_t >(
                                                vertex ) } );
                        } );
                },
                "Return the three vertices of all the triangles, as a new "
                "numpy array of uint32 with shape (nb_triangles, 3). The "
                "array is a copy: later changes of the mesh do not modify "
                "it" );

        const auto create_triangles_doc =
            create_elements_doc( "triangle", "triangles", 3 );
        bound_class< geode::PointSetBuilder< dimension > >(
            module, "PointSetBuilder" + suffix )
            .def( "create_points",
                &create_points< dimension,
                    geode::PointSetBuilder< dimension > >,
                pybind11::arg( "points" ), create_points_doc );
        bound_class< geode::EdgedCurveBuilder< dimension > >(
            module, "EdgedCurveBuilder" + suffix )
            .def( "create_points",
                &create_points< dimension,
                    geode::EdgedCurveBuilder< dimension > >,
                pybind11::arg( "points" ), create_points_doc );
        bound_class< geode::SurfaceMeshBuilder< dimension > >(
            module, "SurfaceMeshBuilder" + suffix )
            .def( "create_points",
                &create_points< dimension,
                    geode::SurfaceMeshBuilder< dimension > >,
                pybind11::arg( "points" ), create_points_doc );
        bound_class< geode::TriangulatedSurfaceBuilder< dimension > >(
            module, "TriangulatedSurfaceBuilder" + suffix )
            .def(
                "create_triangles",
                []( geode::TypedObject<
                        geode::TriangulatedSurfaceBuilder< dimension > > self,
                    const geode::ArrayLike& triangles ) {
                    return create_elements< 3,
                        geode::TriangulatedSurface< dimension > >(
                        "create_triangles", "triangles", self, triangles,
                        []( const auto& mesh ) {
                            return mesh.nb_polygons();
                        },
                        []( auto& builder,
                            const std::array< Index, 3 >& vertices ) {
                            builder.create_triangle( vertices );
                        } );
                },
                pybind11::arg( "triangles" ), create_triangles_doc.c_str() );
        bound_class< geode::PolygonalSurfaceBuilder< dimension > >(
            module, "PolygonalSurfaceBuilder" + suffix )
            .def( "create_polygons", &create_polygons< dimension >,
                pybind11::arg( "vertices" ), pybind11::arg( "offsets" ),
                "Create one polygon per interval of offsets and return the "
                "index of the first new polygon (the number of polygons "
                "before the call; nothing is created if there is no "
                "polygon, and vertices may then be an empty array of any "
                "dtype such as []). vertices is a 1D array of integers of any "
                "dtype "
                "holding the indices of existing vertices, the concatenation "
                "of the vertices of all the polygons; offsets is a 1D array "
                "of integers of any dtype with nb_polygons + 1 values, the "
                "polygon i being vertices[offsets[i]:offsets[i + 1]] (the "
                "same layout as SurfaceMesh.polygons_array). Raise ValueError "
                "if an array is not 1D or not of integers, if offsets does "
                "not start at 0 or does not end at len(vertices), or if a "
                "polygon would have fewer than 3 vertices or NO_LID or more "
                "vertices, and IndexError if a vertex is negative or not in "
                "[0, nb_vertices), in which case the mesh is not modified, and "
                "RuntimeError if the builder does not know its mesh (it was "
                "not given by Builder.create(mesh))" );
    }

    void define_numpy_access_3d( pybind11::module& module )
    {
        bound_class< geode::SolidMesh< 3 > >( module, "SolidMesh3D" )
            .def( "points_array", &points_array< 3, geode::SolidMesh< 3 > >,
                points_doc );
        bound_class< geode::TetrahedralSolid< 3 > >(
            module, "TetrahedralSolid3D" )
            .def(
                "tetrahedra_array",
                []( const geode::TetrahedralSolid< 3 >& mesh ) {
                    return elements_array< 4 >( mesh, mesh.nb_polyhedra(),
                        []( const auto& solid, Index tetrahedron,
                            Index vertex ) {
                            return solid.polyhedron_vertex( { tetrahedron,
                                static_cast< geode::local_index_t >(
                                    vertex ) } );
                        } );
                },
                "Return the four vertices of all the tetrahedra, as a new "
                "numpy array of uint32 with shape (nb_tetrahedra, 4). The "
                "array is a copy: later changes of the mesh do not modify "
                "it" );
        bound_class< geode::Graph >( module, "Graph" )
            .def(
                "edges_array",
                []( const geode::Graph& mesh ) {
                    return elements_array< 2 >( mesh, mesh.nb_edges(),
                        []( const auto& graph, Index edge, Index vertex ) {
                            return graph.edge_vertex(
                                { edge, static_cast< geode::local_index_t >(
                                            vertex ) } );
                        } );
                },
                "Return the two vertices of all the edges, as a new numpy "
                "array of uint32 with shape (nb_edges, 2); an endpoint "
                "without vertex is NO_ID. The array is a copy: later "
                "changes of the mesh do not modify it" );

        const auto create_edges_doc = create_elements_doc( "edge", "edges", 2 );
        const auto create_tetrahedra_doc =
            create_elements_doc( "tetrahedron", "tetrahedra", 4 );
        bound_class< geode::SolidMeshBuilder< 3 > >(
            module, "SolidMeshBuilder3D" )
            .def( "create_points",
                &create_points< 3, geode::SolidMeshBuilder< 3 > >,
                pybind11::arg( "points" ), create_points_doc );
        bound_class< geode::GraphBuilder >( module, "GraphBuilder" )
            .def(
                "create_edges_with_vertices",
                []( geode::TypedObject< geode::GraphBuilder > self,
                    const geode::ArrayLike& edges ) {
                    return create_elements< 2, geode::Graph >(
                        "create_edges_with_vertices", "edges", self, edges,
                        []( const auto& mesh ) {
                            return mesh.nb_edges();
                        },
                        []( auto& builder,
                            const std::array< Index, 2 >& vertices ) {
                            builder.create_edge( vertices[0], vertices[1] );
                        } );
                },
                pybind11::arg( "edges" ), create_edges_doc.c_str() );
        bound_class< geode::TetrahedralSolidBuilder< 3 > >(
            module, "TetrahedralSolidBuilder3D" )
            .def(
                "create_tetrahedra",
                []( geode::TypedObject< geode::TetrahedralSolidBuilder< 3 > >
                        self,
                    const geode::ArrayLike& tetrahedra ) {
                    return create_elements< 4, geode::TetrahedralSolid< 3 > >(
                        "create_tetrahedra", "tetrahedra", self, tetrahedra,
                        []( const auto& mesh ) {
                            return mesh.nb_polyhedra();
                        },
                        []( auto& builder,
                            const std::array< Index, 4 >& vertices ) {
                            builder.create_tetrahedron( vertices );
                        } );
                },
                pybind11::arg( "tetrahedra" ), create_tetrahedra_doc.c_str() );
    }
} // namespace geode::numpy_bindings

namespace geode
{
    void define_numpy_access( pybind11::module& module )
    {
        numpy_bindings::define_numpy_access_dimension< 2 >( module );
        numpy_bindings::define_numpy_access_dimension< 3 >( module );
        numpy_bindings::define_numpy_access_3d( module );
    }
} // namespace geode
