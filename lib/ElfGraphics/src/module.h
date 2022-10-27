#pragma once

#include "egfx/module.h"

#include "egfx/window_type.h"
#include "window.h"
#include "compositor.h"
#include "scene.h"
#include "imgui/system.h"

#include "core/uptr.h"

#include "math/unit/time.h"

namespace ot::egfx
{
	class module::impl
	{
		window main_window;
		pass_registry pass_reg;
		uptr<imgui::system> imgui;
		scene_impl* main_scene;

	public:
		~impl();

		[[nodiscard]] bool initialize(window_parameters const& window_params);

		[[nodiscard]] scene create_scene(std::string const& workspace, size_t number_threads);
		void set_main_scene(scene& s);

		void on_window_events(std::span<window_event const> events);

		void pre_update();
		[[nodiscard]] bool render();

		[[nodiscard]] window const& get_window(window_id id) const noexcept;

		bool load_texture(std::span<unsigned char> image_rgba_data, int pitch, imgui::texture& t);
		void free_texture(imgui::texture& t);
	};
}
