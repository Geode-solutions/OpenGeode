/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/basic/common.h>
#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

namespace geode
{
    class IdentifierBuilder;
    struct uuid;
} // namespace geode

namespace geode
{
    class opengeode_basic_api Identifier
    {
        PASSKEY( IdentifierBuilder, IdentifierKey );
        friend class bitsery::Access;

    public:
        static constexpr auto DEFAULT_NAME = "unknown";

        Identifier( Identifier&& );
        ~Identifier();

        const uuid& id() const;

        absl::string_view name() const;

        void save_identifier( absl::string_view directory ) const;

        void set_id( uuid id, IdentifierKey );

        void set_name( absl::string_view name, IdentifierKey );

        void load_identifier( absl::string_view directory, IdentifierKey );

    protected:
        Identifier();

        void set_id( uuid id );

        void set_name( absl::string_view name );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode