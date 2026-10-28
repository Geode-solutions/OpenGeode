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

#include <memory>
#include <string_view>

#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    ALIAS_3D( PointSet );
    class BRep;
} // namespace geode

namespace geode
{
    /*!
     * Time series computed outside of a BRep (e.g. by a simulator), stored
     * apart from the model and written on it on demand.
     * Values are stored as vertex attributes of two point sets: the
     * simulation unique vertices (one point per colocated position) and the
     * simulation cell barycenters. They are matched geometrically with the
     * Block vertices and polyhedra of a BRep when applied.
     * Time series are expected to be created as time step attributes (see
     * AttributeManager::create_time_step_attribute), but any transferable
     * attribute is stored and applied the same way.
     * The native file can be loaded onto a BRep with load_brep_time_series.
     */
    class opengeode_model_api ExternalTimeSeries
    {
        OPENGEODE_DISABLE_COPY( ExternalTimeSeries );

    public:
        ExternalTimeSeries();
        ExternalTimeSeries( ExternalTimeSeries&& other ) noexcept;
        ExternalTimeSeries& operator=( ExternalTimeSeries&& other ) noexcept;
        ~ExternalTimeSeries();

        [[nodiscard]] static std::string_view native_extension_static()
        {
            return "og_ets";
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Set the simulation unique vertices, carrying the vertex values as
         * vertex attributes
         * @exception OpenGeodeException if the point set is null
         */
        void set_unique_vertices( std::unique_ptr< PointSet3D > vertices );

        /*!
         * Set the simulation cell barycenters, carrying the cell values as
         * vertex attributes
         * @exception OpenGeodeException if the point set is null
         */
        void set_cell_barycenters( std::unique_ptr< PointSet3D > barycenters );

        /*!
         * Write the transferable attributes on the Block meshes of the model:
         * each unique vertex on the closest model unique vertex (on all its
         * Block vertices), each cell barycenter on the Block polyhedron with
         * the closest barycenter. Block elements matched with no point keep
         * no value, Blocks matched with no point are left untouched.
         * Attribute ids and properties (time steps included) are kept. A
         * Block time step with the same name and time as an applied one is
         * replaced.
         * The matching tolerance is GLOBAL_EPSILON relative to the largest
         * absolute coordinate of the points (when larger than 1), to accept
         * points rounded by the simulator (e.g. stored in single precision).
         * @exception OpenGeodeException if a point is farther than the
         * tolerance from its closest Block element, if several points are
         * matched with the same Block element, or if a point is within the
         * tolerance of several Block elements
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
