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

#include "../common.hpp"

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/attribute_time_series.hpp>
#include <geode/basic/constant_attribute.hpp>
#include <geode/basic/sparse_attribute.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <typeinfo>

namespace geode
{
    template < template < typename > class Attribute, typename type >
    void python_attribute_creation_from_default_value(
        pybind11::class_< AttributeManager >& manager, const std::string& name )
    {
        manager.def(
            name.c_str(),
            []( AttributeManager& attribute_manager,
                std::string_view attribute_name, type default_value,
                AttributeProperties properties ) {
                AttributeValues< type > values;
                values.default_value = std::move( default_value );
                return attribute_manager.create_attribute< Attribute, type >(
                    attribute_name, std::move( values ), properties );
            },
            pybind11::arg( "attribute_name" ), pybind11::arg( "default_value" ),
            pybind11::arg_v(
                "properties", AttributeProperties{}, "AttributeProperties()" ),
            "Create an attribute from its name and default value and return "
            "its uuid" );
    }

    template < template < typename > class Attribute, typename type >
    void python_attribute_kind( pybind11::class_< AttributeManager >& manager,
        const std::string& kind,
        const std::string& suffix )
    {
        const auto create_suffix =
            absl::StrCat( "create_attribute_", kind, "_", suffix );
        manager.def( create_suffix.c_str(),
            static_cast< geode::uuid ( AttributeManager::* )( std::string_view,
                AttributeValues< type >, AttributeProperties ) >(
                &AttributeManager::create_attribute< Attribute, type > ),
            pybind11::arg( "attribute_name" ),
            pybind11::arg( "default_values" ), pybind11::arg( "properties" ),
            "Create an attribute from its name, default values and "
            "properties and return its uuid" );
        manager.def( create_suffix.c_str(),
            static_cast< void ( AttributeManager::* )( std::string_view,
                const uuid&, AttributeValues< type >, AttributeProperties ) >(
                &AttributeManager::create_attribute< Attribute, type > ),
            pybind11::arg( "attribute_name" ), pybind11::arg( "attribute_id" ),
            pybind11::arg( "default_values" ), pybind11::arg( "properties" ),
            "Create an attribute with the given uuid from its name, default "
            "values and properties" );
        python_attribute_creation_from_default_value< Attribute, type >(
            manager, create_suffix );
        const auto find_suffix =
            absl::StrCat( "find_attribute_", kind, "_", suffix );
        manager.def( find_suffix.c_str(),
            static_cast< std::shared_ptr< Attribute< type > > (
                AttributeManager::* )( const geode::uuid& ) >(
                &AttributeManager::find_attribute< Attribute, type > ),
            pybind11::arg( "attribute_id" ),
            "Return the attribute with the given id. Raise an error if it "
            "does not exist or is not of this kind and type" );
        const auto time_step_suffix =
            absl::StrCat( "create_time_step_attribute_", kind, "_", suffix );
        manager.def( time_step_suffix.c_str(),
            &AttributeManager::create_time_step_attribute< Attribute, type >,
            pybind11::arg( "attribute_name" ), pybind11::arg( "time" ),
            pybind11::arg( "default_values" ), pybind11::arg( "properties" ),
            "Create an attribute which is the step at the given time of the "
            "time series called attribute_name and return its uuid" );
    }

    template < typename type >
    void python_attribute_class( pybind11::class_< AttributeManager >& manager,
        const std::string& suffix )
    {
        const auto read_suffix =
            absl::StrCat( "find_read_only_attribute_", suffix );
        manager.def( read_suffix.c_str(),
            &AttributeManager::find_read_only_attribute< type >,
            pybind11::arg( "attribute_id" ),
            "Return the attribute with the given id as a read-only "
            "attribute. Raise an error if it does not exist or has another "
            "type" );
        python_attribute_kind< ConstantAttribute, type >(
            manager, "constant", suffix );
        python_attribute_kind< VariableAttribute, type >(
            manager, "variable", suffix );
        python_attribute_kind< SparseAttribute, type >(
            manager, "sparse", suffix );
    }

    template < typename type >
    void python_attribute_time_series_class(
        pybind11::module& module, const std::string& typestr )
    {
        const auto name = absl::StrCat( "AttributeTimeSeries", typestr );
        pybind11::class_< AttributeTimeSeries< type > >( module, name.c_str(),
            "Values of the attributes of a time series, ordered by time" )
            .def( pybind11::init< const AttributeManager&, std::string_view >(),
                pybind11::arg( "manager" ), pybind11::arg( "name" ),
                "Gather the steps of the time series called name in manager" )
            .def( "nb_time_steps", &AttributeTimeSeries< type >::nb_time_steps,
                "Return the number of time steps" )
            .def( "time", &AttributeTimeSeries< type >::time,
                pybind11::arg( "step" ),
                "Return the time of the step (steps are ordered by time)" )
            .def( "step_attribute",
                &AttributeTimeSeries< type >::step_attribute,
                pybind11::return_value_policy::reference_internal,
                pybind11::arg( "step" ), "Return the attribute of the step" )
            .def( "value", &AttributeTimeSeries< type >::value,
                pybind11::arg( "step" ), pybind11::arg( "element" ),
                "Return the value of the element at the step" )
            .def( "element_values",
                &AttributeTimeSeries< type >::element_values,
                pybind11::arg( "element" ),
                "Return the values of the element at all the steps" );
    }

    void define_attribute_manager( pybind11::module& module )
    {
        pybind11::class_< AttributeTimeStep >( module, "AttributeTimeStep",
            "Step of a time series: a time and the attribute holding its "
            "values" )
            .def_readonly(
                "time", &AttributeTimeStep::time, "Time of the step" )
            .def_readonly( "attribute_id", &AttributeTimeStep::attribute_id,
                "Uuid of the attribute holding the values of the step" );
        pybind11::class_< AttributeManager > manager( module,
            "AttributeManager",
            "Container of the attributes of a set of elements (vertices, "
            "polygons...). All the attributes have one value per element" );
        manager
            .def( pybind11::init<>(),
                "Build a manager without attributes nor elements" )
            .def(
                "find_generic_attribute",
                // std::optional makes the generated stub return
                // `AttributeBase | None`: a null shared_ptr becomes None
                []( const AttributeManager& attribute_manager,
                    const uuid& attribute_id )
                    -> std::optional< std::shared_ptr< AttributeBase > > {
                    if( auto attribute =
                            attribute_manager.find_generic_attribute(
                                attribute_id ) )
                    {
                        return attribute;
                    }
                    return std::nullopt;
                },
                pybind11::arg( "attribute_id" ),
                "Return the attribute with the given id whatever its type, "
                "or None if there is none" )
            .def( "attribute_ids", &AttributeManager::attribute_ids,
                "Return the ids of all the attributes" )
            .def( "attribute_type", &AttributeManager::attribute_type,
                pybind11::arg( "attribute_id" ),
                "Return the C++ type id name of the values of the attribute "
                "(for example \"i\" for int), or \"undefined\" if there is no "
                "such attribute" )
            .def( "attribute_exists", &AttributeManager::attribute_exists,
                pybind11::arg( "attribute_id" ),
                "Return True if an attribute has the given id" )
            .def( "nb_elements", &AttributeManager::nb_elements,
                "Return the number of elements of the attributes" )
            .def( "reserve", &AttributeManager::reserve,
                pybind11::arg( "capacity" ),
                "Reserve memory for capacity elements" )
            .def( "resize", &AttributeManager::resize, pybind11::arg( "size" ),
                "Set the number of elements of all the attributes" )
            .def( "clear", &AttributeManager::clear,
                "Delete all the attributes and set the number of elements to "
                "0" )
            .def( "clear_attributes", &AttributeManager::clear_attributes,
                "Remove all the elements of all the attributes, which are "
                "kept, and set the number of elements to 0" )
            .def( "delete_attribute", &AttributeManager::delete_attribute,
                pybind11::arg( "attribute_id" ),
                "Delete the attribute with the given id, if it exists" )
            .def( "copy_attribute", &AttributeManager::copy_attribute,
                pybind11::arg( "attribute_id" ),
                pybind11::arg( "new_attribute_id" ),
                "Create the attribute new_attribute_id as a copy of the "
                "attribute attribute_id. Raise an error if attribute_id does "
                "not exist, new_attribute_id already exists or attribute_id is "
                "a step of a time series" )
            .def( "set_attribute_properties",
                &AttributeManager::set_attribute_properties,
                pybind11::arg( "attribute_id" ),
                pybind11::arg( "new_properties" ),
                "Replace all the properties of the attribute, time included. "
                "Raise an error if the attribute does not exist or if the time "
                "is already used by another step of the same time series" )
            .def( "delete_elements", &AttributeManager::delete_elements,
                pybind11::arg( "to_delete" ),
                "Delete the elements e for which to_delete[e] is True and "
                "shift the following ones. to_delete must have nb_elements() "
                "items" )
            .def( "attribute_ids_matching_name",
                &AttributeManager::attribute_ids_matching_name,
                pybind11::arg( "name" ),
                "Return the ids of the attributes called name, or None if "
                "there is none" )
            .def( "time_steps", &AttributeManager::time_steps,
                pybind11::arg( "attribute_name" ),
                "Return the steps (time and attribute id) of the time series "
                "called attribute_name, ordered by increasing time" )
            .def( "time_series_names", &AttributeManager::time_series_names,
                "Return the names of all the time series" );
        python_attribute_class< bool >( manager, "bool" );
        python_attribute_class< int >( manager, "int" );
        python_attribute_class< unsigned int >( manager, "uint" );
        python_attribute_class< float >( manager, "float" );
        python_attribute_class< double >( manager, "double" );
        python_attribute_class< std::array< double, 2 > >(
            manager, "arraydouble2" );
        python_attribute_class< std::array< double, 3 > >(
            manager, "arraydouble3" );
        python_attribute_time_series_class< bool >( module, "Bool" );
        python_attribute_time_series_class< int >( module, "Int" );
        python_attribute_time_series_class< unsigned int >( module, "UInt" );
        python_attribute_time_series_class< float >( module, "Float" );
        python_attribute_time_series_class< double >( module, "Double" );
        python_attribute_time_series_class< std::array< double, 2 > >(
            module, "ArrayDouble2" );
        python_attribute_time_series_class< std::array< double, 3 > >(
            module, "ArrayDouble3" );
    }
} // namespace geode
