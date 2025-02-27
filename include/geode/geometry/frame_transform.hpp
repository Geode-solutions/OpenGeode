/*
 * Copyright (c) 2019 - 2025 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/pimpl.hpp>

#include <geode/geometry/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Frame );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class FrameTransform
    {
    public:
        FrameTransform(
            const Frame< dimension >& from, const Frame< dimension >& to );
        virtual ~FrameTransform();

        virtual [[nodiscard]] local_index_t direction(
            local_index_t index ) const final;

        virtual [[nodiscard]] signed_index_t orientation(
            local_index_t index ) const final;

        virtual [[nodiscard]] Frame< dimension > apply(
            const Frame< dimension >& frame ) const;

        virtual [[nodiscard]] Vector< dimension > apply(
            const Vector< dimension >& vector ) const;

        virtual [[nodiscard]] Point< dimension > apply(
            const Point< dimension >& point ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( FrameTransform );
} // namespace geode