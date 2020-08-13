#pragma once

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	class face_context : public context
	{
		map const* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;

		size_t selected_brush;
		graphics::face::id selected_face;

	public:
		face_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face);

		virtual void render(graphics::node::manual& m) override;
		virtual void get_debug_string(std::string& s) const override;
	};
}