/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/georepresentation/common.h>
#include <geode/georepresentation/core/component.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Boundaries );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundariesBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Collection component describing a Boundary.
     */
    template < index_t dimension >
    class Boundary : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Boundary );
        friend class Boundaries< dimension >;

    public:
        ~Boundary() = default;

        static ComponentType component_type_static()
        {
            return ComponentType{ "Boundary" };
        }

        ComponentType component_type() const
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

    private:
        friend class bitsery::Access;
        Boundary() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_2D_AND_3D( Boundary );
} // namespace geode
