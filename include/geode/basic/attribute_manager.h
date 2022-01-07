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
#include <string>

#include <absl/container/fixed_array.h>

#include <bitsery/bitsery.h>

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/common.h>
#include <geode/basic/pimpl.h>

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
        AttributeManager( AttributeManager&& );
        ~AttributeManager();

        /*!
         * Recover the non-typed/generic Attribute from the attribute
         * name. This can be used when attribute type is not known in a
         * context.
         * @param[in] name The associated attribute name to look for.
         * @return nullptr if no attribute matches the given name.
         */
        std::shared_ptr< AttributeBase > find_generic_attribute(
            absl::string_view name ) const
        {
            return find_attribute_base( name );
        }

        /*!
         * Recover the typed Attribute from the attribute name
         * @param[in] name The associated attribute name to look for
         * @tparam T The type to of the ReadOnlyAttribute element
         * @exception OpenGeodeException if no Attribute found
         */
        template < typename T >
        std::shared_ptr< ReadOnlyAttribute< T > > find_attribute(
            absl::string_view name ) const
        {
            auto attribute =
                std::dynamic_pointer_cast< ReadOnlyAttribute< T > >(
                    find_attribute_base( name ) );
            OPENGEODE_EXCEPTION( attribute.get(),
                "[AttributeManager::find_attribute] You have to create an "
                "attribute before using it. "
                "See find_or_create_attribute method and "
                "derived classes of ReadOnlyAttribute." );
            return attribute;
        }

        /*!
         * Recover or create the attribute from the manager
         * and the attribute name.
         * If the recovered Attribute is not a of the same type than the
         * attribute, it replaces it by the Attribute corresponding to the
         * attribute.
         * @param[in] name The associated attribute name to look for
         * @param[in] default_value The default value to use when new attribute
         * element are created
         * @param[in] properties The AttributeProperties to set the attribute
         * flags for future modifications
         * @tparam Attribute The attribute type to look for,
         * such as ConstantAttribute
         * @tparam T The type of the Attribute element
         * @exception OpenGeodeException if the Attribute replacement failed
         */
        template < template < typename > class Attribute, typename T >
        std::shared_ptr< Attribute< T > > find_or_create_attribute(
            absl::string_view name,
            T default_value,
            AttributeProperties properties )
        {
            auto attribute = find_attribute_base( name );
            auto typed_attribute =
                std::dynamic_pointer_cast< Attribute< T > >( attribute );
            if( !typed_attribute.get() )
            {
                OPENGEODE_EXCEPTION( attribute.use_count() < 2,
                    "[AttributeManager::find_or_create_attribute] Do not "
                    "instantiate an attribute "
                    "if an instantiated attribute of the same name "
                    "with different storage already exists." );

                typed_attribute.reset( new Attribute< T >{
                    std::move( default_value ), std::move( properties ), {} } );
                register_attribute( typed_attribute, name );
            }
            return typed_attribute;
        }

        template < template < typename > class Attribute, typename T >
        std::shared_ptr< Attribute< T > > find_or_create_attribute(
            absl::string_view name, T default_value )
        {
            return find_or_create_attribute< Attribute, T >(
                name, std::move( default_value ), AttributeProperties{} );
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
         * Interpolate attribute value from other values in the same attribute
         * @param[in] interpolation Attribute interpolator
         * @param[in] to_element Where the value is assign
         * @warning Only affect Attributes created with its AttributeProperties
         * interpolable flag set to true
         */
        void interpolate_attribute_value(
            const AttributeLinearInterpolation& interpolation,
            index_t to_element );

        bool has_assignable_attributes() const;

        bool has_interpolable_attributes() const;

        /*!
         * Get all the associated attribute names
         */
        absl::FixedArray< absl::string_view > attribute_names() const;

        /*!
         * Return true if an attribute matching the given name.
         * @param[in] name The attribute name to use
         */
        bool attribute_exists( absl::string_view name ) const;

        /*!
         * Delete the attribute matching the given name.
         * Do nothing if the name does not exist.
         * @param[in] name The attribute name to delete
         */
        void delete_attribute( absl::string_view name );

        /*!
         * Get the typeid name of the attribute type
         * @param[in] name The attribute name to use
         */
        absl::string_view attribute_type( absl::string_view name ) const;

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
        index_t nb_elements() const;

        void copy( const AttributeManager& attribute_manager );

        template < typename Type, typename Serializer >
        static void register_attribute_type(
            PContext& context, absl::string_view name )
        {
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                ConstantAttribute< Type > >(
                absl::StrCat( "ConstantAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                VariableAttribute< Type > >(
                absl::StrCat( "VariableAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                SparseAttribute< Type > >(
                absl::StrCat( "SparseAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, ConstantAttribute< Type > >(
                absl::StrCat( "ConstantAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, VariableAttribute< Type > >(
                absl::StrCat( "VariableAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, SparseAttribute< Type > >(
                absl::StrCat( "SparseAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                ConstantAttribute< Type >, ConstantAttribute< Type > >(
                absl::StrCat( "ConstantAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                VariableAttribute< Type >, VariableAttribute< Type > >(
                absl::StrCat( "VariableAttribute", name ).c_str() );
            context.registerSingleBaseBranch< Serializer,
                SparseAttribute< Type >, SparseAttribute< Type > >(
                absl::StrCat( "SparseAttribute", name ).c_str() );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        /*!
         * Find the Attribute associated with the given name
         * regardless the content type
         * @param[in] name The attribute name to search for
         * @return The associated store. If the name was not found,
         * the shared pointer is empty.
         */
        std::shared_ptr< AttributeBase > find_attribute_base(
            absl::string_view name ) const;

        /*!
         * Register an Attribute to the given name.
         * If the given name already exists in the manager, the new attribute
         * will override the old one.
         * @param[in] attribute The attribute to register
         * @param[in] name The associated name to the store
         */
        void register_attribute( std::shared_ptr< AttributeBase > attribute,
            absl::string_view name );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
