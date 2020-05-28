/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/mesh_factory.h>

#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class MeshFactory::Impl
    {
    public:
        void register_kind( MeshKind kind, MeshType key )
        {
            kinds_.emplace( std::move( key ), std::move( kind ) );
        }

        void register_default( MeshKind kind, MeshType key )
        {
            defaults_.emplace( std::move( kind ), std::move( key ) );
        }

        const MeshKind& kind( const MeshType& key ) const
        {
            return kinds_.at( key );
        }

        const MeshType& default_mesh( const MeshKind& kind ) const
        {
            return defaults_.at( kind );
        }

    private:
        absl::flat_hash_map< MeshType, MeshKind > kinds_;
        absl::flat_hash_map< MeshKind, MeshType > defaults_;
    };

    MeshFactory::MeshFactory() {}

    MeshFactory::~MeshFactory() {}

    MeshFactory& MeshFactory::instance()
    {
        static MeshFactory mesh_factory;
        return mesh_factory;
    }

    void MeshFactory::register_kind( MeshKind kind, MeshType key )
    {
        instance().impl_->register_kind( std::move( kind ), std::move( key ) );
    }

    const MeshKind& MeshFactory::kind( const MeshType& key )
    {
        return instance().impl_->kind( key );
    }

    const MeshType& MeshFactory::default_mesh( const MeshKind& kind )
    {
        return instance().impl_->default_mesh( kind );
    }

    void MeshFactory::register_default( MeshKind kind, MeshType key )
    {
        instance().impl_->register_default(
            std::move( kind ), std::move( key ) );
    }

} // namespace geode
