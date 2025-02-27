/*
 * Copyright (c) 2019 - 2025 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/pimpl.hpp>

#include <geode/geometry/common.hpp>
#include <geode/geometry/frame_transform.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Frame );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class NormalFrameTransform : public FrameTransform< dimension >
    {
    public:
        NormalFrameTransform(
            const Frame< dimension >& from, const Frame< dimension >& to );
        ~NormalFrameTransform();

        [[nodiscard]] Frame< dimension > apply(
            const Frame< dimension >& frame ) const final;

        [[nodiscard]] Vector< dimension > apply(
            const Vector< dimension >& vector ) const final;

        [[nodiscard]] Point< dimension > apply(
            const Point< dimension >& point ) const final;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( NormalFrameTransform );
} // namespace geode