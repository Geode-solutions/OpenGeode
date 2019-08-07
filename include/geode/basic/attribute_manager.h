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

#include <memory>
#include <string>

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
        OPENGEODE_DISABLE_COPY_AND_MOVE( AttributeManager );

    public:
        AttributeManager();
        ~AttributeManager();

        /*!
         * Recover the typed Attribute from the attribute name
         * @param[in] name The associated attribute name to look for
         * @tparam T The type to of the ReadOnlyAttribute element
         * @exception OpenGeodeException if no Attribute found
         */
        template < typename T >
        std::shared_ptr< ReadOnlyAttribute< T > > find_attribute(
            const std::string& name )
        {
            auto attribute =
                std::dynamic_pointer_cast< ReadOnlyAttribute< T > >(
                    find_attribute_base( name ) );
            OPENGEODE_EXCEPTION( attribute.get(),
                "You have to create an attribute before using it. "
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
         * @tparam Attribute The attribute type to look for,
         * such as ConstantAttribute
         * @tparam T The type of the Attribute element
         * @tparam Args The optional arguments to construct the Attribute
         * @exception OpenGeodeException if the Attribute replacement failed
         */
        template < template < typename > class Attribute,
            typename T,
            typename... Args >
        std::shared_ptr< Attribute< T > > find_or_create_attribute(
            const std::string& name, const Args&... args )
        {
            auto attribute = find_attribute_base( name );
            auto typed_attribute =
                std::dynamic_pointer_cast< Attribute< T > >( attribute );
            if( !typed_attribute.get() )
            {
                OPENGEODE_EXCEPTION( attribute.use_count() < 2,
                    "Do not instantiate an attribute "
                    "if an instantiated attribute of the same name "
                    "with different storage already exists." );

                typed_attribute.reset( new Attribute< T >( args... ) );
                register_attribute( typed_attribute, name );
            }
            return typed_attribute;
        }

        /*!
         * Resize all the attributes to the given size
         * @param[in] size The new attribute size
         */
        void resize( index_t size );

        /*!
         * Get all the associated attribute names
         */
        std::vector< std::string > attribute_names() const;

        /*!
         * Return true if an attribute matching the given name.
         * @param[in] name The attribute name to use
         */
        bool attribute_exists( const std::string& name ) const;

        /*!
         * Delete the attribute matching the given name.
         * Do nothing if the name does not exist.
         * @param[in] name The attribute name to delete
         */
        void delete_attribute( const std::string& name );

        /*!
         * Get the typeid name of the attribute type
         * @param[in] name The attribute name to use
         */
        std::string attribute_type( const std::string& name ) const;

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
         * Get the number of elements in each attribute
         */
        index_t nb_elements() const;

        template < typename Type, typename Serializer >
        static void register_attribute_type( PContext& context )
        {
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                ConstantAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                VariableAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer, AttributeBase,
                SparseAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, ConstantAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, VariableAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                ReadOnlyAttribute< Type >, SparseAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                ConstantAttribute< Type >, ConstantAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                VariableAttribute< Type >, VariableAttribute< Type > >();
            context.registerSingleBaseBranch< Serializer,
                SparseAttribute< Type >, SparseAttribute< Type > >();
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
            const std::string& name );

        /*!
         * Register an Attribute to the given name.
         * If the given name already exists in the manager, the new attribute
         * will override the old one.
         * @param[in] attribute The attribute to register
         * @param[in] name The associated name to the store
         */
        void register_attribute( std::shared_ptr< AttributeBase > attribute,
            const std::string& name );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
