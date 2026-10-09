/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/library.hpp>

#include <atomic>
#include <mutex>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class Library::Impl
    {
    public:
        void call_initialize( Library& library, const char* library_name )
        {
            geode_unused( library_name );
            if( is_loaded_.load( std::memory_order_acquire ) )
            {
                return;
            }
            // Recursive so that a re-entrant call from do_initialize returns
            // instead of deadlocking, other threads wait for the end
            const std::lock_guard< std::recursive_mutex > locking{ lock_ };
            if( is_loading_ || is_loaded_.load( std::memory_order_relaxed ) )
            {
                return;
            }
            is_loading_ = true;
            try
            {
                library.do_initialize();
            }
            catch( ... )
            {
                // A later call will retry the initialization
                is_loading_ = false;
                throw;
            }
            is_loading_ = false;
            is_loaded_.store( true, std::memory_order_release );
            DEBUG_LOGGER( library_name, " library initialized" );
        }

    private:
        std::recursive_mutex lock_;
        bool is_loading_{ false };
        std::atomic< bool > is_loaded_{ false };
    };

    Library::Library() = default;

    Library::~Library() = default;

    void Library::call_initialize( const char* library_name )
    {
        impl_->call_initialize( *this, library_name );
    }
} // namespace geode