/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * This abstract class represents a component that can be used in assembly.
     */
    template < index_t dimension >
    class Component
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Component );
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        virtual ~Component();

        absl::string_view name() const;

        const uuid& id() const;

        virtual ComponentType component_type() const = 0;

    protected:
        friend class bitsery::Access;
        Component();

        void set_name( absl::string_view name );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Component );
} // namespace geode
