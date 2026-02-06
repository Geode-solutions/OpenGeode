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

#include <geode/model/mixin/core/component_registry.hpp>

#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    class ComponentRegistry::Impl
    {
    public:
        void add_mesh_component( const ComponentType& type, const uuid& id )
        {
            mesh_components_[type].push_back( id );
        }

        void add_collection_component(
            const ComponentType& type, const uuid& id )
        {
            collection_components_[type].push_back( id );
        }

        void remove_mesh_component( const ComponentType& type, const uuid& id )
        {
            auto& registry = mesh_components_[type];
            registry.erase( std::remove( registry.begin(), registry.end(), id ),
                registry.end() );
        }

        void remove_collection_component(
            const ComponentType& type, const uuid& id )
        {
            auto& registry = collection_components_[type];
            registry.erase( std::remove( registry.begin(), registry.end(), id ),
                registry.end() );
        }

        const Registry& mesh_components() const
        {
            return mesh_components_;
        }

        const Registry& collection_components() const
        {
            return collection_components_;
        }

    private:
        Registry mesh_components_;
        Registry collection_components_;
    };

    ComponentRegistry::ComponentRegistry() = default;

    ComponentRegistry::~ComponentRegistry() = default;

    ComponentRegistry::ComponentRegistry(
        ComponentRegistry&& other ) noexcept = default;

    ComponentRegistry& ComponentRegistry::operator=(
        ComponentRegistry&& other ) noexcept = default;

    auto ComponentRegistry::mesh_components() const -> const Registry&
    {
        return impl_->mesh_components();
    }

    auto ComponentRegistry::collection_components() const -> const Registry&
    {
        return impl_->collection_components();
    }

    void ComponentRegistry::add_mesh_component(
        const ComponentType& type, const uuid& id, RegistryBuilder )
    {
        impl_->add_mesh_component( type, id );
    }

    void ComponentRegistry::add_collection_component(
        const ComponentType& type, const uuid& id, RegistryBuilder )
    {
        impl_->add_collection_component( type, id );
    }

    void ComponentRegistry::remove_mesh_component(
        const ComponentType& type, const uuid& id, RegistryBuilder )
    {
        impl_->remove_mesh_component( type, id );
    }

    void ComponentRegistry::remove_collection_component(
        const ComponentType& type, const uuid& id, RegistryBuilder )
    {
        impl_->remove_collection_component( type, id );
    }
} // namespace geode
