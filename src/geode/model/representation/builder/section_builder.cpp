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

#include <geode/model/representation/builder/section_builder.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    SectionBuilder::SectionBuilder( Section& section )
        : GeoRepresentationBuilder( section ),
          AddComponentsBuilders< 2, Corners, Lines, Surfaces >( section ),
          section_( section )
    {
    }

    const uuid& SectionBuilder::add_corner()
    {
        const auto& id = create_corner();
        relationships().add_component( id );
        unique_vertices().register_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner( const MeshType& type )
    {
        const auto& id = create_corner( type );
        relationships().add_component( id );
        unique_vertices().register_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line()
    {
        const auto& id = create_line();
        relationships().add_component( id );
        unique_vertices().register_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line( const MeshType& type )
    {
        const auto& id = create_line( type );
        relationships().add_component( id );
        unique_vertices().register_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface()
    {
        const auto& id = create_surface();
        relationships().add_component( id );
        unique_vertices().register_component( section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface( const MeshType& type )
    {
        const auto& id = create_surface( type );
        relationships().add_component( id );
        unique_vertices().register_component( section_.surface( id ) );
        return id;
    }

    void SectionBuilder::remove_corner( const Corner2D& corner )
    {
        relationships().remove_component( corner.id() );
        unique_vertices().remove_component( corner );
        delete_corner( corner );
    }

    void SectionBuilder::remove_line( const Line2D& line )
    {
        relationships().remove_component( line.id() );
        unique_vertices().remove_component( line );
        delete_line( line );
    }

    void SectionBuilder::remove_surface( const Surface2D& surface )
    {
        relationships().remove_component( surface.id() );
        unique_vertices().remove_component( surface );
        delete_surface( surface );
    }

    void SectionBuilder::add_boundary_relation(
        const Corner2D& corner, const Line2D& line )
    {
        relationships().add_boundary_relation( corner.id(), line.id() );
    }

    void SectionBuilder::add_boundary_relation(
        const Line2D& line, const Surface2D& surface )
    {
        relationships().add_boundary_relation( line.id(), surface.id() );
    }

} // namespace geode
