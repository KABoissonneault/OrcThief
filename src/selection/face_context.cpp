#include "selection/face_context.h"

#include "datablock.h"

namespace ot::selection
{
	face_context::face_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face)
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
	{

	}

	void face_context::render(graphics::node::manual& m)
	{
		brush const& b = current_map->get_brushes()[selected_brush];
		math::transformation const t = b.get_world_transform(math::transformation::identity());

		m.add_face(datablock::overlay_unlit_transparent_heavy, graphics::face::cref{ b.mesh_def, selected_face }, t);
	}

	void face_context::get_debug_string(std::string& s) const
	{
		s += "Selected brush: " + std::to_string(selected_brush) + "\n";
		s += "Selected face: " + std::to_string(static_cast<size_t>(selected_face)) + "\n";
	}
}
