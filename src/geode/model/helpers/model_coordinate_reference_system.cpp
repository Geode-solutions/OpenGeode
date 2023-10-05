/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/helpers/model_coordinate_reference_system.h>

#include <absl/container/flat_hash_set.h>

#include <geode/mesh/core/coordinate_reference_system_manager.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    using CRSMapValue = std::pair< geode::CRSType, std::string >;
    using CRSMap = absl::flat_hash_set< CRSMapValue >;

    template < typename Range >
    absl::optional< CRSMap > get_first_components_crs( Range range )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            if( mesh.nb_vertices() == 0 )
            {
                continue;
            }
            const auto& crs_manager =
                mesh.main_coordinate_reference_system_manager();
            CRSMap crss;
            for( const auto& crs_name :
                crs_manager.coordinate_reference_system_names() )
            {
                crss.emplace(
                    crs_manager.find_coordinate_reference_system( crs_name )
                        .type_name(),
                    geode::to_string( crs_name ) );
            }
            return crss;
        }
        return absl::nullopt;
    }

    template < typename Range >
    void get_components_crs_intersection(
        CRSMap& crs_intersection, Range range )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            if( mesh.nb_vertices() == 0 )
            {
                continue;
            }
            const auto& crs_manager =
                mesh.main_coordinate_reference_system_manager();
            CRSMap crss;
            for( const auto& crs_name :
                crs_manager.coordinate_reference_system_names() )
            {
                auto pair = std::make_pair(
                    crs_manager.find_coordinate_reference_system( crs_name )
                        .type_name(),
                    geode::to_string( crs_name ) );
                if( crs_intersection.contains( pair ) )
                {
                    crss.emplace( std::move( pair ) );
                }
            }
            if( crss.size() == crs_intersection.size() )
            {
                continue;
            }
            std::vector< CRSMapValue > to_remove;
            for( const auto& pair : crs_intersection )
            {
                if( !crss.contains( pair ) )
                {
                    to_remove.push_back( pair );
                }
            }
            for( const auto& pair : to_remove )
            {
                crs_intersection.erase( pair );
            }
        }
    }

    template < typename Range >
    void get_components_crs( CRSMap& crss, Range range )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            if( mesh.nb_vertices() == 0 )
            {
                continue;
            }
            const auto& crs_manager =
                mesh.main_coordinate_reference_system_manager();
            crss.emplace(
                crs_manager.active_coordinate_reference_system().type_name(),
                geode::to_string(
                    crs_manager.active_coordinate_reference_system_name() ) );
        }
    }

    absl::FixedArray< std::pair< geode::CRSType, std::string > > get_result(
        CRSMap& crss )
    {
        absl::FixedArray< std::pair< geode::CRSType, std::string > > result(
            crss.size() );
        geode::index_t count{ 0 };
        for( auto& crs : crss )
        {
            result[count++] = std::move( crs );
        }
        return result;
    }

    template < typename Model >
    absl::optional< CRSMap > get_first_model_crs( const Model& model )
    {
        if( auto crss = get_first_components_crs( model.corners() ) )
        {
            return crss;
        }
        if( auto crss = get_first_components_crs( model.lines() ) )
        {
            return crss;
        }
        if( auto crss = get_first_components_crs( model.surfaces() ) )
        {
            return crss;
        }
        return absl::nullopt;
    }

    template < typename Model >
    CRSMap model_crss( const Model& model )
    {
        CRSMap crss;
        get_components_crs( crss, model.corners() );
        get_components_crs( crss, model.lines() );
        get_components_crs( crss, model.surfaces() );
        return crss;
    }

    template < typename Model >
    void model_crss_intersection( const Model& model, CRSMap& intersection )
    {
        get_components_crs_intersection( intersection, model.corners() );
        get_components_crs_intersection( intersection, model.lines() );
        get_components_crs_intersection( intersection, model.surfaces() );
    }
} // namespace

namespace geode
{
    absl::FixedArray< std::pair< CRSType, std::string > >
        brep_coordinate_reference_systems( const BRep& brep )
    {
        if( auto crss = get_first_model_crs( brep ) )
        {
            model_crss_intersection( brep, crss.value() );
            get_components_crs_intersection( crss.value(), brep.blocks() );
            return get_result( crss.value() );
        }
        if( auto crss = get_first_components_crs( brep.blocks() ) )
        {
            model_crss_intersection( brep, crss.value() );
            get_components_crs_intersection( crss.value(), brep.blocks() );
            return get_result( crss.value() );
        }
        return {};
    }

    absl::FixedArray< std::pair< CRSType, std::string > >
        section_coordinate_reference_systems( const Section& section )
    {
        if( auto crss = get_first_model_crs( section ) )
        {
            model_crss_intersection( section, crss.value() );
            return get_result( crss.value() );
        }
        return {};
    }

    absl::FixedArray< std::pair< CRSType, std::string > >
        brep_active_coordinate_reference_systems( const BRep& brep )
    {
        auto crss = model_crss( brep );
        get_components_crs( crss, brep.blocks() );
        return get_result( crss );
    }

    absl::FixedArray< std::pair< CRSType, std::string > >
        section_active_coordinate_reference_systems( const Section& section )
    {
        auto crss = model_crss( section );
        return get_result( crss );
    }
} // namespace geode