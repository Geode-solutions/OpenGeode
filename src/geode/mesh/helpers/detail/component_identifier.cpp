/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/helpers/detail/component_identifier.h>

#include <queue>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/generic_solid_accessor.h>
#include <geode/mesh/helpers/generic_surface_accessor.h>

namespace
{
    template < typename Mesh, typename Propagator >
    void add_adjacents( const Mesh& mesh,
        const Propagator& propagator,
        geode::index_t element_id,
        std::queue< geode::index_t >& queue )
    {
        for( const auto f :
            geode::LRange{ mesh.nb_element_facets( element_id ) } )
        {
            if( const auto adj = mesh.element_adjacent( { element_id, f } ) )
            {
                if( propagator.identifier( adj.value() ) == geode::NO_ID )
                {
                    queue.emplace( adj.value() );
                }
            }
        }
    }

} // namespace

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        class Propagator
        {
            OPENGEODE_DISABLE_COPY_AND_MOVE( Propagator< Mesh > );

        public:
            virtual ~Propagator() = default;

            index_t identifier( index_t element ) const
            {
                OPENGEODE_ASSERT( element < identification_.size(),
                    "[Propagator::identifier] Wrong element index required" );
                return identification_[element];
            }

            index_t identify_elements()
            {
                for( const auto p : Indices{ identification_ } )
                {
                    if( identifier( p ) == NO_ID )
                    {
                        propagate( p, nb_components_ );
                        nb_components_++;
                    }
                }
                return nb_components_;
            }

            absl::FixedArray< std::vector< index_t > >
                identified_components() const
            {
                absl::FixedArray< std::vector< index_t > > components(
                    nb_components_ );
                for( const auto p : Indices{ identification_ } )
                {
                    components[identifier( p )].push_back( p );
                }
                return components;
            }

        protected:
            Propagator( const Mesh& mesh, index_t nb_elements )
                : mesh_( mesh ), identification_( nb_elements, NO_ID )
            {
            }

            const Mesh& mesh() const
            {
                return mesh_;
            }

        private:
            void propagate( index_t element_from, index_t tag_id )
            {
                std::queue< index_t > queue;
                queue.emplace( element_from );
                while( !queue.empty() )
                {
                    const auto cur_el = queue.front();
                    queue.pop();
                    if( identifier( cur_el ) != NO_ID )
                    {
                        continue;
                    }
                    identification_[cur_el] = tag_id;
                    add_adjacents( cur_el, queue );
                }
            }

            virtual void add_adjacents(
                index_t element_id, std::queue< index_t >& queue ) const = 0;

        private:
            const Mesh& mesh_;
            absl::FixedArray< index_t > identification_{};
            index_t nb_components_{ 0 };
        };

        class GraphIdentifier::Impl : public Propagator< Graph >
        {
        public:
            explicit Impl( const Graph& curve )
                : Propagator< Graph >( curve, curve.nb_vertices() )
            {
            }

        private:
            void add_adjacents(
                index_t vertex_id, std::queue< index_t >& queue ) const override
            {
                for( const auto& around :
                    this->mesh().edges_around_vertex( vertex_id ) )
                {
                    const auto other_vertex_id =
                        this->mesh().edge_vertex( around.opposite() );
                    if( this->identifier( other_vertex_id ) == NO_ID )
                    {
                        queue.emplace( other_vertex_id );
                    }
                }
            }
        };

        GraphIdentifier::GraphIdentifier( const Graph& graph ) : impl_( graph )
        {
        }

        GraphIdentifier::~GraphIdentifier() = default;

        index_t GraphIdentifier::identify_vertices()
        {
            return impl_->identify_elements();
        }

        index_t GraphIdentifier::vertex_identifier( index_t vertex_id ) const
        {
            return impl_->identifier( vertex_id );
        }

        absl::FixedArray< std::vector< index_t > >
            GraphIdentifier::identified_connected_vertices() const
        {
            return impl_->identified_components();
        }

        template < index_t dimension >
        class EdgedCurveIdentifier< dimension >::Impl
            : public Propagator< EdgedCurve< dimension > >
        {
        public:
            explicit Impl( const EdgedCurve< dimension >& curve )
                : Propagator< EdgedCurve< dimension > >(
                    curve, curve.nb_edges() )
            {
            }

        private:
            void add_adjacents(
                index_t edge_id, std::queue< index_t >& queue ) const override
            {
                for( const auto vertex_id :
                    this->mesh().edge_vertices( edge_id ) )
                {
                    for( const auto& around :
                        this->mesh().edges_around_vertex( vertex_id ) )
                    {
                        if( around.edge_id != edge_id
                            && this->identifier( around.edge_id ) == NO_ID )
                        {
                            queue.emplace( around.edge_id );
                        }
                    }
                }
            }
        };

        template < index_t dimension >
        EdgedCurveIdentifier< dimension >::EdgedCurveIdentifier(
            const EdgedCurve< dimension >& curve )
            : impl_( curve )
        {
        }

        template < index_t dimension >
        EdgedCurveIdentifier< dimension >::~EdgedCurveIdentifier() = default;

        template < index_t dimension >
        index_t EdgedCurveIdentifier< dimension >::identify_edges()
        {
            return impl_->identify_elements();
        }

        template < index_t dimension >
        index_t EdgedCurveIdentifier< dimension >::edge_identifier(
            index_t edge_id ) const
        {
            return impl_->identifier( edge_id );
        }

        template < index_t dimension >
        absl::FixedArray< std::vector< index_t > >
            EdgedCurveIdentifier< dimension >::identified_connected_edges()
                const
        {
            return impl_->identified_components();
        }

        template < index_t dimension >
        class SurfaceIdentifier< dimension >::Impl
            : public Propagator< SurfaceMesh< dimension > >
        {
        public:
            explicit Impl( const SurfaceMesh< dimension >& surface )
                : Propagator< SurfaceMesh< dimension > >(
                    surface, surface.nb_polygons() )
            {
            }

        private:
            void add_adjacents( index_t polygon_id,
                std::queue< index_t >& queue ) const override
            {
                ::add_adjacents(
                    GenericMeshAccessor< SurfaceMesh< dimension > >{
                        this->mesh() },
                    *this, polygon_id, queue );
            }
        };

        template < index_t dimension >
        SurfaceIdentifier< dimension >::SurfaceIdentifier(
            const SurfaceMesh< dimension >& surface )
            : impl_( surface )
        {
        }

        template < index_t dimension >
        SurfaceIdentifier< dimension >::~SurfaceIdentifier() = default;

        template < index_t dimension >
        index_t SurfaceIdentifier< dimension >::identify_polygons()
        {
            return impl_->identify_elements();
        }

        template < index_t dimension >
        index_t SurfaceIdentifier< dimension >::polygon_identifier(
            index_t polygon ) const
        {
            return impl_->identifier( polygon );
        }

        template < index_t dimension >
        absl::FixedArray< std::vector< index_t > >
            SurfaceIdentifier< dimension >::identified_connected_polygons()
                const
        {
            return impl_->identified_components();
        }

        class SolidIdentifier::Impl : public Propagator< SolidMesh3D >
        {
        public:
            explicit Impl( const SolidMesh3D& solid )
                : Propagator( solid, solid.nb_polyhedra() )
            {
            }

        private:
            void add_adjacents( index_t polyhedron_id,
                std::queue< index_t >& queue ) const override
            {
                ::add_adjacents(
                    GenericMeshAccessor< SolidMesh3D >{ this->mesh() }, *this,
                    polyhedron_id, queue );
            }
        };

        SolidIdentifier::SolidIdentifier( const SolidMesh3D& solid )
            : impl_( solid )
        {
        }

        SolidIdentifier::~SolidIdentifier() = default;

        index_t SolidIdentifier::identify_polyhedra()
        {
            return impl_->identify_elements();
        }

        index_t SolidIdentifier::polyhedron_identifier(
            index_t polyhedron ) const
        {
            return impl_->identifier( polyhedron );
        }

        absl::FixedArray< std::vector< index_t > >
            SolidIdentifier::identified_connected_polyhedra() const
        {
            return impl_->identified_components();
        }

        template class opengeode_mesh_api EdgedCurveIdentifier< 1 >;

        template class opengeode_mesh_api EdgedCurveIdentifier< 2 >;
        template class opengeode_mesh_api SurfaceIdentifier< 2 >;

        template class opengeode_mesh_api EdgedCurveIdentifier< 3 >;
        template class opengeode_mesh_api SurfaceIdentifier< 3 >;
    } // namespace detail
} // namespace geode