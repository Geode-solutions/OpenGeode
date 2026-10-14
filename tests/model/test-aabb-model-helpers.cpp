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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/aabb_model_helpers.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/tests/common.hpp>

template < typename Model >
void check_meshes_aabb_trees( const Model& model,
    const geode::uuid& surface_with_polygons,
    const geode::uuid& surface_without_polygons,
    const geode::uuid& line_with_edges,
    const geode::uuid& line_without_edges )
{
    const auto surfaces_trees =
        geode::create_surface_meshes_aabb_trees( model );
    geode::OpenGeodeModelException::test(
        surfaces_trees.components_tree_.nb_bboxes() == 1,
        "[Test] Wrong number of surfaces in components tree" );
    geode::OpenGeodeModelException::test(
        surfaces_trees.uuids_.size() == 1
            && surfaces_trees.uuids_[0] == surface_with_polygons,
        "[Test] Wrong surface uuids" );
    geode::OpenGeodeModelException::test(
        surfaces_trees.mesh_tree_ids_.at( surface_with_polygons ) == 0
            && !surfaces_trees.mesh_tree_ids_.contains(
                surface_without_polygons ),
        "[Test] Wrong surface mesh tree ids" );
    geode::OpenGeodeModelException::test(
        surfaces_trees.mesh_trees_[0].nb_bboxes() == 1,
        "[Test] Wrong surface mesh tree" );

    const auto lines_trees = geode::create_line_meshes_aabb_trees( model );
    geode::OpenGeodeModelException::test(
        lines_trees.components_tree_.nb_bboxes() == 1,
        "[Test] Wrong number of lines in components tree" );
    geode::OpenGeodeModelException::test(
        lines_trees.uuids_.size() == 1
            && lines_trees.uuids_[0] == line_with_edges,
        "[Test] Wrong line uuids" );
    geode::OpenGeodeModelException::test(
        lines_trees.mesh_tree_ids_.at( line_with_edges ) == 0
            && !lines_trees.mesh_tree_ids_.contains( line_without_edges ),
        "[Test] Wrong line mesh tree ids" );

    const auto [surfaces_tree, surfaces_mapping] =
        geode::create_surfaces_aabb_tree( model );
    geode::OpenGeodeModelException::test(
        surfaces_tree.nb_bboxes() == 2 && surfaces_mapping.size() == 2,
        "[Test] Wrong number of surfaces in surfaces bbox tree" );
}

template < typename Model, typename Builder, geode::index_t dimension >
void test_meshes_without_elements()
{
    Model model;
    Builder builder{ model };
    const auto& surface_with_polygons = builder.add_surface();
    const auto& surface_without_polygons = builder.add_surface();
    const auto& line_with_edges = builder.add_line();
    const auto& line_without_edges = builder.add_line();

    geode::Point< dimension > origin;
    geode::Point< dimension > point_x;
    point_x.set_value( 0, 1 );
    geode::Point< dimension > point_y;
    point_y.set_value( 1, 1 );

    auto surface_builder =
        builder.surface_mesh_builder( model.surface( surface_with_polygons ) );
    surface_builder->create_point( origin );
    surface_builder->create_point( point_x );
    surface_builder->create_point( point_y );
    surface_builder->create_polygon( { 0, 1, 2 } );

    auto empty_surface_builder = builder.surface_mesh_builder(
        model.surface( surface_without_polygons ) );
    empty_surface_builder->create_point( origin );
    empty_surface_builder->create_point( point_x );

    auto line_builder =
        builder.line_mesh_builder( model.line( line_with_edges ) );
    line_builder->create_point( origin );
    line_builder->create_point( point_x );
    line_builder->create_edge( 0, 1 );

    auto empty_line_builder =
        builder.line_mesh_builder( model.line( line_without_edges ) );
    empty_line_builder->create_point( point_y );

    check_meshes_aabb_trees( model, surface_with_polygons,
        surface_without_polygons, line_with_edges, line_without_edges );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_meshes_without_elements< geode::Section, geode::SectionBuilder, 2 >();
    test_meshes_without_elements< geode::BRep, geode::BRepBuilder, 3 >();
}

OPENGEODE_TEST( "aabb-model-helpers" )
