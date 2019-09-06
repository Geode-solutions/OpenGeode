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

#include <mutex>

#include <geode/georepresentation/common.h>

#include <geode/georepresentation/core/relationship_manager.h>
#include <geode/georepresentation/core/vertex_identifier.h>

namespace geode
{
    class GeoRepresentationBuilder;
} // namespace geode

namespace geode
{
    /*!
     * This abstract class represents an assembly of geometric components.
     * The geometric component types composing the GeoRepresentation
     * are flexible.
     * Syntax for create a derivated class is
     * class DerivateClass : public GeoRepresentation<
     *	dimension, ComponentClassA, ComponentClassB, ComponentClassC >
     */
    class GeoRepresentation
    {
        friend class GeoRepresentationBuilder;

    public:
        const RelationshipManager& relationships() const
        {
            return relationships_;
        }

        const VertexIdentifier& unique_vertices() const
        {
            return unique_vertices_;
        }

        double epsilon() const
        {
            std::call_once( is_epsilon_initialized_,
                &GeoRepresentation::compute_epsilon, this,
                std::ref( epsilon_ ) );
            return epsilon_;
        }

    protected:
        GeoRepresentation() = default;

    private:
        virtual void compute_epsilon( double& epsilon ) const = 0;

    protected:
        static constexpr double epsilon_scaling = 1e-7;

    private:
        RelationshipManager relationships_;
        VertexIdentifier unique_vertices_;

        mutable std::once_flag is_epsilon_initialized_;
        mutable double epsilon_{ 0 };
    };
} // namespace geode
