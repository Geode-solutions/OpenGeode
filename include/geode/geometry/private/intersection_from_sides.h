/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geometry/private/predicates.h>

namespace geode
{
    namespace detail
    {
        inline Position segment_intersection_detection(
            const Side& s0, const Side& s1 )
        {
            if( s0 == Side::zero && s1 == Side::zero )
            {
                return Position::parallel;
            }
            if( s0 == Side::zero )
            {
                return Position::vertex0;
            }
            if( s1 == Side::zero )
            {
                return Position::vertex1;
            }
            if( s0 != s1 )
            {
                return Position::inside;
            }
            return Position::outside;
        }

        inline Position triangle_intersection_detection(
            const Side s0, const Side s1, const Side s2 )
        {
            if( s0 == Side::zero )
            {
                if( s1 == Side::zero )
                {
                    return Position::vertex1;
                }
                if( s2 == Side::zero )
                {
                    return Position::vertex0;
                }
                if( s1 == s2 )
                {
                    return Position::edge0;
                }
                return Position::outside;
            }
            if( s1 == Side::zero )
            {
                if( s2 == Side::zero )
                {
                    return Position::vertex2;
                }
                if( s0 == s2 )
                {
                    return Position::edge1;
                }
                return Position::outside;
            }
            if( s2 == Side::zero )
            {
                if( s0 == s1 )
                {
                    return Position::edge2;
                }
                return Position::outside;
            }
            if( s0 == s1 && s1 == s2 )
            {
                return Position::inside;
            }
            return Position::outside;
        }
    } // namespace detail
} // namespace geode
