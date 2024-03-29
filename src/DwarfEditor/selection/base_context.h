#pragma once

#include "selection/context.h"
#include "map.h"

#include "application/basic_mesh_repo.h"

#include "egfx/scene.h"
#include "egfx/window.h"

#include <optional>

namespace ot::dedit::selection
{
	// The context at the root of the application. Handles object selection and global events
	class base_context : public composite_context
	{
		basic_mesh_repo const* mesh_repo;
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;
		std::optional<entity_id> selected_entity;
		
		void do_selection();

		void select_entity(entity_id brush);
		void deselect_entity();

		void hierarchy_window(action::accumulator& acc);

	public:
		base_context(basic_mesh_repo const& mesh_repo, map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window) noexcept
			: mesh_repo(&mesh_repo)
			, current_map(&current_map)
			, current_scene(&current_scene)
			, main_window(&main_window)
		{

		}

		virtual void start_frame() override;
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
		virtual void update(action::accumulator& acc, input::frame_input& input) override;
	};
}
