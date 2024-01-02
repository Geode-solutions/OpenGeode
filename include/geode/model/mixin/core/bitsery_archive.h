/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/bitsery_archive.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Component );
    FORWARD_DECLARATION_DIMENSION_CLASS( Corner );
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( ModelBoundary );
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    ALIAS_2D_AND_3D( Component );
    ALIAS_2D_AND_3D( Corner );
    ALIAS_2D_AND_3D( Line );
    ALIAS_2D_AND_3D( Surface );
    ALIAS_2D_AND_3D( ModelBoundary );
    ALIAS_3D( Block );
} // namespace geode

namespace geode
{
    /*!
     * Register all the information needed by Bitsery to serialize the objects
     * in the model library.
     * @param[in] context The context where to register this information.
     * @warning The context can be used only once per archive.
     */
    void opengeode_model_api register_model_serialize_pcontext(
        PContext& context );

    /*!
     * Register all the information needed by Bitsery to deserialize the objects
     * in the model library.
     * @param[in] context The context where to register this information.
     * @warning The context can be used only once per archive.
     */
    void opengeode_model_api register_model_deserialize_pcontext(
        PContext& context );
} // namespace geode

namespace bitsery
{
    namespace ext
    {
        BITSERY_CLASS_NAME( geode::Component2D, "Component2D" );
        BITSERY_CLASS_NAME( geode::Corner2D, "Corner2D" );
        BITSERY_CLASS_NAME( geode::Line2D, "Line2D" );
        BITSERY_CLASS_NAME( geode::Surface2D, "Surface2D" );
        BITSERY_CLASS_NAME( geode::ModelBoundary2D, "ModelBoundary2D" );
        BITSERY_CLASS_NAME( geode::Component3D, "Component3D" );
        BITSERY_CLASS_NAME( geode::Corner3D, "Corner3D" );
        BITSERY_CLASS_NAME( geode::Line3D, "Line3D" );
        BITSERY_CLASS_NAME( geode::Surface3D, "Surface3D" );
        BITSERY_CLASS_NAME( geode::ModelBoundary3D, "ModelBoundary3D" );
        BITSERY_CLASS_NAME( geode::Block3D, "Block3D" );
    } // namespace ext
} // namespace bitsery