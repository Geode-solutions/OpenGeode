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

#include <fstream>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

void test_constant_attribute( geode::AttributeManager& manager )
{
    auto constant_attribute =
        manager.find_or_create_attribute< geode::ConstantAttribute, bool >(
            "bool" );
    constant_attribute->value() = true;

    auto attribute = manager.find_attribute< bool >( "bool" );
    OPENGEODE_EXCEPTION( attribute->value( 0 ), "[Test] Should be equal to true" );
    OPENGEODE_EXCEPTION( attribute->type() == typeid( bool ).name(),
        "[Test] Should be equal to 'b' or 'bool'" );

    constant_attribute->value() = false;
    OPENGEODE_EXCEPTION( !attribute->value( 12 ), "[Test] Should be equal to false" );
}

void test_int_variable_attribute( geode::AttributeManager& manager )
{
    auto variable_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute, int >(
            "int", 12 );
    variable_attribute->value( 3 ) = 3;

    auto attribute = manager.find_attribute< int >( "int" );
    OPENGEODE_EXCEPTION( attribute->value( 3 ) == 3, "[Test] Should be equal to 3" );
    OPENGEODE_EXCEPTION( attribute->value( 6 ) == 12, "[Test] Should be equal to 12" );

    variable_attribute->value( 3 ) = 5;
    OPENGEODE_EXCEPTION( attribute->value( 3 ) == 5, "[Test] Should be equal to 5" );
}

void test_double_sparse_attribute( geode::AttributeManager& manager )
{
    auto sparse_attribute =
        manager.find_or_create_attribute< geode::SparseAttribute, double >(
            "double", 12 );
    sparse_attribute->value( 3 ) = 3;

    auto attribute = manager.find_attribute< double >( "double" );
    OPENGEODE_EXCEPTION( attribute->value( 3 ) == 3, "[Test] Should be equal to 3" );
    OPENGEODE_EXCEPTION( attribute->value( 6 ) == 12, "[Test] Should be equal to 12" );

    sparse_attribute->value( 3 ) = 5;
    OPENGEODE_EXCEPTION( attribute->value( 3 ) == 5, "[Test] Should be equal to 5" );
}

void test_bool_variable_attribute( geode::AttributeManager& manager )
{
    auto variable_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute, bool >(
            "bool_var", false );
    variable_attribute->value( 3 ) = true;

    auto attribute = manager.find_attribute< bool >( "bool_var" );
    OPENGEODE_EXCEPTION( attribute->value( 3 ), "[Test] Should be equal to true" );

    variable_attribute->value( 3 ) = false;
    OPENGEODE_EXCEPTION( !attribute->value( 3 ), "[Test] Should be equal to false" );
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
        if( std::find( reloaded_attribute_names.begin(),
                reloaded_attribute_names.end(), att )
            == reloaded_attribute_names.end() )
        {
            return false;
        }
    }
    for( const auto& att : reloaded_attribute_names )
    {
        if( std::find( attribute_names.begin(), attribute_names.end(), att )
            == attribute_names.end() )
        {
            return false;
        }
    }
    return true;
}

template < typename T >
void check_one_attribute_values( geode::AttributeManager& manager,
    geode::AttributeManager& reloaded_manager,
    const std::string& name )
{
    auto in_att = manager.find_attribute< T >( name );
    auto out_att = reloaded_manager.find_attribute< T >( name );
    for( auto i : geode::Range{ manager.nb_elements() } )
    {
        OPENGEODE_EXCEPTION( in_att->value( i ) == out_att->value( i ),
            "[Test] At least one value of Attribute " + name
                + " is not correct after reloading" );
    }
}

void check_attribute_values( geode::AttributeManager& manager,
    geode::AttributeManager& reloaded_manager )
{
    check_one_attribute_values< bool >( manager, reloaded_manager, "bool" );
    check_one_attribute_values< int >( manager, reloaded_manager, "int" );
    check_one_attribute_values< double >( manager, reloaded_manager, "double" );
    check_one_attribute_values< bool >( manager, reloaded_manager, "bool_var" );
}

void test_serialize_manager( geode::AttributeManager& manager )
{
    std::string filename = "manager.out";
    std::ofstream file{ filename, std::ofstream::binary };
    geode::TContext context{};
    geode::register_basic_serialize_pcontext( std::get< 0 >( context ) );
    geode::Serializer archive{ context, file };
    archive.object( manager );
    archive.adapter().flush();
    OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
        "[Test] Error while writing file: " + filename );

    std::ifstream infile{ filename, std::ifstream::binary };
    geode::AttributeManager reloaded_manager;
    geode::TContext reload_context{};
    geode::register_basic_deserialize_pcontext(
        std::get< 0 >( reload_context ) );
    geode::Deserializer unarchive{ reload_context, infile };
    unarchive.object( reloaded_manager );
    auto& adapter = unarchive.adapter();
    OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                             && adapter.isCompletedSuccessfully()
                             && std::get< 1 >( context ).isValid(),
        "[Test] Error while reading file: " + filename );

    OPENGEODE_EXCEPTION(
        reloaded_manager.nb_elements() == manager.nb_elements(),
        "[Test] Number of elements in reloaded AttributeManager is not correct" );
    OPENGEODE_EXCEPTION(
        managers_have_same_attributes( manager, reloaded_manager ),
        "[Test] Number and names of attributes in reloaded AttributeManager are not "
        "correct" );
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
        "[Test] Should have " + std::to_string( nb ) + " attributes in the manager" );
}

void test_delete_attribute_elements( geode::AttributeManager& manager )
{
    std::vector< bool > to_delete( manager.nb_elements(), false );
    to_delete[3] = true;
    to_delete[5] = true;
    manager.delete_elements( to_delete );
    OPENGEODE_EXCEPTION( manager.nb_elements() == to_delete.size() - 2,
        "[Test] Two attribute elements should have being removed" );
}

int main()
{
    using namespace geode;

    try
    {
        AttributeManager manager;
        manager.resize( 10 );
        OPENGEODE_EXCEPTION(
            manager.nb_elements() == 10, "[Test] Manager should have 10 elements" );
        test_constant_attribute( manager );
        test_int_variable_attribute( manager );
        test_double_sparse_attribute( manager );
        test_bool_variable_attribute( manager );
        test_delete_attribute_elements( manager );

        test_serialize_manager( manager );

        test_attribute_types( manager );
        test_number_of_attributes( manager, 4 );
        manager.delete_attribute( "bool" );
        test_number_of_attributes( manager, 3 );
        manager.clear_attributes();
        test_number_of_attributes( manager, 3 );
        manager.resize( 10 );
        OPENGEODE_EXCEPTION(
            manager.nb_elements() == 10, "[Test] Manager should have 10 elements" );
        manager.clear();
        test_number_of_attributes( manager, 0 );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
