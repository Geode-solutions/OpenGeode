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

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    class BRep;
    class BRepBuilder;
    class Section;
    class SectionBuilder;
} // namespace geode

namespace geode
{
    void opengeode_model_api convert_surface_mesh( const Section& model,
        SectionBuilder& builder,
        const geode::Surface2D& surface,
        const geode::MeshType& mesh_type );

    void opengeode_model_api convert_surface_mesh( const BRep& model,
        BRepBuilder& builder,
        const geode::Surface3D& surface,
        const geode::MeshType& mesh_type );

    void opengeode_model_api convert_surface_meshes_into_triangulated_surfaces(
        const BRep& brep, BRepBuilder& builder );

    void opengeode_model_api convert_surface_meshes_into_triangulated_surfaces(
        const Section& section, SectionBuilder& builder );

    void opengeode_model_api convert_block_mesh( const BRep& model,
        BRepBuilder& builder,
        const Block3D& block,
        const MeshType& new_mesh_type );

    void opengeode_model_api convert_block_meshes_into_tetrahedral_solids(
        const BRep& brep, BRepBuilder& builder );

    void opengeode_model_api triangulate_surface_meshes(
        const BRep& brep, BRepBuilder& builder );

    void opengeode_model_api triangulate_surface_meshes(
        const Section& section, SectionBuilder& builder );
} // namespace geode