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

#include <geode/model/representation/io/brep_time_series_input.hpp>

#include <string_view>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/timer.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    void add_time_series( const geode::AttributeManager& manager,
        absl::flat_hash_set< std::string >& names,
        absl::flat_hash_set< double >& times )
    {
        for( auto& name : manager.time_series_names() )
        {
            for( const auto& step : manager.time_steps( name ) )
            {
                times.insert( step.time );
            }
            names.insert( std::move( name ) );
        }
    }

    void log_time_series( const geode::BRep& brep )
    {
        absl::flat_hash_set< std::string > names;
        absl::flat_hash_set< double > times;
        for( const auto& block : brep.blocks() )
        {
            const auto& mesh = block.mesh();
            add_time_series( mesh.vertex_attribute_manager(), names, times );
            add_time_series(
                mesh.polyhedron_attribute_manager(), names, times );
        }
        geode::Logger::info( "BRep has: ", names.size(), " time series, ",
            times.size(), " time steps" );
    }
} // namespace

namespace geode
{
    void load_brep_time_series( BRep& brep, std::string_view filename )
    {
        constexpr auto TYPE = "BRep time series";
        try
        {
            const Timer timer;
            auto input =
                detail::geode_object_input_reader< BRepTimeSeriesInputFactory >(
                    filename );
            input->read( brep );
            Logger::info(
                TYPE, " loaded from ", filename, " in ", timer.duration() );
            log_time_series( brep );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< BRepTimeSeriesInputFactory >( TYPE );
            throw OpenGeodeModelException{ nullptr,
                OpenGeodeException::TYPE::data,
                "Cannot load BRep time series from file: ", filename };
        }
    }

    AdditionalFiles brep_time_series_additional_files(
        std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< BRepTimeSeriesInputFactory >(
                filename );
        return input->additional_files();
    }

    Percentage is_brep_time_series_loadable( std::string_view filename )
    {
        try
        {
            const auto input =
                detail::geode_object_input_reader< BRepTimeSeriesInputFactory >(
                    filename );
            return input->is_loadable();
        }
        catch( ... )
        {
            return Percentage{ 0 };
        }
    }
} // namespace geode
