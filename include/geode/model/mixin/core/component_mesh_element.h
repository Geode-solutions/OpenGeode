/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    /*!
     * Identify an element in a model component
     */
    struct ComponentMeshElement
    {
        ComponentMeshElement(
            ComponentID component_id_in, index_t element_id_in )
            : component_id( std::move( component_id_in ) ),
              element_id( element_id_in )
        {
        }

        bool operator==( const ComponentMeshElement& other ) const
        {
            return component_id == other.component_id
                   && element_id == other.element_id;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, ComponentMeshElement >{},
                []( Archive& a, ComponentMeshElement& cme ) {
                    a.object( cme.component_id );
                    a.value4b( cme.element_id );
                } );
        }

        ComponentID component_id;
        index_t element_id{ NO_ID };
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::ComponentMeshElement >
    {
    public:
        size_t operator()( const geode::ComponentMeshElement& cme ) const
        {
            return absl::Hash< geode::uuid >()( cme.component_id.id() )
                   ^ absl::Hash< geode::index_t >()( cme.element_id );
        }
    };
} // namespace std