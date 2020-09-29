#include "application/menu.h"

#include "application/application.h"
#include "application/action_handler.h"

#include "menu/console_window.h"
#include "input.h"

#include "action/brush.h"

#include <imgui.h>

namespace ot::dedit
{
	namespace
	{
		math::plane const cube_planes[6] = {
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		auto const sqrt_half = 0.70710678118654752440084436210485f;
		math::plane const octagon_planes[] = {
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 0, 1}, 0.5},
			{{0, 0, -1}, 0.5},
			{{sqrt_half, 0, sqrt_half}, 0.5},
			{{sqrt_half, 0, -sqrt_half}, 0.5},
			{{-sqrt_half, 0, sqrt_half}, 0.5},
			{{-sqrt_half, 0, -sqrt_half}, 0.5},
		};

		math::plane const pyramid_planes[] = {
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};

		std::shared_ptr<egfx::mesh_definition const> make_mesh_def(std::span<math::plane const> planes)
		{
			auto mesh = std::make_shared<egfx::mesh_definition>();
			egfx::mesh_definition::init_from_planes(*mesh, planes);
			return mesh;
		}
	}

	template<typename Application>
	menu<Application>::menu()
		: cube(make_mesh_def(cube_planes))
		, octagonal_prism(make_mesh_def(octagon_planes))
		, square_pyramid(make_mesh_def(pyramid_planes))
	{

	}

	template<typename Application>
	bool menu<Application>::handle_keyboard_event(SDL_KeyboardEvent const& e)
	{
		if (e.keysym.sym == SDLK_o
			&& e.state == SDL_PRESSED
			&& e.repeat == 0
			&& input::keyboard::get_modifiers() == input::keyboard::mod_group::alt)
		{
			draw_console_window = !draw_console_window;
			return true;
		}

		return false;
	}

	template<typename Application>
	void menu<Application>::update()
	{
		if (draw_console_window)
		{
			console_window::draw(&draw_console_window);
		}

		if (!ImGui::BeginMainMenuBar())
			return;

		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_actions();
		map& m = app.get_current_map();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				app.quit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, acc.has_undo()))
			{
				acc.undo_latest(m);
			}

			if (ImGui::MenuItem("Redo", "CTRL+Y", false, acc.has_redo()))
			{
				acc.redo_latest(m);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Console", "Alt+O", &draw_console_window);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Brush"))
		{
			if (ImGui::BeginMenu("Spawn Primitive"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					acc.emplace_action<action::spawn_brush>(cube, m.allocate_entity_id());
				}

				if (ImGui::MenuItem("Octagonal Prism"))
				{
					acc.emplace_action<action::spawn_brush>(octagonal_prism, m.allocate_entity_id());
				}

				if (ImGui::MenuItem("Square Pyramid"))
				{
					acc.emplace_action<action::spawn_brush>(square_pyramid, m.allocate_entity_id());
				}

				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	template class menu<application>;
}
