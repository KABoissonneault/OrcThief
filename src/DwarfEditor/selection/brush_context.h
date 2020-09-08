#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/scene.h"
#include "egfx/window.h"

namespace ot::dedit::selection
{
	class brush_context : public composite_context
	{
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;

		size_t selected_brush;
		egfx::face::id hovered_face = egfx::face::id::none;

		void select(size_t brush_idx);
		void select_next();
		void select_previous();

		void detect_hovered_face();

	public:
		brush_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, size_t selected_brush) noexcept;

		virtual void update() override;
		virtual void render(egfx::node::manual& m) override;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse, action::accumulator& acc) override;

		virtual bool is_selected(entity_id id) const noexcept override { return current_map->get_brushes()[selected_brush].get_id() == id; }

		virtual void get_debug_string(std::string& s) const override;
	};
}