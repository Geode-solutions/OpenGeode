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

#include <geode/basic/identifier_builder.hpp>

#include <geode/basic/identifier.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    IdentifierBuilder::IdentifierBuilder( Identifier& identifier )
        : identifier_( identifier )
    {
    }

    void IdentifierBuilder::set_id( const uuid& unique_id )
    {
        identifier_.set_id( unique_id, {} );
    }

    void IdentifierBuilder::set_name( std::string_view name )
    {
        identifier_.set_name( name, {} );
    }

    void IdentifierBuilder::copy_identifier( const Identifier& other )
    {
        identifier_.copy_identifier( other, {} );
    }

    void IdentifierBuilder::load_identifier( std::string_view directory )
    {
        identifier_.load_identifier( directory, {} );
    }

} // namespace geode
