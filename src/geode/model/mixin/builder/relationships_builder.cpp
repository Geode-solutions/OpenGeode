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

#include <geode/model/mixin/builder/relationships_builder.hpp>

namespace geode
{
    RelationshipsBuilder::RelationshipsBuilder( Relationships& relationships )
        : relationships_( relationships )
    {
    }

    void RelationshipsBuilder::unregister_component( const uuid& component_id )
    {
        relationships_.remove_component( component_id, {} );
    }

    index_t RelationshipsBuilder::add_boundary_relation(
        const ComponentID& boundary, const ComponentID& incidence )
    {
        return relationships_.add_boundary_relation( boundary, incidence, {} );
    }

    index_t RelationshipsBuilder::add_internal_relation(
        const ComponentID& internal, const ComponentID& embedding )
    {
        return relationships_.add_internal_relation( internal, embedding, {} );
    }

    index_t RelationshipsBuilder::add_item_in_collection(
        const ComponentID& item, const ComponentID& collection )
    {
        return relationships_.add_item_in_collection( item, collection, {} );
    }

    void RelationshipsBuilder::remove_relation(
        const uuid& component_id1, const uuid& component_id2 )
    {
        return relationships_.remove_relation(
            component_id1, component_id2, {} );
    }

    void RelationshipsBuilder::copy_relationships(
        const ModelCopyMapping& mapping, const Relationships& relationships )
    {
        relationships_.copy_relationships( mapping, relationships, {} );
    }

    void RelationshipsBuilder::load_relationships( std::string_view directory )
    {
        relationships_.load_relationships( directory, {} );
    }

} // namespace geode
