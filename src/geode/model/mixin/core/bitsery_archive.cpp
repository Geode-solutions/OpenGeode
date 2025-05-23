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

#include <geode/model/mixin/core/bitsery_archive.hpp>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/bitsery_attribute.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>

namespace bitsery
{
    namespace ext
    {
        template <>
        struct PolymorphicBaseClass< geode::Component2D >
            : PolymorphicDerivedClasses< geode::Corner2D,
                  geode::Line2D,
                  geode::Surface2D,
                  geode::ModelBoundary2D,
                  geode::CornerCollection2D,
                  geode::LineCollection2D,
                  geode::SurfaceCollection2D >
        {
        };

        template <>
        struct PolymorphicBaseClass< geode::Component3D >
            : PolymorphicDerivedClasses< geode::Corner3D,
                  geode::Line3D,
                  geode::Surface3D,
                  geode::Block3D,
                  geode::ModelBoundary3D,
                  geode::CornerCollection3D,
                  geode::LineCollection3D,
                  geode::SurfaceCollection3D,
                  geode::BlockCollection3D >
        {
        };
    } // namespace ext
} // namespace bitsery

namespace
{
    template < typename Serializer >
    void register_model_pcontext( geode::PContext& context )
    {
        geode::register_attribute_type<
            std::vector< geode::ComponentMeshVertex >, Serializer >(
            context, "vector_MCV" );
        geode::register_attribute_type< geode::ComponentID, Serializer >(
            context, "ComponentID" );
        context.registerBasesList< Serializer >(
            bitsery::ext::PolymorphicClassesList< geode::Component2D >{} );
        context.registerBasesList< Serializer >(
            bitsery::ext::PolymorphicClassesList< geode::Component3D >{} );
    } // namespace )
} // namespace

namespace geode
{
    void register_model_deserialize_pcontext( PContext& context )
    {
        register_model_pcontext< Deserializer >( context );
    }

    void register_model_serialize_pcontext( PContext& context )
    {
        register_model_pcontext< Serializer >( context );
    }
} // namespace geode
