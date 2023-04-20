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

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>

#include <geode/mesh/core/coordinate_reference_system_manager.h>

namespace geode
{
    template < index_t dimension >
    void CoordinateReferenceSystemManagerBuilder< dimension >::
        register_coordinate_reference_system( absl::string_view name,
            std::shared_ptr< CoordinateReferenceSystem< dimension > >&& crs )
    {
        crs_manager_.register_coordinate_reference_system(
            name, std::move( crs ), {} );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManagerBuilder< dimension >::
        delete_coordinate_reference_system( absl::string_view name )
    {
        crs_manager_.delete_coordinate_reference_system( name, {} );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManagerBuilder< dimension >::
        set_active_coordinate_reference_system( absl::string_view name )
    {
        crs_manager_.set_active_coordinate_reference_system( name, {} );
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManagerBuilder<
            dimension >::active_coordinate_reference_system()
    {
        return crs_manager_.modifiable_active_coordinate_reference_system( {} );
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManagerBuilder<
            dimension >::coordinate_reference_system( absl::string_view name )
    {
        return crs_manager_.modifiable_coordinate_reference_system( name, {} );
    }

    template class opengeode_mesh_api
        CoordinateReferenceSystemManagerBuilder< 1 >;
    template class opengeode_mesh_api
        CoordinateReferenceSystemManagerBuilder< 2 >;
    template class opengeode_mesh_api
        CoordinateReferenceSystemManagerBuilder< 3 >;
} // namespace geode