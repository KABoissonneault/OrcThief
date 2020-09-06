#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/scene.h"
#include "egfx/window.h"

namespace ot::selection
{
	class face_context : public composite_context
	{
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;

		size_t selected_brush;
		egfx::face::id selected_face;
		egfx::half_edge::id hovered_edge = egfx::half_edge::id::none;

		void detect_hovered_edge();

	public:
		face_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, size_t selected_brush, egfx::face::id selected_face);

		virtual void update() override;
		virtual void render(egfx::node::manual& m) override;

		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& e, action::accumulator& acc) override;

		virtual void get_debug_string(std::string& s) const override;
	};
}
