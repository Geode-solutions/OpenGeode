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

#pragma once

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HybridSolid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an HybridSolid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] impl Data structure implementation.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > > load_hybrid_solid(
        const MeshImpl& impl, absl::string_view filename );

    /*!
     * API function for loading an HybridSolid.
     * The adequate loader is called depending on the filename extension.
     * Default data structure implémentation is used.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > > load_hybrid_solid(
        absl::string_view filename );

    template < index_t dimension >
    class HybridSolidInput
        : public Input< std::unique_ptr< HybridSolid< dimension > >, MeshImpl >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        using Base =
            Input< std::unique_ptr< HybridSolid< dimension > >, MeshImpl >;
        using Base::InputData;
        using Base::MissingFiles;

    protected:
        explicit HybridSolidInput( absl::string_view filename )
            : Base{ filename }
        {
        }
    };

    template < index_t dimension >
    typename HybridSolidInput< dimension >::MissingFiles
        check_hybrid_solid_missing_files( absl::string_view filename );

    template < index_t dimension >
    bool is_hybrid_solid_loadable( absl::string_view filename );

    template < index_t dimension >
    using HybridSolidInputFactory = Factory< std::string,
        HybridSolidInput< dimension >,
        absl::string_view >;
    ALIAS_3D( HybridSolidInputFactory );
} // namespace geode
