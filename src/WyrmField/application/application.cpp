#include "application/application.h"
#include "application/game_mode.h"
#include "application/serialization.h"
#include "config.h"
#include "main_imgui.h"
#include "debug/debug_menu.h"

#include "math/ops.h"

#include "egfx/window_type.h"
#include "egfx/module.h"
#include "egfx/object/camera.h"

#include <SDL_events.h>
#include <imgui_impl_sdl2.h>
#include <im3d.h>
#include <imgui.h>

#include <filesystem>
#include <fstream>

#pragma warning(push)
#pragma warning(disable:4505) /* unreferenced function with internal linkage has been removed */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#pragma warning(pop)

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

		Im3d::Vec3 get_cursor_ray(egfx::camera_cref const camera, math::transform_matrix const& camera_projection)
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

		void im3d_preupdate(egfx::camera_cref const camera)
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

		void generate_player_characters(std::vector<m3::character_data>& player_characters)
		{
			m3::character_data& pc1 = player_characters.emplace_back();
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

			m3::character_data& pc2 = player_characters.emplace_back();
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

			m3::character_data& pc3 = player_characters.emplace_back();
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

			m3::character_data& pc4 = player_characters.emplace_back();
			pc4.name = "Heboric";
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

			m3::character_data& pc5 = player_characters.emplace_back();
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

			m3::character_data& pc6 = player_characters.emplace_back();
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
	}

	application::application(SDL_Window& window, egfx::module& gfx_module, config const& program_config)
		: window(&window)
		, gfx_module(&gfx_module)
		, program_config(&program_config)
		, main_scene(gfx_module, program_config)
		, game(get_play_mode(*this))
		, app_generator(std::random_device{}())
	{
		
	}

	application::~application() = default;

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
		load_monster_pack();

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
				game->update(fixed_step);
				main_scene.update(fixed_step);

				time_buffer -= fixed_step;
			}

			// Render
			main_scene.render();

			game->draw();

			if (draw_debug)
			{
				draw_debug_menu();
			}

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

				if (game->handle_hud_input(e))
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

	std::span<m3::character_data> application::get_player_data() noexcept
	{
		return player_data;
	}

	void application::change_game_mode(uptr<game_mode> new_game_mode)
	{
		game = as_moveable(new_game_mode);
	}

	void application::load_monster_pack()
	{
		auto const pack_path = std::filesystem::path(program_config->get_core().get_resource_root()) / "MonsterPack";

		auto const load_texture = [this, &pack_path](char const* sub_path)
		{
			auto const file_path = pack_path / sub_path;

			int image_width, image_height;
			int const component_count = 4;
			unsigned char* load_result = stbi_load(file_path.string().c_str(), &image_width, &image_height, nullptr, component_count);
			if (load_result == nullptr)
				throw std::runtime_error(std::format("Could not load image '{}'", sub_path));

			size_t const data_size = image_width * image_height * component_count;

			egfx::imgui::texture tex_result;
			if (!gfx_module->load_texture({ load_result, data_size }, image_width * component_count, tex_result))
				throw std::runtime_error(std::format("Could not load texture '{}'", sub_path));
			return tex_result;
		};

		combat_background = load_texture("RPGMP_Plains.png");
		
		auto const load_sprite_bundle = [this, &pack_path, &load_texture] (char const* sprite_name)
		{
			mp_portrait& portrait = portraits.emplace_back();
			portrait.name = sprite_name;
			
			try
			{
				portrait.tex_a = load_texture(std::format("Sprites/{}.png", sprite_name).c_str());
				portrait.tex_b = load_texture(std::format("Sprites/{}B.png", sprite_name).c_str());
				portrait.tex_shadow = load_texture(std::format("Sprites/{}Shadow.png", sprite_name).c_str());
			}
			catch (std::exception& e)
			{
				std::fprintf(stderr, "Failed to load sprite bundle '%s': %s", sprite_name, e.what());
				portraits.pop_back();
			}
		};

		load_sprite_bundle("AnimatedPlant");
		load_sprite_bundle("Bandit");
		load_sprite_bundle("Bat");
		load_sprite_bundle("Fairy");
		load_sprite_bundle("GelatinousCube");
		load_sprite_bundle("GiantHornet");
		load_sprite_bundle("GiantRat");
		load_sprite_bundle("Goblin");
		load_sprite_bundle("Merchant");
		load_sprite_bundle("Ogre");
		load_sprite_bundle("Orc");
		load_sprite_bundle("Skeleton");
		load_sprite_bundle("Slug");
		load_sprite_bundle("Treant");
		load_sprite_bundle("WildBoar");
		load_sprite_bundle("Wizard");
	}
}