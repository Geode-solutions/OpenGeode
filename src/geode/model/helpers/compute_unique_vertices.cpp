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

#include <geode/model/helpers/compute_unique_vertices.hpp>

#include <geode/geometry/nn_search.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    std::vector< geode::Point< Model::dim > > get_all_points_base(
        const Model& model )
    {
        std::vector< geode::Point< Model::dim > > points;
        for( const auto& corner : model.corners() )
        {
            for( const auto v : geode::Range{ corner.mesh().nb_vertices() } )
            {
                points.emplace_back( corner.mesh().point( v ) );
            }
        }
        for( const auto& line : model.lines() )
        {
            for( const auto v : geode::Range{ line.mesh().nb_vertices() } )
            {
                points.emplace_back( line.mesh().point( v ) );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                points.emplace_back( surface.mesh().point( v ) );
            }
        }
        return points;
    }

    std::vector< geode::Point2D > get_all_points( const geode::Section& model )
    {
        return get_all_points_base< geode::Section >( model );
    }

    std::vector< geode::Point3D > get_all_points( const geode::BRep& model )
    {
        auto points = get_all_points_base< geode::BRep >( model );
        for( const auto& block : model.blocks() )
        {
            for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
            {
                points.emplace_back( block.mesh().point( v ) );
            }
        }
        return points;
    }

    template < typename Model >
    void set_unique_vertices_base( const Model& model,
        typename Model::Builder& builder,
        const geode::NNSearch< Model::dim >& unique_nns )
    {
        for( const auto& corner : model.corners() )
        {
            for( const auto v : geode::Range{ corner.mesh().nb_vertices() } )
            {
                const auto& point = corner.mesh().point( v );
                builder.set_unique_vertex( { corner.component_id(), v },
                    unique_nns.closest_neighbor( point ) );
            }
        }
        for( const auto& line : model.lines() )
        {
            for( const auto v : geode::Range{ line.mesh().nb_vertices() } )
            {
                const auto& point = line.mesh().point( v );
                builder.set_unique_vertex( { line.component_id(), v },
                    unique_nns.closest_neighbor( point ) );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                const auto& point = surface.mesh().point( v );
                builder.set_unique_vertex( { surface.component_id(), v },
                    unique_nns.closest_neighbor( point ) );
            }
        }
    }

    void set_unique_vertices( const geode::Section& model,
        geode::SectionBuilder& builder,
        const geode::NNSearch2D& unique_nns )
    {
        set_unique_vertices_base( model, builder, unique_nns );
    }

    void set_unique_vertices( const geode::BRep& model,
        geode::BRepBuilder& builder,
        const geode::NNSearch3D& unique_nns )
    {
        set_unique_vertices_base( model, builder, unique_nns );
        for( const auto& block : model.blocks() )
        {
            for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
            {
                const auto& point = block.mesh().point( v );
                builder.set_unique_vertex( { block.component_id(), v },
                    unique_nns.closest_neighbor( point ) );
            }
        }
    }

} // namespace

namespace geode
{
    template < typename Model >
    void compute_model_unique_vertices(
        const Model& model, typename Model::Builder& builder )
    {
        if( model.nb_unique_vertices() > 0 )
        {
            return;
        }
        auto all_vertices = get_all_points( model );
        NNSearch< Model::dim > nns{ all_vertices };
        const auto colocated_info =
            nns.colocated_index_mapping( GLOBAL_EPSILON );
        builder.create_unique_vertices( colocated_info.nb_unique_points() );
        NNSearch< Model::dim > unique_nns{ colocated_info.unique_points };
        set_unique_vertices( model, builder, unique_nns );
    }

    template void opengeode_model_api compute_model_unique_vertices(
        const BRep&, BRepBuilder& );
    template void opengeode_model_api compute_model_unique_vertices(
        const Section&, SectionBuilder& );
} // namespace geode
