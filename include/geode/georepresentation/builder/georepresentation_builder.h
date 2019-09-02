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

#pragma once

#include <geode/georepresentation/common.h>

#include <geode/georepresentation/core/georepresentation.h>

namespace geode
{
    /*!
     * Class managing modification of GeoRepresentation
     * It gives a modifiable access to the VertexIdentifier
     */
    template < index_t dimension,
        template < index_t > class... ComponentBuilderList >
    class GeoRepresentationBuilder : public ComponentBuilderList< dimension >...
    {
    public:
        void load_relationships( const std::string& directory )
        {
            relationships().load_relationships( directory );
        }

        VertexIdentifier& unique_vertices()
        {
            return unique_vertices_;
        }

    protected:
        template < template < index_t > class... ComponentList >
        GeoRepresentationBuilder(
            GeoRepresentation< dimension, ComponentList... >&
                georepresentation )
            : ComponentBuilderList< dimension >( georepresentation )...,
              relationships_( georepresentation.relationships_ ),
              unique_vertices_( georepresentation.unique_vertices_ )
        {
        }

        RelationshipManager& relationships()
        {
            return relationships_;
        }

    private:
        RelationshipManager& relationships_;
        VertexIdentifier& unique_vertices_;
    };
} // namespace geode
