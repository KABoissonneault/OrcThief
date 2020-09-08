#include "application.h"

#include "selection/base_context.h"
#include "action/brush.h"

#include "egfx/scene.h"
#include "egfx/camera.h"
#include "egfx/node/mesh.h"
#include "egfx/node/light.h"

#include "Ogre/ConfigOptionMap.h"
#include "Ogre/RenderSystem.h"

#include "SDL2/window.h"
#include "SDL2/macro.h"
#include <SDL_video.h>
#include <SDL_events.h>

#include <imgui_impl_sdl.h>

#include <iterator>

namespace ot::dedit
{
	namespace
	{
		bool keyboard_modifier_is(int mod)
		{
			constexpr auto k_modifiers = KMOD_CTRL | KMOD_SHIFT | KMOD_ALT;
			auto const modifier_state = SDL_GetModState() & k_modifiers;
			return (modifier_state & mod) && !(modifier_state & ~mod);
		}

		math::plane const cube_planes[6] = {
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		auto const sqrt_half = 0.70710678118654752440084436210485;
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

		size_t get_number_threads()
		{
			return Ogre::PlatformInformation::getNumLogicalCores();
		}

		void push_window_event(SDL_Event const& e, std::vector<egfx::window_event>& window_events)
		{
			using egfx::window_event;
			using egfx::window_id;
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_MOVED:
					window_events.push_back({ window_id(e.window.windowID), window_event::moved{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_RESIZED:
					window_events.push_back({ window_id(e.window.windowID), window_event::resized{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_gained{} });
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_lost{} });
					break;
				}
				break;
			}
		}
	}

	application::application(sdl::unique_window window, egfx::module& graphics)
		: main_window(std::move(window))
		, graphics(graphics)
	{

	}

	bool application::initialize()
	{
		main_scene = graphics.create_scene("OrcThiefWorkspace", get_number_threads() - 1);

		auto const& render_window = graphics.get_window(egfx::window_id{ SDL_GetWindowID(main_window.get()) });
		selection_context.reset(new selection::base_context(current_map, main_scene, render_window));

		return true;
	}
	 
	void application::setup_default_scene()
	{
		current_map.add_brush(make_brush(cube_planes, "Cube", { 2.5, 0.0, 0.0 }));
		current_map.add_brush(make_brush(octagon_planes, "Octagon", { 0, 0.0, 0.0 }));
		current_map.add_brush(make_brush(pyramid_planes, "Pyramid", { -2.5, 0.0, 0.0 }));

		auto& camera = main_scene.get_camera();
		set_position(camera, { 0.0, 2.0, -5.5 });
		look_at(camera, { 0.0, 0.0, 0.0 });

		light = egfx::node::create_directional_light(main_scene.get_root_node());
		light.set_position({ 10.0, 10.0, -10.0 });
		light.set_direction(normalized(math::vector3d{ -1.0, -1.0, 1.0 }));

		debug_surface = egfx::overlay::create_surface("DebugSurface");
		debug_text.emplace(debug_surface, "DebugText");
		debug_text->set_font("DebugFont");
		debug_text->set_height(0.025);

		debug_surface.show();

		selection_render = egfx::node::create_manual(main_scene.get_root_node());
	}

	brush application::make_brush(std::span<math::plane const> planes, std::string const& name, math::point3d position)
	{
		auto mesh = std::make_shared<egfx::mesh_definition const>(egfx::mesh_definition::make_from_planes(planes));
		auto node = egfx::node::create_mesh(main_scene.get_root_node(), name, *mesh);
		node.set_position(position);
		return { std::move(mesh), std::move(node) };
	}

	void application::run()
	{
		using namespace ot::math::literals;
		auto last_frame = std::chrono::steady_clock::now();
		auto const frame_time = 0.02_s;
		auto frame_accumulator = frame_time;

		while (!wants_quit) 
		{
			// Events
			handle_events();

			// Update
			while (frame_accumulator >= frame_time)
			{
				update(frame_time);
				frame_accumulator -= frame_time;
			}

			// Rendering
			if (!render())
			{
				break;
			}

			auto const current_frame = std::chrono::steady_clock::now();
			frame_accumulator += std::min(1._s, std::chrono::duration_cast<ot::math::seconds>(current_frame - last_frame));
			last_frame = current_frame;
		}
	}

	void application::handle_events()
	{
		SDL_PumpEvents();

		ImGuiIO& imgui_io = ImGui::GetIO();
		std::vector<egfx::window_event> window_events;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_KEYUP:
			case SDL_KEYDOWN:
			{
				SDL_KeyboardEvent const& key = e.key;
				auto const k_modifiers = KMOD_CTRL | KMOD_ALT | KMOD_SHIFT;

				if (imgui_io.WantCaptureKeyboard)
				{
					ImGui_ImplSDL2_ProcessEvent(&e);

					if (key.state == SDL_PRESSED && key.keysym.scancode == SDL_SCANCODE_F4 && keyboard_modifier_is(KMOD_LALT))
						wants_quit = true;

					break;
				}


				if (key.keysym.scancode == SDL_SCANCODE_Z 
					&& keyboard_modifier_is(KMOD_CTRL)
					&& key.state == SDL_PRESSED
					&& key.repeat == 0)
				{
					selection_actions.undo_latest(current_map);
					break;
				}

				if (selection_context != nullptr && selection_context->handle_keyboard_event(key, selection_actions))
				{
					break;
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				if (imgui_io.WantCaptureMouse)
				{
					ImGui_ImplSDL2_ProcessEvent(&e);
					continue;
				}

				if (selection_context != nullptr && selection_context->handle_mouse_button_event(e.button, selection_actions))
				{
					continue;
				}

				break;

			case SDL_MOUSEMOTION:
				if (imgui_io.WantCaptureMouse)
				{
					ImGui_ImplSDL2_ProcessEvent(&e);
					continue;
				}

				if (selection_context != nullptr && selection_context->handle_mouse_motion_event(e.motion, selection_actions))
				{
					continue;
				}

				break;

			case SDL_WINDOWEVENT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				push_window_event(e, window_events);

				if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(main_window.get()))
				{
					wants_quit = true;
				}

				break;

			case SDL_QUIT:
				wants_quit = true;
				break;
			}
		}

		graphics.on_window_events(window_events);

		selection_actions.apply_actions(current_map);
	}

	void application::update(math::seconds dt)
	{
		for (auto& brush : current_map.get_brushes())
		{
			if (!selection_context->is_selected(brush.get_id()))
			{
				brush.node.rotate_around(math::vector3d{ 0.0, 1.0, 0.0 }, dt.count());
			}
		}		
	}

	bool application::render()
	{
		// Start frame
		graphics.start_frame();
		ImGui_ImplSDL2_NewFrame(main_window.get());
		ImGui::NewFrame();
		std::string s;
		selection_context->get_debug_string(s);
		debug_text->set_text(s);

		selection_context->update();
		selection_render.clear();
		selection_context->render(selection_render);

		// Render
		ImGui::Render();		

		// End frame
		if (!graphics.render())
		{
			return false;
		}

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui::EndFrame();

		return true;
	}

	void application::actions::push_brush_action(uptr<action::brush_base, fwd_delete<action::brush_base>> action)
	{ 
		current_brush.push_back(std::move(action)); 
	}

	void application::actions::apply_actions(map& current_map)
	{
		if (current_brush.empty())
			return;

		for (auto& action : current_brush)
		{
			action->apply(current_map);
		}

		previous_brush.insert(previous_brush.end(), std::make_move_iterator(current_brush.begin()), std::make_move_iterator(current_brush.end()));
		current_brush.clear();		
	}
	
	void application::actions::undo_latest(map& current_map)
	{
		if (previous_brush.empty())
			return;

		previous_brush.back()->undo(current_map);
		previous_brush.pop_back();
	}
}