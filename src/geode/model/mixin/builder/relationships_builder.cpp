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

#include <geode/model/mixin/builder/relationships_builder.h>

namespace geode
{
    RelationshipsBuilder::RelationshipsBuilder( Relationships& relationships )
        : relationships_( relationships )
    {
    }

    void RelationshipsBuilder::register_component( const ComponentID& id )
    {
        relationships_.register_component( id );
    }

    void RelationshipsBuilder::unregister_component( const uuid& id )
    {
        relationships_.unregister_component( id );
    }

    void RelationshipsBuilder::add_boundary_relation(
        const uuid& boundary, const uuid& incidence )
    {
        relationships_.add_boundary_relation( boundary, incidence );
    }

    void RelationshipsBuilder::add_internal_relation(
        const uuid& internal, const uuid& embedding )
    {
        relationships_.add_internal_relation( internal, embedding );
    }

    void RelationshipsBuilder::add_item_in_collection(
        const uuid& item, const uuid& collection )
    {
        relationships_.add_item_in_collection( item, collection );
    }

    void RelationshipsBuilder::load_relationships( absl::string_view directory )
    {
        relationships_.load_relationships( directory );
    }

} // namespace geode
