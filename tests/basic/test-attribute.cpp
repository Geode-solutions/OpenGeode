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

#include <fstream>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/tests/common.h>

struct Foo
{
    bool operator==( const Foo& foo ) const
    {
        return double_ == foo.double_ || int_ == foo.int_;
    }

    bool operator!=( const Foo& foo ) const
    {
        return double_ != foo.double_ || int_ != foo.int_;
    }

    float generic_value() const
    {
        return double_ + int_;
    }

    template < typename Archive >
    void serialize( Archive& archive )
    {
        archive.value8b( double_ );
        archive.value4b( int_ );
    }
    double double_{ 0 };
    int int_{ 0 };
};

namespace geode
{
    template <>
    struct GenericAttributeConversion< Foo >
    {
        static float converted_value( const Foo& value )
        {
            return value.generic_value();
        }
    };

    template <>
    struct AttributeLinearInterpolationImpl< Foo >
    {
        template < template < typename > class Attribute >
        static Foo compute( const AttributeLinearInterpolation& interpolator,
            const Attribute< Foo >& attribute )
        {
            Foo result;
            for( auto i : Range{ interpolator.indices_.size() } )
            {
                result.double_ +=
                    interpolator.lambdas_[i]
                    * attribute.value( interpolator.indices_[i] ).double_;
            }
            return result;
        }
    };
} // namespace geode

void test_constant_attribute( geode::AttributeManager& manager )
{
    auto constant_attribute =
        manager.find_or_create_attribute< geode::ConstantAttribute, bool >(
            "bool", true );

    auto attribute = manager.find_attribute< bool >( "bool" );
    OPENGEODE_EXCEPTION(
        attribute->value( 0 ), "[Test] Should be equal to true" );
    OPENGEODE_EXCEPTION( attribute->type() == typeid( bool ).name(),
        "[Test] Should be equal to 'b' or 'bool'" );

    constant_attribute->set_value( false );
    OPENGEODE_EXCEPTION(
        !attribute->value( 12 ), "[Test] Should be equal to false" );
}

void test_foo_constant_attribute( geode::AttributeManager& manager )
{
    auto constant_attribute =
        manager.find_or_create_attribute< geode::ConstantAttribute, Foo >(
            "foo_cst", Foo{} );
    constant_attribute->modify_value( []( Foo& foo ) { foo.double_ = 12.4; } );
    OPENGEODE_EXCEPTION( constant_attribute->value().double_ == 12.4,
        "[Test] Should be equal to 12.4" );
}

void test_foo_variable_attribute( geode::AttributeManager& manager )
{
    auto variable_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute, Foo >(
            "foo_var", Foo{} );
    variable_attribute->modify_value(
        3, []( Foo& foo ) { foo.double_ = 12.4; } );
    OPENGEODE_EXCEPTION( variable_attribute->value( 0 ).double_ == 0,
        "[Test] Should be equal to 0" );
    OPENGEODE_EXCEPTION( variable_attribute->value( 3 ).double_ == 12.4,
        "[Test] Should be equal to 12.4" );
}

void test_int_variable_attribute( geode::AttributeManager& manager )
{
    auto variable_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute, int >(
            "int", 12 );
    variable_attribute->set_value( 3, 3 );

    const auto attribute = manager.find_attribute< int >( "int" );
    OPENGEODE_EXCEPTION(
        attribute->value( 3 ) == 3, "[Test] Should be equal to 3" );
    OPENGEODE_EXCEPTION(
        attribute->value( 6 ) == 12, "[Test] Should be equal to 12" );

    variable_attribute->set_value( 3, 5 );
    OPENGEODE_EXCEPTION(
        attribute->value( 3 ) == 5, "[Test] Should be equal to 5" );
}

void test_foo_sparse_attribute( geode::AttributeManager& manager )
{
    auto sparse_attribute =
        manager.find_or_create_attribute< geode::SparseAttribute, Foo >(
            "foo_spr", Foo{} );
    sparse_attribute->modify_value( 3, []( Foo& foo ) { foo.double_ = 12.4; } );
    sparse_attribute->modify_value( 3, []( Foo& foo ) { foo.int_ = 3; } );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 0 ).double_ == 0,
        "[Test] Should be equal to 0" );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 3 ).double_ == 12.4,
        "[Test] Should be equal to 12.4" );
    OPENGEODE_EXCEPTION(
        sparse_attribute->value( 3 ).int_ == 3, "[Test] Should be equal to 3" );
}

void test_double_sparse_attribute( geode::AttributeManager& manager )
{
    auto sparse_attribute =
        manager.find_or_create_attribute< geode::SparseAttribute, double >(
            "double", 12., { true, true } );
    sparse_attribute->set_value( 3, 3 );
    sparse_attribute->set_value( 7, 7 );
    manager.assign_attribute_value( 3, 2 );
    manager.interpolate_attribute_value( { { 1, 7 }, { 0.5, 0.3 } }, 4 );

    auto attribute = manager.find_attribute< double >( "double" );
    OPENGEODE_EXCEPTION(
        attribute->value( 2 ) == 3, "[Test] Should be equal to 3" );
    OPENGEODE_EXCEPTION(
        attribute->value( 3 ) == 3, "[Test] Should be equal to 3" );
    OPENGEODE_EXCEPTION(
        attribute->value( 4 ) == 8.1, "[Test] Should be equal to 8.1" );
    OPENGEODE_EXCEPTION(
        attribute->value( 6 ) == 12, "[Test] Should be equal to 12" );
    OPENGEODE_EXCEPTION(
        attribute->value( 7 ) == 7, "[Test] Should be equal to 7" );

    sparse_attribute->set_value( 3, 5 );
    OPENGEODE_EXCEPTION(
        attribute->value( 3 ) == 5, "[Test] Should be equal to 5" );
}

void test_bool_variable_attribute( geode::AttributeManager& manager )
{
    auto variable_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute, bool >(
            "bool_var", false );
    variable_attribute->set_value( 3, true );

    const auto attribute = manager.find_attribute< bool >( "bool_var" );
    OPENGEODE_EXCEPTION(
        attribute->value( 3 ), "[Test] Should be equal to true" );

    variable_attribute->set_value( 3, false );
    OPENGEODE_EXCEPTION(
        !attribute->value( 3 ), "[Test] Should be equal to false" );
}

bool managers_have_same_attributes( const geode::AttributeManager& manager,
    const geode::AttributeManager& reloaded_manager )
{
    const auto& attribute_names = manager.attribute_names();
    const auto& reloaded_attribute_names = reloaded_manager.attribute_names();
    if( attribute_names.size() != reloaded_attribute_names.size() )
    {
        return false;
    }
    for( const auto& att : attribute_names )
    {
        if( absl::c_find( reloaded_attribute_names, att )
            == reloaded_attribute_names.end() )
        {
            return false;
        }
    }
    for( const auto& att : reloaded_attribute_names )
    {
        if( absl::c_find( attribute_names, att ) == attribute_names.end() )
        {
            return false;
        }
    }
    return true;
}

template < typename T >
void check_one_attribute_values( geode::AttributeManager& manager,
    geode::AttributeManager& reloaded_manager,
    absl::string_view name )
{
    const auto in_att = manager.find_attribute< T >( name );
    const auto out_att = reloaded_manager.find_attribute< T >( name );
    for( auto i : geode::Range{ manager.nb_elements() } )
    {
        OPENGEODE_EXCEPTION( in_att->value( i ) == out_att->value( i ),
            "[Test] At least one value of Attribute ", name,
            " is not correct after reloading" );
    }
}

void check_attribute_values( geode::AttributeManager& manager,
    geode::AttributeManager& reloaded_manager )
{
    check_one_attribute_values< bool >( manager, reloaded_manager, "bool" );
    check_one_attribute_values< int >( manager, reloaded_manager, "int" );
    check_one_attribute_values< double >( manager, reloaded_manager, "double" );
    check_one_attribute_values< bool >( manager, reloaded_manager, "bool_var" );
    check_one_attribute_values< Foo >( manager, reloaded_manager, "foo_cst" );
    check_one_attribute_values< Foo >( manager, reloaded_manager, "foo_var" );
    check_one_attribute_values< Foo >( manager, reloaded_manager, "foo_spr" );
}

void test_serialize_manager( geode::AttributeManager& manager )
{
    const auto filename = "manager.out";
    std::ofstream file{ filename, std::ofstream::binary };
    geode::TContext context{};
    geode::register_basic_serialize_pcontext( std::get< 0 >( context ) );
    geode::AttributeManager::register_attribute_type< Foo, geode::Serializer >(
        std::get< 0 >( context ) );
    geode::AttributeManager::register_attribute_type< std::array< double, 3 >,
        geode::Serializer >( std::get< 0 >( context ) );
    geode::Serializer archive{ context, file };
    archive.object( manager );
    archive.adapter().flush();
    OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
        "[Test] Error while writing file: ", filename );

    std::ifstream infile{ filename, std::ifstream::binary };
    geode::AttributeManager reloaded_manager;
    geode::TContext reload_context{};
    geode::register_basic_deserialize_pcontext(
        std::get< 0 >( reload_context ) );
    geode::AttributeManager::register_attribute_type< Foo,
        geode::Deserializer >( std::get< 0 >( reload_context ) );
    geode::AttributeManager::register_attribute_type< std::array< double, 3 >,
        geode::Deserializer >( std::get< 0 >( reload_context ) );
    geode::Deserializer unarchive{ reload_context, infile };
    unarchive.object( reloaded_manager );
    const auto& adapter = unarchive.adapter();
    OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                             && adapter.isCompletedSuccessfully()
                             && std::get< 1 >( context ).isValid(),
        "[Test] Error while reading file: ", filename );

    OPENGEODE_EXCEPTION(
        reloaded_manager.nb_elements() == manager.nb_elements(),
        "[Test] Number of elements in reloaded AttributeManager is not "
        "correct" );
    OPENGEODE_EXCEPTION(
        managers_have_same_attributes( manager, reloaded_manager ),
        "[Test] Number and names of attributes in reloaded AttributeManager "
        "are not correct" );
    check_attribute_values( manager, reloaded_manager );
}

void test_attribute_types( geode::AttributeManager& manager )
{
    OPENGEODE_EXCEPTION(
        manager.attribute_type( "bool_var" ) == typeid( bool ).name(),
        "[Test] Returned attribute type is not correct (should be bool)" );
    OPENGEODE_EXCEPTION(
        manager.attribute_type( "unknown_name" ) == "undefined",
        "[Test] Returned attribute type is not correct (should be undefined)" );
}

void test_number_of_attributes(
    geode::AttributeManager& manager, geode::index_t nb )
{
    OPENGEODE_EXCEPTION( manager.attribute_names().size() == nb,
        "[Test] Should have ", nb, " attributes in the manager" );
}

void test_delete_attribute_elements( geode::AttributeManager& manager )
{
    std::vector< bool > to_delete( manager.nb_elements(), false );
    to_delete[3] = true;
    to_delete[5] = true;
    manager.delete_elements( to_delete );
    OPENGEODE_EXCEPTION( manager.nb_elements() == to_delete.size() - 2,
        "[Test] Two attribute elements should have been removed" );
}

void test_sparse_attribute_after_element_deletion(
    geode::AttributeManager& manager )
{
    const auto sparse_attribute = manager.find_attribute< double >( "double" );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 0 ) == 12,
        "Element 0 of sparse attribute should be 12 " );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 3 ) == 8.1,
        "Element 3 of sparse attribute should be 8.1 " );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 5 ) == 7,
        "Element 5 of sparse attribute should be 7 " );
    OPENGEODE_EXCEPTION( sparse_attribute->value( 7 ) == 12,
        "Element 7 of sparse attribute should be 12 " );
}

void test_generic_value( geode::AttributeManager& manager )
{
    const auto& foo_attr = manager.find_attribute< Foo >( "foo_spr" );
    OPENGEODE_EXCEPTION( foo_attr->generic_value( 3 ) == 15.4f,
        "Generic value for element 3 of foo sparse attribute should be 15.4" );

    const auto& double_attr = manager.find_attribute< double >( "double" );
    OPENGEODE_EXCEPTION( double_attr->generic_value( 7 ) == 7,
        "Generic value for element 7 of double attribute should be 7" );

    auto array_attr =
        manager.find_or_create_attribute< geode::VariableAttribute,
            std::array< double, 3 > >(
            "array_double", std::array< double, 3 >() );
    array_attr->set_value( 2, { 3.1, 1.3 } );
    OPENGEODE_EXCEPTION( array_attr->generic_value( 2 ) == 0.,
        "Generic value for element 2 of array attribute should be 0." );
}

void test()
{
    geode::AttributeManager manager;
    manager.resize( 10 );
    OPENGEODE_EXCEPTION(
        manager.nb_elements() == 10, "[Test] Manager should have 10 elements" );
    test_constant_attribute( manager );
    test_foo_constant_attribute( manager );
    test_int_variable_attribute( manager );
    test_bool_variable_attribute( manager );
    test_foo_variable_attribute( manager );
    test_double_sparse_attribute( manager );
    test_foo_sparse_attribute( manager );
    test_generic_value( manager );
    test_delete_attribute_elements( manager );
    test_sparse_attribute_after_element_deletion( manager );

    test_serialize_manager( manager );

    test_attribute_types( manager );
    test_number_of_attributes( manager, 8 );
    manager.delete_attribute( "bool" );
    test_number_of_attributes( manager, 7 );
    manager.clear_attributes();
    test_number_of_attributes( manager, 7 );
    manager.resize( 10 );
    OPENGEODE_EXCEPTION(
        manager.nb_elements() == 10, "[Test] Manager should have 10 elements" );
    manager.clear();
    test_number_of_attributes( manager, 0 );
}

OPENGEODE_TEST( "attribute" )