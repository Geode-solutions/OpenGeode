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

#include <geode/basic/bitsery_archive.h>

#include <geode/basic/attribute_manager.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/mixin/core/vertex_identifier.h>

namespace bitsery
{
    namespace ext
    {
        template <>
        struct PolymorphicBaseClass< geode::Component2D >
            : PolymorphicDerivedClasses< geode::Corner2D,
                  geode::Line2D,
                  geode::Surface2D,
                  geode::ModelBoundary2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::Component3D >
            : PolymorphicDerivedClasses< geode::Corner3D,
                  geode::Line3D,
                  geode::Surface3D,
                  geode::Block3D,
                  geode::ModelBoundary3D >
        {
        };
    } // namespace ext
} // namespace bitsery

namespace geode
{
    namespace detail
    {
        template < typename Serializer >
        void register_model_pcontext( PContext& context )
        {
            AttributeManager::register_attribute_type<
                std::vector< MeshComponentVertex >, Serializer >( context );
            AttributeManager::register_attribute_type< ComponentID,
                Serializer >( context );
            context.registerBasesList< Serializer >(
                bitsery::ext::PolymorphicClassesList< Component2D >{} );
            context.registerBasesList< Serializer >(
                bitsery::ext::PolymorphicClassesList< Component3D >{} );
        }
    } // namespace detail
} // namespace geode
