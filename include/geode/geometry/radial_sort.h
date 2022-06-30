/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <absl/container/fixed_array.h>
#include <absl/types/span.h>

#include <geode/geometry/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    ALIAS_2D_AND_3D( Point );
    ALIAS_2D_AND_3D( Segment );
} // namespace geode

namespace geode
{
    absl::FixedArray< index_t > opengeode_geometry_api radial_sort(
        const Segment3D& segment, absl::Span< const Point3D > points );
} // namespace geode
