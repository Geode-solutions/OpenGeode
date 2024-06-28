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

#include <geode/mesh/helpers/detail/create_mesh.h>

#include <absl/types/span.h>

#include <memory>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        std::unique_ptr< Mesh > create_mesh(
            absl::Span< const std::reference_wrapper< const Mesh > > meshes )
        {
            if( meshes.empty() )
            {
                return Mesh::create();
            }
            const auto type = meshes.front().get().type_name();
            for( const auto& mesh : meshes )
            {
                if( mesh.get().type_name() != type )
                {
                    return Mesh::create();
                }
            }
            return Mesh::create( geode::MeshFactory::default_impl( type ) );
        }

        template std::unique_ptr< PointSet2D > opengeode_mesh_api create_mesh(
            absl::Span< const std::reference_wrapper< const PointSet2D > > );
        template std::unique_ptr< EdgedCurve2D > opengeode_mesh_api create_mesh(
            absl::Span< const std::reference_wrapper< const EdgedCurve2D > > );
        template std::unique_ptr< SurfaceMesh2D >
            opengeode_mesh_api create_mesh( absl::Span<
                const std::reference_wrapper< const SurfaceMesh2D > > );
        template std::unique_ptr< PointSet3D > opengeode_mesh_api create_mesh(
            absl::Span< const std::reference_wrapper< const PointSet3D > > );
        template std::unique_ptr< EdgedCurve3D > opengeode_mesh_api create_mesh(
            absl::Span< const std::reference_wrapper< const EdgedCurve3D > > );
        template std::unique_ptr< SurfaceMesh3D >
            opengeode_mesh_api create_mesh( absl::Span<
                const std::reference_wrapper< const SurfaceMesh3D > > );
        template std::unique_ptr< SolidMesh3D > opengeode_mesh_api create_mesh(
            absl::Span< const std::reference_wrapper< const SolidMesh3D > > );
    } // namespace detail
} // namespace geode