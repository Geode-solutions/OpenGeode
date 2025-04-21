/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/constant_attribute.hpp>
#include <geode/basic/sparse_attribute.hpp>
#include <geode/basic/variable_attribute.hpp>

namespace geode
{
    template < typename Type, typename Serializer >
    void register_attribute_type( PContext &context, std::string_view name )
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
        context.registerSingleBaseBranch< Serializer, ConstantAttribute< Type >,
            ConstantAttribute< Type > >(
            absl::StrCat( "ConstantAttribute", name ).c_str() );
        context.registerSingleBaseBranch< Serializer, VariableAttribute< Type >,
            VariableAttribute< Type > >(
            absl::StrCat( "VariableAttribute", name ).c_str() );
        context.registerSingleBaseBranch< Serializer, SparseAttribute< Type >,
            SparseAttribute< Type > >(
            absl::StrCat( "SparseAttribute", name ).c_str() );
    }
} // namespace geode
