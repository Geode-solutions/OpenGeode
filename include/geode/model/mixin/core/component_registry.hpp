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

#include <vector>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    struct uuid;
    class ComponentRegistryBuilder;
} // namespace geode

namespace geode
{
    class opengeode_model_api ComponentRegistry
    {
        PASSKEY( ComponentRegistryBuilder, RegistryBuilder );

    public:
        using Registry =
            absl::flat_hash_map< ComponentType, std::vector< uuid > >;

        ComponentRegistry();
        ~ComponentRegistry();

        const Registry& mesh_components() const;

        const Registry& collection_components() const;

    public:
        void add_mesh_component(
            const ComponentType& type, const uuid& id, RegistryBuilder key );

        void add_collection_component(
            const ComponentType& type, const uuid& id, RegistryBuilder key );

        void remove_mesh_component(
            const ComponentType& type, const uuid& id, RegistryBuilder key );

        void remove_collection_component(
            const ComponentType& type, const uuid& id, RegistryBuilder key );

    protected:
        ComponentRegistry( ComponentRegistry&& other ) noexcept;
        ComponentRegistry& operator=( ComponentRegistry&& other ) noexcept;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
