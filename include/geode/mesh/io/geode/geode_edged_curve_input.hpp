/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/core/geode/geode_edged_curve.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/geode/geode_bitsery_mesh_input.hpp>

namespace geode
{
    BITSERY_INPUT_MESH_DIMENSION( EdgedCurve );

    // template < index_t dimension >
    // class OpenGeodeEdgedCurveInput : public EdgedCurveInput< dimension >
    // {
    // public:
    //     explicit OpenGeodeEdgedCurveInput( std::string_view filename )
    //         : EdgedCurveInput< dimension >( filename )
    //     {
    //     }

    //     [[nodiscard]] AdditionalFiles additional_files() const final
    //     {
    //         return {};
    //     }

    //     [[nodiscard]] std::unique_ptr< EdgedCurve< dimension > > read(
    //         const MeshImpl& /*impl*/ ) final
    //     {
    //         std::ifstream file{ to_string( this->filename() ),
    //             std::ifstream::binary };
    //         geode::OpenGeodeMeshException::check_exception( !file.fail(),
    //             nullptr, geode::OpenGeodeException::TYPE::data,
    //             "[Bitsery::read] Failed to open file: ",
    //             to_string( this->filename() ) );
    //         TContext context{};
    //         BitseryExtensions::register_deserialize_pcontext(
    //             std::get< 0 >( context ) );
    //         Deserializer archive{ context, file };
    //         // auto mesh = Mesh::create( impl );
    //         std::unique_ptr< EdgedCurve< dimension > > mesh{
    //             new OpenGeodeEdgedCurve< dimension >{ BITSERY::constructor }
    //         };
    //         archive.object(
    //             dynamic_cast< OpenGeodeEdgedCurve< dimension >& >( *mesh ) );
    //         const auto& adapter = archive.adapter();
    //         geode::OpenGeodeMeshException::check_exception(
    //             adapter.error() == bitsery::ReaderError::NoError
    //                 && adapter.isCompletedSuccessfully()
    //                 && std::get< 1 >( context ).isValid(),
    //             nullptr, geode::OpenGeodeException::TYPE::internal,
    //             "[Bitsery::read] Error while reading file: ",
    //             this->filename() );
    //         return mesh;
    //     }

    //     index_t object_priority() const final
    //     {
    //         return 0;
    //     }

    //     Percentage is_loadable() const final
    //     {
    //         return Percentage{ 1 };
    //     }
    // };
    // ALIAS_2D_AND_3D( OpenGeodeEdgedCurveInput );
} // namespace geode
