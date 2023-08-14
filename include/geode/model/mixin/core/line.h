/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component.h>

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
        Line( Line&& other ) noexcept;
        ~Line();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Line" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        const EdgedCurve< dimension >& mesh() const;

        const MeshImpl& mesh_type() const;

        EdgedCurve< dimension >& modifiable_mesh( LinesKey )
        {
            return modifiable_mesh();
        }

    public:
        Line( LinesKey ) : Line() {}

        Line( const MeshImpl& impl, LinesKey ) : Line( impl ) {}

        void set_mesh(
            std::unique_ptr< EdgedCurve< dimension > > mesh, LinesKey );

        void set_mesh(
            std::unique_ptr< EdgedCurve< dimension > > mesh, LinesBuilderKey );

        void set_line_name( absl::string_view name, LinesBuilderKey )
        {
            this->set_name( name );
        }

        EdgedCurve< dimension >& modifiable_mesh( LinesBuilderKey )
        {
            return modifiable_mesh();
        }

    private:
        Line();

        explicit Line( const MeshImpl& impl );

        EdgedCurve< dimension >& modifiable_mesh();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Line );
} // namespace geode
