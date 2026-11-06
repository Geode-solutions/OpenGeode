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

#pragma once

#include <memory>

#include <absl/container/fixed_array.h>
#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>
#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/growable.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    namespace internal
    {
        /*!
         * Named CRS storage shared by the mesh and grid CRS managers
         */
        template < typename CRS >
        class CoordinateReferenceSystemManagerImpl
        {
        public:
            [[nodiscard]] index_t nb_coordinate_reference_systems() const
            {
                return crss_.size();
            }

            [[nodiscard]] const CRS& find_coordinate_reference_system(
                std::string_view name ) const
            {
                return *find( name );
            }

            [[nodiscard]] const CRS& active_coordinate_reference_system() const
            {
                OpenGeodeMeshException::check_exception( active_crs_ != nullptr,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[CoordinateReferenceSystemManager::active_coordinate_"
                    "reference_system] Active CRS not defined" );
                return *active_crs_;
            }

            [[nodiscard]] std::string_view
                active_coordinate_reference_system_name() const
            {
                return active_crs_name_;
            }

            [[nodiscard]] absl::FixedArray< std::string >
                coordinate_reference_system_names() const
            {
                absl::FixedArray< std::string > names( crss_.size() );
                index_t count{ 0 };
                for( const auto& it : crss_ )
                {
                    names[count++] = it.first;
                }
                return names;
            }

            [[nodiscard]] bool coordinate_reference_system_exists(
                std::string_view name ) const
            {
                return crss_.find( name ) != crss_.end();
            }

            void register_coordinate_reference_system(
                std::string_view name, std::shared_ptr< CRS >&& crs )
            {
                const auto status =
                    crss_.emplace( to_string( name ), std::move( crs ) );
                OpenGeodeMeshException::check_exception( status.second, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[CoordinateReferenceSystemManager::register_coordinate_"
                    "reference_system] CRS named ",
                    name, " already exists" );
            }

            void delete_coordinate_reference_system( std::string_view name )
            {
                const auto it = crss_.find( name );
                if( it != crss_.end() )
                {
                    crss_.erase( it );
                }
                if( name == active_crs_name_ )
                {
                    active_crs_name_.clear();
                    active_crs_.reset();
                }
            }

            void set_active_coordinate_reference_system( std::string_view name )
            {
                active_crs_ = find( name );
                active_crs_name_ = to_string( name );
            }

            [[nodiscard]] CRS& modifiable_active_coordinate_reference_system()
            {
                OpenGeodeMeshException::check_exception( active_crs_ != nullptr,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[CoordinateReferenceSystemManager::modifiable_active_"
                    "coordinate_reference_system] Active CRS not defined" );
                return *active_crs_;
            }

            [[nodiscard]] CRS& modifiable_coordinate_reference_system(
                std::string_view name )
            {
                return *find( name );
            }

        protected:
            [[nodiscard]] bool has_active_coordinate_reference_system() const
            {
                return active_crs_ != nullptr;
            }

            /*!
             * Replace the stored CRSs by clones of the given ones
             */
            void copy( const CoordinateReferenceSystemManagerImpl& other )
            {
                crss_.clear();
                for( const auto& [name, crs] : other.crss_ )
                {
                    crss_.emplace( name, crs->clone() );
                }
                active_crs_.reset();
                active_crs_name_.clear();
                if( other.active_crs_ )
                {
                    set_active_coordinate_reference_system(
                        other.active_crs_name_ );
                }
            }

            template < typename Archive >
            void serialize_coordinate_reference_systems( Archive& archive )
            {
                archive.ext( crss_, bitsery::ext::StdMap{ crss_.max_size() },
                    []( Archive& archive2, std::string& name,
                        std::shared_ptr< CRS >& crs ) {
                        archive2.text1b( name, MAX_SERIALIZED_NAME_SIZE );
                        archive2.ext( crs, bitsery::ext::StdSmartPtr{} );
                    } );
                archive.ext( active_crs_, bitsery::ext::StdSmartPtr{} );
                archive.text1b( active_crs_name_, MAX_SERIALIZED_NAME_SIZE );
                check_active_crs();
            }

        private:
            const std::shared_ptr< CRS >& find( std::string_view name ) const
            {
                const auto it = crss_.find( name );
                OpenGeodeMeshException::check_exception( it != crss_.end(),
                    nullptr, OpenGeodeException::TYPE::data,
                    "[CoordinateReferenceSystemManager] Unknown CRS: ", name );
                return it->second;
            }

            void check_active_crs() const
            {
                if( !active_crs_ )
                {
                    OpenGeodeMeshException::check_exception(
                        active_crs_name_.empty(), nullptr,
                        OpenGeodeException::TYPE::data,
                        "[CoordinateReferenceSystemManager] Active CRS name "
                        "without active CRS: file may be corrupted" );
                    return;
                }
                const auto it = crss_.find( active_crs_name_ );
                OpenGeodeMeshException::check_exception(
                    it != crss_.end() && it->second == active_crs_, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[CoordinateReferenceSystemManager] Active CRS is not a "
                    "registered CRS: file may be corrupted" );
            }

        private:
            absl::flat_hash_map< std::string, std::shared_ptr< CRS > > crss_;
            std::shared_ptr< CRS > active_crs_;
            std::string active_crs_name_;
        };
    } // namespace internal
} // namespace geode
