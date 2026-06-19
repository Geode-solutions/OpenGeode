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
#include <geode/basic/pimpl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
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
         * name. This can be used when attribute type is not known in a
         * context.
         * @param[in] name The associated attribute name to look for.
         * @return nullptr if no attribute matches the given name.
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
        [[nodiscard]] std::shared_ptr< ReadOnlyAttribute< T > > read_attribute(
            const geode::uuid& attribute_id ) const
        {
            absl::ReaderMutexLock lock{ mutex() };
            auto attribute =
                std::dynamic_pointer_cast< ReadOnlyAttribute< T > >(
                    find_attribute_base( attribute_id ) );
            OpenGeodeBasicException::check_exception( attribute.get(), nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::find_attribute] Could not find attribute '",
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
                "[AttributeManager::find_attribute] Could not find attribute '",
                attribute_id.string(),
                "'. You have to create an attribute before using it. See "
                "create_attribute method and derived classes of "
                "ReadOnlyAttribute." );
            return attribute;
        }

        template < template < typename > class Attribute, typename T >
        void create_attribute( std::string_view attribute_name,
            const geode::uuid& attribute_id,
            T default_value,
            AttributeProperties properties )
        {
            absl::MutexLock lock{ mutex() };
            auto attribute = find_attribute_base( attribute_id );
            auto typed_attribute =
                std::dynamic_pointer_cast< Attribute< T > >( attribute );
            OpenGeodeBasicException::check_exception(
                typed_attribute.get() == nullptr, nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::create_attribute] Attribute '",
                attribute_id.string(), "' already exists." );
            typed_attribute.reset(
                new Attribute< T >{ std::move( default_value ), attribute_name,
                    std::move( properties ), {} } );
            IdentifierBuilder builder{ *typed_attribute };
            builder.set_id( attribute_id );
            register_attribute( typed_attribute, attribute_id );
        }

        template < template < typename > class Attribute, typename T >
        void create_attribute( std::string_view attribute_name,
            const geode::uuid& attribute_id,
            T default_value )
        {
            return create_attribute< Attribute, T >( attribute_name,
                attribute_id, std::move( default_value ),
                AttributeProperties{} );
        }

        template < template < typename > class Attribute, typename T >
        [[nodiscard]] geode::uuid create_attribute(
            std::string_view attribute_name,
            T default_value,
            AttributeProperties properties )
        {
            geode::uuid attribute_id;
            create_attribute< Attribute, T >( attribute_name, attribute_id,
                std::move( default_value ), std::move( properties ) );
            return attribute_id;
        }

        template < template < typename > class Attribute, typename T >
        [[nodiscard]] geode::uuid create_attribute(
            std::string_view attribute_name, T default_value )
        {
            return create_attribute< Attribute, T >( attribute_name,
                std::move( default_value ), AttributeProperties{} );
        }

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
         * Get all the associated attribute names
         */
        [[nodiscard]] absl::FixedArray< geode::uuid > attribute_ids() const;

        /*!
         * Return true if an attribute matching the given name.
         * @param[in] name The attribute name to use
         */
        [[nodiscard]] bool attribute_exists( const geode::uuid& ) const;

        /*!
         * Delete the attribute matching the given name.
         * Do nothing if the name does not exist.
         * @param[in] name The attribute name to delete
         */
        void delete_attribute( const geode::uuid& );

        /*!
         * Get the typeid name of the attribute type
         * @param[in] name The attribute name to use
         */
        [[nodiscard]] std::string_view attribute_type(
            const geode::uuid& ) const;

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
         * Each value correponds to the destination index.
         */
        void permute_elements( absl::Span< const index_t > permutation );

        /*!
         * Get the number of elements in each attribute
         */
        [[nodiscard]] index_t nb_elements() const;

        [[nodiscard]] std::optional< std::string_view > attribute_name(
            const uuid& ) const;

        std::optional< std::vector< uuid > > attribute_ids_with_name(
            std::string_view name ) const;

        void copy( const AttributeManager& attribute_manager );

        void import( const AttributeManager& attribute_manager,
            absl::Span< const index_t > old2new );

        void import( const AttributeManager& attribute_manager,
            absl::Span< const index_t > old2new,
            uuid attribute_id );

        void import( const AttributeManager& attribute_manager,
            const GenericMapping< index_t >& old2new_mapping );

        void import( const AttributeManager& attribute_manager,
            const GenericMapping< index_t >& old2new_mapping,
            uuid attribute_id );

    private:
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
         * If the given id already exists in the manager, the new attribute
         * will override the old one.
         * @param[in] attribute The attribute to register
         * @param[in] id The associated id to the store
         */
        void register_attribute(
            std::shared_ptr< AttributeBase > attribute, const geode::uuid& );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
