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
    template < typename Range >
    void get_components_crs(
        absl::flat_hash_set< std::pair< geode::CRSType, std::string > >& crss,
        Range range )
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
        absl::flat_hash_set< std::pair< geode::CRSType, std::string > >& crss )
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
} // namespace

namespace geode
{
    absl::FixedArray< std::pair< CRSType, std::string > >
        brep_coordinate_reference_systems( const BRep& brep )
    {
        absl::flat_hash_set< std::pair< CRSType, std::string > > crss;
        get_components_crs( crss, brep.corners() );
        get_components_crs( crss, brep.lines() );
        get_components_crs( crss, brep.surfaces() );
        get_components_crs( crss, brep.blocks() );
        return get_result( crss );
    }

    absl::FixedArray< std::pair< CRSType, std::string > >
        section_coordinate_reference_systems( const Section& section )
    {
        absl::flat_hash_set< std::pair< CRSType, std::string > > crss;
        get_components_crs( crss, section.corners() );
        get_components_crs( crss, section.lines() );
        get_components_crs( crss, section.surfaces() );
        return get_result( crss );
    }
} // namespace geode