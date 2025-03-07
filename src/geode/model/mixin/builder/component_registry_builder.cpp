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

#include <geode/model/mixin/builder/component_registry_builder.hpp>

#include <geode/model/mixin/core/component_registry.hpp>

namespace geode
{
    ComponentRegistryBuilder::ComponentRegistryBuilder(
        ComponentRegistry& component_registry )
        : registry_( component_registry )
    {
    }

    ComponentRegistryBuilder::~ComponentRegistryBuilder() = default;

    void ComponentRegistryBuilder::add_mesh_component(
        const ComponentType& type, const uuid& id )
    {
        registry_.add_mesh_component( type, id, {} );
    }

    void ComponentRegistryBuilder::add_collection_component(
        const ComponentType& type, const uuid& id )
    {
        registry_.add_collection_component( type, id, {} );
    }

    void ComponentRegistryBuilder::remove_mesh_component(
        const ComponentType& type, const uuid& id )
    {
        registry_.remove_mesh_component( type, id, {} );
    }

    void ComponentRegistryBuilder::remove_collection_component(
        const ComponentType& type, const uuid& id )
    {
        registry_.remove_collection_component( type, id, {} );
    }
} // namespace geode
