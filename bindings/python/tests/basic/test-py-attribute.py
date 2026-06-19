# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic


def test_constant_attribute(manager):
    
    constant_attribute_id = manager.create_attribute_constant_bool(
        "bool", True)
    constant_attribute = manager.find_attribute_constant_bool(constant_attribute_id)
    attribute = manager.read_attribute_bool(constant_attribute_id)
    if not attribute.value(0):
        raise ValueError("[Test] Should be equal to True")

    constant_attribute.set_value(False)
    if attribute.value(12):
        raise ValueError("[Test] Should be equal to False")
    return constant_attribute_id


def test_int_variable_attribute(manager):
    variable_attribute_id = manager.create_attribute_variable_int(
        "int", 12)
    variable_attribute = manager.find_attribute_variable_int(variable_attribute_id)
    variable_attribute.set_value(3, 3)
    if not variable_attribute.is_genericable():
        raise ValueError("[Test] Should be genericable")
    
    manager.set_attribute_properties(variable_attribute_id,basic.AttributeProperties(True,True))
    if not variable_attribute.properties().assignable or not variable_attribute.properties().interpolable :
        raise ValueError("[Test] Should be assignable and interpolable")

    read_attribute = manager.read_attribute_int(variable_attribute_id)
    if variable_attribute.value(3) != 3:
        raise ValueError("[Test] Should be equal to 3")
    if variable_attribute.value(6) != 12:
        raise ValueError("[Test] Should be equal to 12")
    if read_attribute.value(3) != 3:
        raise ValueError("[Test] Should be equal to 3")
    if read_attribute.value(6) != 12:
        raise ValueError("[Test] Should be equal to 12")

    variable_attribute.set_value(3, 5)
    if variable_attribute.value(3) != 5:
        raise ValueError("[Test] Should be equal to 5")
    if read_attribute.value(3) != 5:
        raise ValueError("[Test] Should be equal to 5")


def test_double_sparse_attribute(manager):
    sparse_attribute_id = manager.create_attribute_sparse_double(
        "double", 12)
    attribute = manager.find_attribute_sparse_double(sparse_attribute_id)
    attribute.set_value(3, 3)
    attribute.set_value(7, 7)

    read_attribute = manager.read_attribute_double(sparse_attribute_id)
    if attribute.value(3) != 3:
        raise ValueError("[Test] Should be equal to 3")
    if attribute.value(6) != 12:
        raise ValueError("[Test] Should be equal to 12")
    if attribute.value(7) != 7:
        raise ValueError("[Test] Should be equal to 7")
    if read_attribute.value(3) != 3:
        raise ValueError("[Test] Should be equal to 3")
    if read_attribute.value(6) != 12:
        raise ValueError("[Test] Should be equal to 12")
    if read_attribute.value(7) != 7:
        raise ValueError("[Test] Should be equal to 7")

    attribute.set_value(3, 5)
    if attribute.value(3) != 5:
        raise ValueError("[Test] Should be equal to 5")
    if read_attribute.value(3) != 5:
        raise ValueError("[Test] Should be equal to 5")
    return sparse_attribute_id


def test_number_of_attributes(manager, nb):
    if len(manager.attribute_ids()) != nb:
        raise ValueError(
            "[Test] Not the correct number of attributes in the manager")


def test_delete_attribute_elements(manager):
    to_delete = [False] * manager.nb_elements()
    to_delete[3] = True
    to_delete[5] = True
    manager.delete_elements(to_delete)
    if manager.nb_elements() != len(to_delete) - 2:
        raise ValueError(
            "[Test] Two attribute elements should have been removed")


def test_sparse_attribute_after_element_deletion(manager, double_attribute_id):
    sparse_attribute = manager.read_attribute_double(double_attribute_id)
    if sparse_attribute.value(0) != 12:
        raise ValueError("Element 0 of sparse attribute should be 12 ")
    if sparse_attribute.value(5) != 7:
        raise ValueError("Element 5 of sparse attribute should be 7 ")
    if sparse_attribute.value(7) != 12:
        raise ValueError("Element 7 of sparse attribute should be 12 ")


if __name__ == '__main__':
    manager = basic.AttributeManager()
    manager.resize(10)
    if manager.nb_elements() != 10:
        raise ValueError("[Test] Manager should have 10 elements")
    bool_attribute_id = test_constant_attribute(manager)
    test_int_variable_attribute(manager)
    test_double_sparse_attribute(manager)
    double_attribute_id = test_double_sparse_attribute(manager)
    test_delete_attribute_elements(manager)
    test_sparse_attribute_after_element_deletion(manager,double_attribute_id)
    test_number_of_attributes(manager, 4)
    manager.delete_attribute(bool_attribute_id)
    test_number_of_attributes(manager, 3)
    manager.clear_attributes()
    test_number_of_attributes(manager, 3)
    manager.resize(10)
    if manager.nb_elements() != 10:
        raise ValueError("[Test] Manager should have 10 elements")
    manager.clear()
    test_number_of_attributes(manager, 0)
