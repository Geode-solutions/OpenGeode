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

#include <geode/mesh/helpers/gradient_computation.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/internal/grid_shape_function.hpp>
namespace
{
    template < typename Mesh >
    class ScalarGradientComputer
    {
    public:
        ScalarGradientComputer(
            const Mesh& mesh, const geode::uuid& scalar_function_id )
            : mesh_( mesh ), vertex_has_value_( mesh.nb_vertices(), true )
        {
            initialize_attribute_and_id( scalar_function_id );
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( std::isnan( scalar_function_->value( vertex_id ) ) )
                {
                    vertex_has_value_[vertex_id] = false;
                }
            }
        }

        ScalarGradientComputer( const Mesh& mesh,
            const geode::uuid& scalar_function_id,
            absl::Span< const geode::index_t > no_value_vertices )
            : mesh_( mesh ), vertex_has_value_( mesh.nb_vertices(), true )
        {
            initialize_attribute_and_id( scalar_function_id );
            for( const auto vertex_id : no_value_vertices )
            {
                vertex_has_value_[vertex_id] = false;
            }
        }

        std::tuple< geode::uuid, std::vector< geode::index_t > >
            compute_scalar_function_gradient() const
        {
            const auto gradient_function_name =
                absl::StrCat( scalar_function_->name().value(), "_gradient" );
            auto gradient_function_id =
                mesh_.vertex_attribute_manager()
                    .template create_attribute< geode::VariableAttribute,
                        geode::Vector< Mesh::dim > >(
                        gradient_function_name, geode::Vector< Mesh::dim >{} );
            auto gradient_function =
                mesh_.vertex_attribute_manager()
                    .template find_attribute< geode::VariableAttribute,
                        geode::Vector< Mesh::dim > >( gradient_function_id );
            std::vector< geode::index_t > no_gradient_value_vertices;
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( !compute_gradient( *gradient_function, vertex_id ) )
                {
                    no_gradient_value_vertices.push_back( vertex_id );
                }
            }
            return std::make_tuple(
                gradient_function_id, std::move( no_gradient_value_vertices ) );
        }

    private:
        void initialize_attribute_and_id(
            const geode::uuid& scalar_function_id )
        {
            geode::OpenGeodeMeshException::check_exception(
                mesh_.vertex_attribute_manager().attribute_exists(
                    scalar_function_id ),
                nullptr, geode::OpenGeodeException::TYPE::data,
                "[compute_scalar_function_gradient] No attribute exists with "
                "given name." );
            geode::OpenGeodeMeshException::check_exception(
                mesh_.vertex_attribute_manager().attribute_type(
                    scalar_function_id )
                    == typeid( double ).name(),
                nullptr, geode::OpenGeodeException::TYPE::data,
                "[compute_scalar_function_gradient] The attribute linked to "
                "given name is not scalar." );
            scalar_function_ =
                mesh_.vertex_attribute_manager()
                    .template read_attribute< double >( scalar_function_id );
        }

        bool compute_gradient(
            geode::VariableAttribute< geode::Vector< Mesh::dim > >&
                gradient_function,
            geode::index_t vertex_id ) const
        {
            const auto& position = mesh_.point( vertex_id );
            const auto function_value = scalar_function_->value( vertex_id );
            if( !vertex_has_value_[vertex_id] )
            {
                gradient_function.set_value( vertex_id, no_value_gradient() );
                return false;
            }
            geode::Vector< Mesh::dim > computed_gradient;
            const auto vertices_around =
                mesh_.vertices_around_vertex( vertex_id );
            bool value_set{ false };
            for( const auto d : geode::LRange{ Mesh::dim } )
            {
                double contribution_sum{ 0 };
                double inverse_dist_sum{ 0 };
                for( const auto vertex_around : vertices_around )
                {
                    if( !vertex_has_value_[vertex_around] )
                    {
                        gradient_function.set_value(
                            vertex_id, no_value_gradient() );
                        return false;
                    }
                    const geode::Vector< Mesh::dim > position_diff{
                        mesh_.point( vertex_around ), position
                    };
                    const auto dist2 = position_diff.length2();
                    if( std::fabs( dist2 ) < geode::GLOBAL_EPSILON
                        || std::fabs(
                               position_diff.value( d ) / std::sqrt( dist2 ) )
                               < 0.1 )
                    {
                        continue;
                    }
                    const double diff_sign{ position_diff.value( d ) < 0 ? -1.
                                                                         : 1. };
                    contribution_sum +=
                        ( function_value
                            - scalar_function_->value( vertex_around ) )
                        * diff_sign / dist2;
                    inverse_dist_sum +=
                        diff_sign * position_diff.value( d ) / dist2;
                }
                if( std::fabs( inverse_dist_sum ) <= geode::GLOBAL_EPSILON )
                {
                    computed_gradient.set_value( d, 0 );
                    continue;
                }
                computed_gradient.set_value(
                    d, contribution_sum / inverse_dist_sum );
                value_set = true;
            }
            if( !value_set )
            {
                return false;
            }
            gradient_function.set_value( vertex_id, computed_gradient );
            return true;
        }

        geode::Vector< Mesh::dim > no_value_gradient() const
        {
            geode::Vector< Mesh::dim > result;
            for( const auto dim : geode::LRange{ Mesh::dim } )
            {
                result.set_value( dim, std::nan( "" ) );
            };
            return result;
        }

    private:
        const Mesh& mesh_;
        std::shared_ptr< geode::ReadOnlyAttribute< double > > scalar_function_;
        std::vector< bool > vertex_has_value_;
    };
} // namespace

namespace geode
{
    template < index_t dimension >
    geode::uuid compute_surface_scalar_function_gradient(
        const SurfaceMesh< dimension >& mesh, const uuid& scalar_function_id )
    {
        ::ScalarGradientComputer computer{ mesh, scalar_function_id };
        return std::get< 0 >( computer.compute_scalar_function_gradient() );
    }

    geode::uuid compute_solid_scalar_function_gradient(
        const SolidMesh3D& mesh, const uuid& scalar_function_id )
    {
        ::ScalarGradientComputer computer{ mesh, scalar_function_id };
        return std::get< 0 >( computer.compute_scalar_function_gradient() );
    }

    template geode::uuid opengeode_mesh_api
        compute_surface_scalar_function_gradient(
            const SurfaceMesh2D&, const uuid& );
    template geode::uuid opengeode_mesh_api
        compute_surface_scalar_function_gradient(
            const SurfaceMesh3D&, const uuid& );

    namespace internal
    {
        template < index_t dimension >
        std::tuple< geode::uuid, std::vector< index_t > >
            compute_surface_scalar_function_gradient(
                const SurfaceMesh< dimension >& mesh,
                const uuid& scalar_function_id,
                absl::Span< const index_t > no_value_vertices )
        {
            ::ScalarGradientComputer computer{ mesh, scalar_function_id,
                no_value_vertices };
            return computer.compute_scalar_function_gradient();
        }

        std::tuple< geode::uuid, std::vector< index_t > >
            compute_solid_scalar_function_gradient( const SolidMesh3D& mesh,
                const uuid& scalar_function_id,
                absl::Span< const index_t > no_value_vertices )
        {
            ::ScalarGradientComputer computer{ mesh, scalar_function_id,
                no_value_vertices };
            return computer.compute_scalar_function_gradient();
        }

        template std::tuple< geode::uuid, std::vector< index_t > >
            opengeode_mesh_api compute_surface_scalar_function_gradient(
                const SurfaceMesh2D&,
                const uuid&,
                absl::Span< const index_t > );
        template std::tuple< geode::uuid, std::vector< index_t > >
            opengeode_mesh_api compute_surface_scalar_function_gradient(
                const SurfaceMesh3D&,
                const uuid&,
                absl::Span< const index_t > );
    } // namespace internal
} // namespace geode