/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        class MeshStorage
        {
        public:
            MeshStorage() : mesh_type_{ "" } {}

            void set_mesh( std::unique_ptr< Mesh > mesh )
            {
                mesh_type_ = mesh->impl_name();
                mesh_ = std::move( mesh );
            }

            const Mesh& mesh() const
            {
                return *mesh_;
            }

            Mesh& modifiable_mesh()
            {
                return *mesh_;
            }

            const MeshImpl& mesh_type() const
            {
                return mesh_type_;
            }

            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, DefaultGrowable< Archive, MeshStorage >{},
                    []( Archive& a, MeshStorage& storage ) {
                        a.object( storage.mesh_type_ );
                    } );
            }

        private:
            std::unique_ptr< Mesh > mesh_;
            MeshImpl mesh_type_;
        };
    } // namespace detail
} // namespace geode
