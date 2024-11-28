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

#include <memory>
#include <string_view>

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( Lines );
    FORWARD_DECLARATION_DIMENSION_CLASS( LinesBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Geometric component describing a Line.
     * This component is described by a mesh.
     */
    template < index_t dimension >
    class Line final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Line );
        PASSKEY( Lines< dimension >, LinesKey );
        PASSKEY( LinesBuilder< dimension >, LinesBuilderKey );
        friend class bitsery::Access;

    public:
        using Mesh = EdgedCurve< dimension >;

        Line( Line&& other ) noexcept;
        ~Line();

        [[nodiscard]] static ComponentType component_type_static()
        {
            return ComponentType{ "Line" };
        }

        [[nodiscard]] ComponentType component_type() const final
        {
            return component_type_static();
        }

        [[nodiscard]] ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        [[nodiscard]] const Mesh& mesh() const;

        [[nodiscard]] const MeshImpl& mesh_type() const;

    public:
        explicit Line( LinesKey key );

        Line( const MeshImpl& impl, LinesKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, LinesKey key );

        void set_mesh( std::unique_ptr< Mesh > mesh, LinesBuilderKey key );

        void set_line_name( std::string_view name, LinesBuilderKey key );

        [[nodiscard]] Mesh& modifiable_mesh( LinesKey key );

        [[nodiscard]] Mesh& modifiable_mesh( LinesBuilderKey key );

        [[nodiscard]] std::unique_ptr< Mesh > steal_mesh( LinesBuilderKey key );

    private:
        Line();

        explicit Line( const MeshImpl& impl );

        [[nodiscard]] Mesh& modifiable_mesh();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Line );
} // namespace geode
