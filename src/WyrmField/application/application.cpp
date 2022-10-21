#include "application/application.h"
#include "application/serialization.h"
#include "config.h"
#include "main_imgui.h"
#include "debug/debug_menu.h"

#include "math/ops.h"

#include "egfx/window_type.h"
#include "egfx/module.h"
#include "egfx/object/camera.h"

#include <SDL_events.h>
#include <imgui_impl_sdl.h>
#include <im3d.h>
#include <imgui.h>

#include <filesystem>
#include <fstream>

namespace ot::wf
{
	namespace
	{
		application* instance;

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

		Im3d::Vec3 get_cursor_ray(egfx::object::camera_cref const camera, math::transform_matrix const& camera_projection)
		{
			Im3d::AppData& ad = Im3d::GetAppData();

			int cursor_x, cursor_y;
			SDL_GetMouseState(&cursor_x, &cursor_y);

			float const norm_x = (static_cast<float>(cursor_x) / ad.m_viewportSize.x) * 2.0f - 1.0f;
			float const norm_y = -((static_cast<float>(cursor_y) / ad.m_viewportSize.y) * 2.0f - 1.0f);

			math::transform_matrix const camera_transform = camera.get_transformation();

			math::vector3f const ray_direction
			{
				norm_x / camera_projection[0][0]
				, norm_y / camera_projection[1][1]
				, -1.0f
			};

			return normalized(transform(ray_direction, camera_transform));
		}

		void im3d_preupdate(egfx::object::camera_cref const camera)
		{
			Im3d::AppData& ad = Im3d::GetAppData();
			math::transform_matrix const camera_projection = camera.get_projection();

			ad.m_cursorRayOrigin = ad.m_viewOrigin;
			ad.m_cursorRayDirection = get_cursor_ray(camera, camera_projection);

			Uint32 const mouse_state = SDL_GetMouseState(nullptr, nullptr);
			ad.m_keyDown[Im3d::Mouse_Left] = (mouse_state & SDL_BUTTON_LMASK) == SDL_BUTTON_LEFT;
		}

		std::filesystem::path get_game_data_path(config const& program_config)
		{
			return std::filesystem::path(program_config.get_core().get_resource_root()) / "Game";
		}

		char const* const enemy_template_filename = "enemy_templates.json";

		void generate_player_characters(std::vector<m3::player_character_data>& player_characters)
		{
			m3::player_character_data& pc1 = player_characters.emplace_back();
			pc1.name = "Karsa";
			pc1.attributes.strength = 80;
			pc1.attributes.constitution = 70;
			pc1.attributes.agility = 60;
			pc1.attributes.cleverness = 20;
			pc1.attributes.hardiness = 70;
			pc1.attributes.focus = 30;
			pc1.attributes.charisma = 30;
			pc1.attributes.will = 60;
			pc1.attributes.wisdom = 30;
			pc1.vitals = m3::generate_initial_vitals(pc1.attributes);
			pc1.skills.hunt = 5;
			pc1.skills.brawl = 3;
			pc1.skills.mountaineering = 5;

			m3::player_character_data& pc2 = player_characters.emplace_back();
			pc2.name = "Caladan";
			pc2.attributes.strength = 50;
			pc2.attributes.constitution = 80;
			pc2.attributes.agility = 70;
			pc2.attributes.cleverness = 30;
			pc2.attributes.hardiness = 40;
			pc2.attributes.focus = 50;
			pc2.attributes.charisma = 40;
			pc2.attributes.will = 50;
			pc2.attributes.wisdom = 40;
			pc2.vitals = m3::generate_initial_vitals(pc2.attributes);
			pc2.skills.military = 5;
			pc2.skills.dueling = 3;
			pc2.skills.brawl = 3;

			m3::player_character_data& pc3 = player_characters.emplace_back();
			pc3.name = "Squint";
			pc3.attributes.strength = 70;
			pc3.attributes.constitution = 30;
			pc3.attributes.agility = 80;
			pc3.attributes.cleverness = 70;
			pc3.attributes.hardiness = 30;
			pc3.attributes.focus = 60;
			pc3.attributes.charisma = 30;
			pc3.attributes.will = 40;
			pc3.attributes.wisdom = 40;
			pc3.vitals = m3::generate_initial_vitals(pc3.attributes);
			pc3.skills.foresting = 5;
			pc3.skills.military = 3;
			pc3.skills.urbanism = 3;

			m3::player_character_data& pc4 = player_characters.emplace_back();
			pc4.name = "Mallick";
			pc4.attributes.strength = 40;
			pc4.attributes.constitution = 70;
			pc4.attributes.agility = 20;
			pc4.attributes.cleverness = 30;
			pc4.attributes.hardiness = 60;
			pc4.attributes.focus = 40;
			pc4.attributes.charisma = 70;
			pc4.attributes.will = 60;
			pc4.attributes.wisdom = 60;
			pc4.vitals = m3::generate_initial_vitals(pc4.attributes);
			pc4.skills.astrology = 5;
			pc4.skills.rhetoric = 5;

			m3::player_character_data& pc5 = player_characters.emplace_back();
			pc5.name = "Crokus";
			pc5.attributes.strength = 40;
			pc5.attributes.constitution = 30;
			pc5.attributes.agility = 70;
			pc5.attributes.cleverness = 50;
			pc5.attributes.hardiness = 50;
			pc5.attributes.focus = 80;
			pc5.attributes.charisma = 40;
			pc5.attributes.will = 60;
			pc5.attributes.wisdom = 50;
			pc5.vitals = m3::generate_initial_vitals(pc5.attributes);
			pc5.skills.urbanism = 5;
			pc5.skills.hunt = 3;
			pc5.skills.sailoring = 3;

			m3::player_character_data& pc6 = player_characters.emplace_back();
			pc6.name = "Kruppe";
			pc6.attributes.strength = 30;
			pc6.attributes.constitution = 30;
			pc6.attributes.agility = 60;
			pc6.attributes.cleverness = 70;
			pc6.attributes.hardiness = 60;
			pc6.attributes.focus = 70;
			pc6.attributes.charisma = 30;
			pc6.attributes.will = 60;
			pc6.attributes.wisdom = 40;
			pc6.vitals = m3::generate_initial_vitals(pc6.attributes);
			pc6.skills.alchemy = 5;
			pc6.skills.medecine = 5;
		}

		enum hud_state
		{
			hud_state_play,
			hud_state_character_data,
		};
	}

	application::application(SDL_Window& window, egfx::module& gfx_module, config const& program_config)
		: window(&window)
		, gfx_module(&gfx_module)
		, program_config(&program_config)
		, main_scene(gfx_module, program_config)
	{

	}

	application& application::create_instance(SDL_Window& window, egfx::module& gfx_module, config const& program_config)
	{
		instance = new application(window, gfx_module, program_config);
		return *instance;
	}

	void application::destroy_instance() noexcept
	{
		delete instance;
		instance = nullptr;
	}

	application& application::get_instance()
	{
		return *instance;
	}

	void application::run()
	{
		generate_player_characters(player_data);

		std::chrono::time_point current_frame = std::chrono::steady_clock::now();
		math::seconds time_buffer = fixed_step; // Start with one step

		while (!wants_quit)
		{
			// Events
			process_events();

			// Pre-update
			imgui::pre_update();
			gfx_module->pre_update();
			im3d_preupdate(main_scene.get_camera());

			// Fixed Update
			while (time_buffer >= fixed_step)
			{
				main_scene.update(fixed_step);

				time_buffer -= fixed_step;
			}

			// Render
			main_scene.render();

			if (draw_debug)
			{
				draw_debug_menu();
			}

			draw_hud();

			if (!gfx_module->render())
				wants_quit = true;

			// End frame
			imgui::end_frame();

			std::chrono::time_point const last_frame = std::exchange(current_frame, std::chrono::steady_clock::now());
			time_buffer += current_frame - last_frame;
		}
	}

	void application::process_events()
	{
		std::vector<egfx::window_event> window_events;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				ImGui_ImplSDL2_ProcessEvent(&e);

				// Handle alt+f4 pressed on a window other than the main one
				if (ImGui::GetIO().WantCaptureKeyboard)
				{
					if (e.key.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F4 && (e.key.keysym.mod & KMOD_ALT) == KMOD_ALT)
						wants_quit = true;

					break;
				}

				if (e.key.type == SDL_KEYUP && e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					draw_debug = !draw_debug;
					break;
				}

				if (handle_hud_input(e))
					break;

				break;

			case SDL_WINDOWEVENT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				push_window_event(e, window_events);

				if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
				{
					wants_quit = true;
				}

				break;

			case SDL_TEXTINPUT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				break;

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				ImGui_ImplSDL2_ProcessEvent(&e);

				if (ImGui::GetIO().WantCaptureMouse)
					break;

				break;

			case SDL_MOUSEWHEEL:
				ImGui_ImplSDL2_ProcessEvent(&e);

				if (ImGui::GetIO().WantCaptureMouse)
					break;

				break;

			case SDL_MOUSEMOTION:
				ImGui_ImplSDL2_ProcessEvent(&e);

				if (ImGui::GetIO().WantCaptureKeyboard)
					break;

				break;

			case SDL_QUIT:
				wants_quit = true;
				break;
			}
		}

		gfx_module->on_window_events(window_events);
	}
	
	void application::save_game_data()
	{
		std::filesystem::path const game_data_path = get_game_data_path(*program_config);
		std::filesystem::create_directory(game_data_path);

		std::ofstream o(game_data_path / enemy_template_filename);
		if (!o)
			throw std::runtime_error(std::format("Could not save enemy template, '{}' could not be opened for write in game data folder", enemy_template_filename));

		save_enemy_template(o, enemy_templates);
	}

	void application::load_game_data()
	{
		enemy_templates.clear();

		std::ifstream i(get_game_data_path(*program_config) / enemy_template_filename);
		if (!i)
			return;

		enemy_templates = load_enemy_template(i);
	}

	std::span<m3::enemy_template> application::get_enemy_templates() noexcept
	{
		return enemy_templates;
	}

	void application::add_enemy_template(m3::enemy_template const& t)
	{
		enemy_templates.push_back(t);
	}

	std::span<m3::player_character_data> application::get_player_data() noexcept
	{
		return player_data;
	}

	bool application::handle_hud_input(SDL_Event const& e)
	{
		if (e.type == SDL_KEYDOWN)
		{
			if (hud_state == hud_state_play || hud_state == hud_state_character_data)
			{
				if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_6)
				{
					hud_param = e.key.keysym.sym - SDLK_1;
					hud_state = hud_state_character_data;
					return true;
				}
			}
		}
		else if (e.type == SDL_KEYUP)
		{
			if (hud_state == hud_state_character_data)
			{
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					hud_state = hud_state_play;
					return true;
				}
			}
		}

		return false;
	}

	void application::draw_hud()
	{
		switch (hud_state)
		{
		case hud_state_play:
			draw_player_vitals();
			break;

		case hud_state_character_data:
			draw_player_sheet();
			draw_player_vitals();
			break;
		}
	}

	void application::draw_player_vitals()
	{
		int window_width, window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);

		size_t const player_count = math::min_value(player_data.size(), 6);

		ImGui::SetNextWindowSize(ImVec2((player_count + 1) * 128.f, 256.f));
		ImGui::SetNextWindowPos(ImVec2(window_width/2, window_height * 0.75f), ImGuiCond_None, ImVec2(0.5f, 0));
		if (ImGui::Begin("##CharacterHud", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
		{			
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f*(i+1));
				ImGui::Text("%s", player_data[i].name.c_str());
			}

			ImGui::NewLine();

			ImGui::Text("Energy");

			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i+1));
				
				int const energy_percent = player_data[i].vitals.current_energy * 100 / player_data[i].vitals.max_energy;
				
				if (energy_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Full");
				}
				else if (energy_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Winded");
				}
				else if (energy_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Sweated");
				}				
				else if (energy_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Exhausted");
				}
				else if(energy_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Knocked Out");
				}
			}

			ImGui::Text("Resolve");
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));

				int const resolve_percent = player_data[i].vitals.current_resolve * 100 / player_data[i].vitals.max_resolve;
				if (resolve_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Confident");
				}
				else if (resolve_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Stressed");
				}
				else if (resolve_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Nervous");
				}
				else if (resolve_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Panicking");
				}
				else if (resolve_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Surrendered");
				}
			}

			ImGui::Text("Health");
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));

				int const health_percent = player_data[i].vitals.current_health * 100 / player_data[i].vitals.max_health;
				if (health_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Top Shape");
				}
				else if (health_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Scratched");
				}
				else if (health_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Injured");
				}
				else if (health_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Wounded");
				}
				else if (health_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Dead");
				}
			}
		}
		ImGui::End();
	}

	void application::draw_player_sheet()
	{
		if (player_data.size() == 0)
			return;

		if (hud_param < 0)
			hud_param = 0;

		if (hud_param >= player_data.size())
			hud_param = (int)player_data.size() - 1;

		m3::player_character_data const& player = player_data[hud_param];

		int window_width, window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);

		ImGui::SetNextWindowSize(ImVec2(1024.f, 512.f));
		ImGui::SetNextWindowPos(ImVec2(window_width / 2, window_height * 0.25f), ImGuiCond_None, ImVec2(0.5f, 0));
		if (ImGui::Begin("##CharacterSheet", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
		{
			ImGui::Text("%s", player.name.c_str());
			
			ImGui::NewLine();
			ImGui::Text("Cleverness: %d", player.attributes.cleverness);
			ImGui::Text("Hardiness: %d", player.attributes.hardiness);
			ImGui::Text("Focus: %d", player.attributes.focus);
			ImGui::Text("Charisma: %d", player.attributes.charisma);
			ImGui::Text("Will: %d", player.attributes.will);
			ImGui::Text("Wisdom: %d", player.attributes.wisdom);
			ImGui::Text("Strength: %d", player.attributes.strength);
			ImGui::Text("Constitution: %d", player.attributes.constitution);
			ImGui::Text("Agility: %d", player.attributes.agility);

			ImGui::NewLine();
			if (player.skills.military > 0)
				ImGui::Text("Military: %d", player.skills.military);
			if (player.skills.hunt > 0)
				ImGui::Text("Hunt: %d", player.skills.hunt);
			if (player.skills.brawl > 0)
				ImGui::Text("Brawl: %d", player.skills.brawl);
			if (player.skills.dueling > 0)
				ImGui::Text("Dueling: %d", player.skills.dueling);

			if (player.skills.foresting > 0)
				ImGui::Text("Foresting: %d", player.skills.foresting);
			if (player.skills.mountaineering > 0)
				ImGui::Text("Mountaineering: %d", player.skills.mountaineering);
			if (player.skills.sailoring > 0)
				ImGui::Text("Sailoring: %d", player.skills.sailoring);
			if (player.skills.urbanism > 0)
				ImGui::Text("Urbanism: %d", player.skills.urbanism);

			if (player.skills.rhetoric > 0)
				ImGui::Text("Rhetoric: %d", player.skills.rhetoric);
			if (player.skills.astrology > 0)
				ImGui::Text("Astrology: %d", player.skills.astrology);
			if (player.skills.medecine > 0)
				ImGui::Text("Medicine: %d", player.skills.medecine);
			if (player.skills.alchemy > 0)
				ImGui::Text("Alchemy: %d", player.skills.alchemy);
		}
		ImGui::End();
	}
}