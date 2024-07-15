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

#include <geode/mesh/helpers/gradient_computation.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/internal/grid_shape_function.h>

namespace
{
    template < typename Mesh >
    std::string compute_scalar_function_gradient(
        const Mesh& mesh, std::string_view scalar_function_name )
    {
        OPENGEODE_EXCEPTION( mesh.vertex_attribute_manager().attribute_exists(
                                 scalar_function_name ),
            "[compute_scalar_function_gradient] No attribute exists with given "
            "name." );
        OPENGEODE_EXCEPTION( mesh.vertex_attribute_manager().attribute_type(
                                 scalar_function_name )
                                 == typeid( double ).name(),
            "[compute_scalar_function_gradient] The attribute linked to given "
            "name is not scalar." );
        auto output_gradient_attribute_name =
            absl::StrCat( scalar_function_name, "_gradient" );
        geode::index_t counter{ 0 };
        while( mesh.vertex_attribute_manager().attribute_exists(
            absl::StrCat( output_gradient_attribute_name, counter ) ) )
        {
            counter++;
        }
        absl::StrAppend( &output_gradient_attribute_name, counter );

        const auto scalar_function =
            mesh.vertex_attribute_manager().template find_attribute< double >(
                scalar_function_name );
        const auto gradient_function =
            mesh.vertex_attribute_manager()
                .template find_or_create_attribute< geode::VariableAttribute,
                    geode::Vector< Mesh::dim > >(
                    output_gradient_attribute_name,
                    geode::Vector< Mesh::dim >{} );
        for( const auto vertex_id : geode::Range{ mesh.nb_vertices() } )
        {
            const auto& position = mesh.point( vertex_id );
            const auto function_value = scalar_function->value( vertex_id );
            geode::Vector< Mesh::dim > computed_gradient;
            const auto vertices_around =
                mesh.vertices_around_vertex( vertex_id );
            for( const auto d : geode::LRange{ Mesh::dim } )
            {
                double contribution_sum{ 0 };
                double inverse_dist_sum{ 0 };
                for( const auto vertex_around : vertices_around )
                {
                    const geode::Vector< Mesh::dim > position_diff{
                        mesh.point( vertex_around ), position
                    };
                    const auto value_diff =
                        function_value
                        - scalar_function->value( vertex_around );
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
                    contribution_sum += value_diff * diff_sign / dist2;
                    inverse_dist_sum +=
                        diff_sign * position_diff.value( d ) / dist2;
                }
                OPENGEODE_EXCEPTION(
                    std::fabs( inverse_dist_sum ) > geode::GLOBAL_EPSILON,
                    "[compute_scalar_function_gradient] Couldn't compute "
                    "gradient on vertex ",
                    vertex_id,
                    ": No vertex around it allows computation of the "
                    "derivative along axis ",
                    d );
                computed_gradient.set_value(
                    d, contribution_sum / inverse_dist_sum );
            }
            gradient_function->set_value( vertex_id, computed_gradient );
        }
        return output_gradient_attribute_name;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::string compute_surface_scalar_function_gradient(
        const SurfaceMesh< dimension >& mesh,
        std::string_view scalar_function_name )
    {
        return compute_scalar_function_gradient( mesh, scalar_function_name );
    }

    std::string compute_solid_scalar_function_gradient(
        const SolidMesh3D& mesh, std::string_view scalar_function_name )
    {
        return compute_scalar_function_gradient( mesh, scalar_function_name );
    }

    template std::string opengeode_mesh_api
        compute_surface_scalar_function_gradient(
            const SurfaceMesh2D&, std::string_view );
    template std::string opengeode_mesh_api
        compute_surface_scalar_function_gradient(
            const SurfaceMesh3D&, std::string_view );
} // namespace geode