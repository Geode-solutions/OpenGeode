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

#pragma once

#include <string_view>

#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    class AttributeManager;
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Time series computed outside of a BRep (e.g. by a simulator), stored
     * apart from the model and written on it on demand.
     * Vertex values are stored per unique vertex of the model and polyhedron
     * values per polyhedron of each Block mesh.
     * Time series are expected to be created as time step attributes (see
     * AttributeManager::create_time_step_attribute), but any transferable
     * attribute is stored and applied the same way.
     */
    class opengeode_model_api ExternalTimeSeriesHelpers
    {
        OPENGEODE_DISABLE_COPY( ExternalTimeSeriesHelpers );

    public:
        ExternalTimeSeriesHelpers();
        /*!
         * Create empty attribute managers sized to the model unique vertices
         * and to the polyhedra of its Blocks.
         */
        explicit ExternalTimeSeriesHelpers( const BRep& model );
        ExternalTimeSeriesHelpers( ExternalTimeSeriesHelpers&& other ) noexcept;
        ExternalTimeSeriesHelpers& operator=(
            ExternalTimeSeriesHelpers&& other ) noexcept;
        ~ExternalTimeSeriesHelpers();

        [[nodiscard]] static std::string_view native_extension_static()
        {
            return "og_ets";
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Attributes on the unique vertices of the model
         */
        [[nodiscard]] AttributeManager& unique_vertex_attribute_manager();

        [[nodiscard]] const AttributeManager&
            unique_vertex_attribute_manager() const;

        /*!
         * Attributes on the polyhedra of a Block mesh
         * @exception OpenGeodeException if the Block is unknown
         */
        [[nodiscard]] AttributeManager& block_polyhedron_attribute_manager(
            const uuid& block_id );

        [[nodiscard]] const AttributeManager&
            block_polyhedron_attribute_manager( const uuid& block_id ) const;

        /*!
         * Write the transferable attributes on the Block meshes of the model:
         * unique vertex attributes on each Block vertex, polyhedron attributes
         * on the matching Block polyhedra. Non-transferable attributes are
         * skipped. Attribute ids and properties (time steps included) are
         * kept.
         * @exception OpenGeodeException if the model does not have the same
         * unique vertices or Block polyhedra as the one used for creation
         */
        void apply_to( BRep& model ) const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
