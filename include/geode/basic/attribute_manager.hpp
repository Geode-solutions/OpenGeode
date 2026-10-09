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
#include <string>
#include <string_view>

#include <absl/container/fixed_array.h>
#include <absl/strings/str_cat.h>
#include <absl/synchronization/mutex.h>

#include <geode/basic/attribute.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    /*!
     * One step of a time series: the attribute holding the values at the
     * given time.
     */
    struct AttributeTimeStep
    {
        double time;
        uuid attribute_id;
    };

    /*!
     * This class manages all its associated Attributes.
     * Each Attribute is registered and can be retrieved by a given name.
     */
    class opengeode_basic_api AttributeManager
    {
        OPENGEODE_DISABLE_COPY( AttributeManager );

    public:
        AttributeManager();
        AttributeManager( AttributeManager&& other ) noexcept;
        AttributeManager& operator=( AttributeManager&& other ) noexcept;
        ~AttributeManager();

        /*!
         * Recover the non-typed/generic Attribute from the attribute
         * id. This can be used when attribute type is not known in a
         * context.
         * @param[in] attribute_id The associated attribute id to look for.
         * @return nullptr if no attribute matches the given id.
         */
        [[nodiscard]] std::shared_ptr< AttributeBase > find_generic_attribute(
            const geode::uuid& attribute_id ) const
        {
            absl::ReaderMutexLock lock{ mutex() };
            return find_attribute_base( attribute_id );
        }

        /*!
         * Recover the typed Attribute from the attribute name
         * @param[in] name The associated attribute name to look for
         * @tparam T The type to of the ReadOnlyAttribute element
         * @exception OpenGeodeException if no Attribute found
         */
        template < typename T >
        [[nodiscard]] std::shared_ptr< ReadOnlyAttribute< T > >
            find_read_only_attribute( const geode::uuid& attribute_id ) const
        {
            absl::ReaderMutexLock lock{ mutex() };
            auto attribute =
                std::dynamic_pointer_cast< ReadOnlyAttribute< T > >(
                    find_attribute_base( attribute_id ) );
            OpenGeodeBasicException::check_exception( attribute.get(), nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::find_read_only_attribute] Could not find "
                "attribute "
                "with id :'",
                attribute_id.string(),
                "'. You have to create an attribute before using it. See "
                "create_attribute method and derived classes of "
                "ReadOnlyAttribute." );
            return attribute;
        }

        template < template < typename > class Attribute, typename T >
        [[nodiscard]] std::shared_ptr< Attribute< T > > find_attribute(
            const geode::uuid& attribute_id )
        {
            absl::ReaderMutexLock lock{ mutex() };
            auto attribute = std::dynamic_pointer_cast< Attribute< T > >(
                find_attribute_base( attribute_id ) );
            OpenGeodeBasicException::check_exception( attribute.get(), nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::find_attribute] Could not find attribute "
                "with id :  '",
                attribute_id.string(),
                "'. You have to create an attribute before using it. See "
                "create_attribute method and derived classes of "
                "ReadOnlyAttribute." );
            return attribute;
        }

        template < template < typename > class Attribute, typename T >
        void create_attribute( std::string_view attribute_name,
            const geode::uuid& attribute_id,
            AttributeValues< T > default_values,
            AttributeProperties properties )
        {
            absl::MutexLock lock{ mutex() };
            create_attribute_unlocked< Attribute, T >( attribute_name,
                attribute_id, std::move( default_values ),
                std::move( properties ) );
        }

        template < template < typename > class Attribute, typename T >
        [[nodiscard]] geode::uuid create_attribute(
            std::string_view attribute_name,
            AttributeValues< T > default_values,
            AttributeProperties properties )
        {
            geode::uuid attribute_id;
            create_attribute< Attribute, T >( attribute_name, attribute_id,
                std::move( default_values ), std::move( properties ) );
            return attribute_id;
        }

        /*!
         * Create one step of a time series.
         * A time series is the set of attributes sharing a name and having a
         * time in their AttributeProperties.
         * @param[in] attribute_name The name of the series.
         * @param[in] time The time of this step, stored in properties.time.
         * @exception OpenGeodeException if time is not finite, if an
         * attribute with this name has no time, a different type or the same
         * time.
         */
        template < template < typename > class Attribute, typename T >
        [[nodiscard]] geode::uuid create_time_step_attribute(
            std::string_view attribute_name,
            double time,
            AttributeValues< T > default_values,
            AttributeProperties properties )
        {
            geode::uuid attribute_id;
            absl::MutexLock lock{ mutex() };
            check_new_time_step( attribute_name, time, typeid( T ).name() );
            properties.time = time;
            create_attribute_unlocked< Attribute, T >( attribute_name,
                attribute_id, std::move( default_values ),
                std::move( properties ) );
            return attribute_id;
        }

        /*!
         * Get the steps of the time series with the given name, sorted by
         * time. Empty if no attribute with this name has a time.
         */
        [[nodiscard]] std::vector< AttributeTimeStep > time_steps(
            std::string_view attribute_name ) const;

        /*!
         * Get the distinct names of the attributes having a time.
         */
        [[nodiscard]] std::vector< std::string > time_series_names() const;

        /*!
         * Resize all the attributes to the given size
         * @param[in] size The new attribute size
         */
        void resize( index_t size );

        /*!
         * Reserve all the attributes to the given capacity
         * @param[in] size The new attribute capacity
         */
        void reserve( index_t capacity );

        /*!
         * Assign attribute value from other value in the same attribute
         * @param[in] from_element Attribute value to assign
         * @param[in] to_element Where the value is assign
         * @warning Only affect Attributes created with its AttributeProperties
         * assignable flag set to true
         */
        void assign_attribute_value( index_t from_element, index_t to_element );

        /*!
         * Copy attribute value from other value in the same attribute
         * @param[in] from_element Attribute value to assign
         * @param[in] to_element Where the value is assigned
         */
        void copy_attribute_value( index_t from_element, index_t to_element );

        /*!
         * Interpolate attribute value from other values in the same attribute
         * @param[in] interpolation Attribute interpolator
         * @param[in] to_element Where the value is assign
         * @warning Only affect Attributes created with its AttributeProperties
         * interpolable flag set to true
         */
        void interpolate_attribute_value(
            const AttributeLinearInterpolation& interpolation,
            index_t to_element );

        [[nodiscard]] bool has_assignable_attributes() const;

        [[nodiscard]] bool has_interpolable_attributes() const;

        /*!
         * Get all the associated attribute ids
         */
        [[nodiscard]] absl::FixedArray< geode::uuid > attribute_ids() const;

        /*!
         * Return true if an attribute matching the given id.
         * @param[in] id The attribute id to use
         */
        [[nodiscard]] bool attribute_exists( const geode::uuid& ) const;

        /*!
         * Delete the attribute matching the given id.
         * Do nothing if the id does not exist.
         * @param[in] id The attribute id to delete
         */
        void delete_attribute( const geode::uuid& );

        /*!
         * Create a new attribute with the given id by copying the values
         * of an existing attribute of this manager.
         * @param[in] attribute_id The id of the attribute to copy.
         * @param[in] new_attribute_id The id to give to the new attribute.
         */
        void copy_attribute( const geode::uuid& attribute_id,
            const geode::uuid& new_attribute_id );

        /*!
         * Get the typeid id of the attribute type
         * @param[in] id The attribute id to use
         */
        [[nodiscard]] std::string_view attribute_type(
            const geode::uuid& ) const;

        /*!
         * Replace all the properties of the attribute.
         * @warning The time is replaced too: passing properties with an empty
         * time removes the attribute from its time series.
         */
        void set_attribute_properties( geode::uuid attribute_id,
            const AttributeProperties& new_properties );

        /*!
         * Remove all the attributes in the manager
         */
        void clear();

        /*!
         * Clear all the attribute content.
         * This is equivalent to calling resize( 0 ).
         */
        void clear_attributes();

        /*!
         * Delete a set of attribute elements.
         * @param[in] to_delete a vector of size @function nb_elements().
         * If to_delete[e] is true, then the element e will be destroyed.
         */
        void delete_elements( const std::vector< bool >& to_delete );

        /*!
         * Permute attribute elements.
         * @param[in] permutation Vector of size @function nb_elements().
         * permutation[new_index] is the old index of the element moved to
         * new_index.
         */
        void permute_elements( absl::Span< const index_t > permutation );

        /*!
         * Get the number of elements in each attribute
         */
        [[nodiscard]] index_t nb_elements() const;

        [[nodiscard]] std::optional< std::vector< uuid > >
            attribute_ids_matching_name( std::string_view name ) const;

        void copy( const AttributeManager& attribute_manager );

        void import( const AttributeManager& attribute_manager,
            const GenericMapping< index_t >& old2new_mapping );

        void import( const AttributeManager& attribute_manager,
            const GenericMapping< index_t >& old2new_mapping,
            const uuid& attribute_id );

    private:
        template < template < typename > class Attribute, typename T >
        void create_attribute_unlocked( std::string_view attribute_name,
            const geode::uuid& attribute_id,
            AttributeValues< T > default_values,
            AttributeProperties properties )
        {
            OpenGeodeBasicException::check_exception(
                find_attribute_base( attribute_id ) == nullptr, nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::create_attribute] Attribute with id '",
                attribute_id.string(), "' already exists." );
            std::shared_ptr< Attribute< T > > typed_attribute =
                std::make_unique< Attribute< T > >( std::move( default_values ),
                    attribute_name, std::move( properties ),
                    AttributeBase::AttributeKey{} );
            IdentifierBuilder builder{ *typed_attribute };
            builder.set_id( attribute_id );
            register_attribute( typed_attribute, attribute_id );
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer );

        [[nodiscard]] absl::Mutex& mutex() const;

        /*!
         * Find the Attribute associated with the given id
         * regardless the content type
         * @param[in] id The attribute id to search for
         * @return The associated store. If the id was not found,
         * the shared pointer is empty.
         */
        [[nodiscard]] std::shared_ptr< AttributeBase > find_attribute_base(
            const geode::uuid& ) const;

        /*!
         * Register an Attribute to the given id.
         * The given id should not already exist in the manager.
         * @param[in] attribute The attribute to register
         * @param[in] id The associated id to the store
         */
        void register_attribute(
            std::shared_ptr< AttributeBase > attribute, const geode::uuid& );

        /*!
         * Check that a new step can be added to the time series.
         */
        void check_new_time_step( std::string_view attribute_name,
            double time,
            std::string_view type ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
