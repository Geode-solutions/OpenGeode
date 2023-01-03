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

#include <geode/mesh/core/mesh_factory.h>

#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class MeshFactory::Impl
    {
    public:
        void register_type( MeshType type, MeshImpl key )
        {
            types_.emplace( std::move( key ), std::move( type ) );
        }

        void register_default( MeshType type, MeshImpl key )
        {
            defaults_.emplace( std::move( type ), std::move( key ) );
        }

        const MeshType& type( const MeshImpl& key ) const
        {
            return types_.at( key );
        }

        const MeshImpl& default_impl( const MeshType& type ) const
        {
            return defaults_.at( type );
        }

    private:
        absl::flat_hash_map< MeshImpl, MeshType > types_;
        absl::flat_hash_map< MeshType, MeshImpl > defaults_;
    };

    MeshFactory::MeshFactory() {}

    MeshFactory::~MeshFactory() {}

    MeshFactory& MeshFactory::instance()
    {
        static MeshFactory mesh_factory;
        return mesh_factory;
    }

    void MeshFactory::register_type( MeshType type, MeshImpl key )
    {
        instance().impl_->register_type( std::move( type ), std::move( key ) );
    }

    const MeshType& MeshFactory::type( const MeshImpl& key )
    {
        return instance().impl_->type( key );
    }

    const MeshImpl& MeshFactory::default_impl( const MeshType& type )
    {
        return instance().impl_->default_impl( type );
    }

    void MeshFactory::register_default( MeshType type, MeshImpl key )
    {
        instance().impl_->register_default(
            std::move( type ), std::move( key ) );
    }

} // namespace geode
