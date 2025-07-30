/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#pragma once

#include <string_view>

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/mesh_id.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an PolyhedralSolid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] impl Data structure implementation.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    [[nodiscard]] std::unique_ptr< PolyhedralSolid< dimension > >
        load_polyhedral_solid(
            const MeshImpl& impl, std::string_view filename );

    /*!
     * API function for loading an PolyhedralSolid.
     * The adequate loader is called depending on the filename extension.
     * Default data structure implémentation is used.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    [[nodiscard]] std::unique_ptr< PolyhedralSolid< dimension > >
        load_polyhedral_solid( std::string_view filename );

    template < index_t dimension >
    class PolyhedralSolidInput
        : public Input< std::unique_ptr< PolyhedralSolid< dimension > >,
              MeshImpl >
    {
    protected:
        explicit PolyhedralSolidInput( std::string_view filename )
            : Input< std::unique_ptr< PolyhedralSolid< dimension > >,
                  MeshImpl >{ filename }
        {
        }
    };
    ALIAS_3D( PolyhedralSolidInput );

    template < index_t dimension >
    [[nodiscard]] typename PolyhedralSolidInput< dimension >::AdditionalFiles
        polyhedral_solid_additional_files( std::string_view filename );

    template < index_t dimension >
    [[nodiscard]] bool is_polyhedral_solid_loadable(
        std::string_view filename );

    template < index_t dimension >
    [[nodiscard]] index_t polyhedral_solid_object_priority(
        std::string_view filename );

    template < index_t dimension >
    using PolyhedralSolidInputFactory = Factory< std::string,
        PolyhedralSolidInput< dimension >,
        std::string_view >;
    ALIAS_3D( PolyhedralSolidInputFactory );
} // namespace geode
