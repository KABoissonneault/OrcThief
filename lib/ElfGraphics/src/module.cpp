#include "egfx/module.h"

#include "module.h"
#include "scene.h"
#include "object/camera.h"

#include "Ogre/RenderSystem.h"
#include "Ogre/Compositor/Manager2.h"
#include "Ogre/Components/Overlay/Manager.h"
#include "Ogre/Window.h"

namespace ot::egfx
{
	namespace
	{
		bool get_vsync(ot::ogre::config_option_map const& config)
		{
			bool vsync;
			return ot::ogre::render_system::get_vsync(config, vsync) ? vsync : false;
		}

		bool get_hardware_gamma_conversion(ot::ogre::config_option_map const& config)
		{
			bool gamma;
			return ot::ogre::render_system::get_hardware_gamma_conversion(config, gamma) ? gamma : false;
		}
	}

	bool module::impl::initialize(window_parameters const& window_params)
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const render_system = root.getRenderSystem();
		auto const& config = render_system->getConfigOptions();

		auto const misc_params = ot::ogre::render_window_misc_params{}
			.set_external_window_handle(window_params.window_handle)
			.set_hardware_gamma_conversion(get_hardware_gamma_conversion(config))
			.set_vsync(get_vsync(config))
			.params;
		auto const render_window = root.createRenderWindow(window_params.window_handle, window_params.width, window_params.height, window_params.fullscreen, &misc_params);
		auto const window_id = window_params.event_id;

		main_window = { render_window, window_id };

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		compositor_manager->setCompositorPassProvider(&pass_reg);

		imgui.reset(new imgui::system);
		if (!imgui->initialize(pass_reg))
		{
			std::fprintf(stderr, "error [Graphics]: cannot initialize ImGui\n");
			return false;
		}

		return true;
	}

	module::impl::~impl()
	{
		imgui->shutdown();
	}

	auto module::impl::create_scene(std::string const& workspace, size_t num_threads) -> scene
	{
		scene s;
		init_scene(s, uptr<scene_impl, fwd_delete<scene_impl>>{ new scene_impl(num_threads, main_window.get_window().getTexture(), workspace) });
		if (main_scene == nullptr)
		{
			set_main_scene(s);
		}
		return s;
	}

	void module::impl::set_main_scene(scene& s)
	{
		main_scene = &get_impl(s);
	}

	void module::impl::on_window_events(std::span<window_event const> events)
	{
		for (window_event const& event : events)
		{
			if (event.id != main_window.get_id())
			{
				continue;
			}

			std::visit([this](auto e)
			{
				if constexpr (std::is_same_v<decltype(e), window_event::moved>)
					main_window.get_window().windowMovedOrResized();
				else if constexpr (std::is_same_v<decltype(e), window_event::resized>)
					main_window.get_window().windowMovedOrResized();
				else if constexpr (std::is_same_v<decltype(e), window_event::focus_gained>)
					main_window.get_window().setFocused(true);
				else if constexpr (std::is_same_v<decltype(e), window_event::focus_lost>)
					main_window.get_window().setFocused(false);
			}, event.data);
		}
	}

	void module::impl::pre_update()
	{
		object::camera_cref const impl = main_scene->get_camera();
		imgui->pre_update(get_camera(impl));
	}

	bool module::impl::render()
	{
		if (main_window.get_window().isVisible())
			return Ogre::Root::getSingleton().renderOneFrame();

		return true;
	}

	window const& module::impl::get_window(window_id id) const noexcept
	{
		assert(id == main_window.get_id());
		return main_window;
	}

	module::module()
		: pimpl(new impl)
	{

	}

	module::~module() = default;

	bool module::initialize(window_parameters const& window_params)
	{
		return pimpl->initialize(window_params);
	}

	auto module::create_scene(std::string const& workspace, size_t number_threads) -> scene
	{
		return pimpl->create_scene(workspace, number_threads);
	}

	void module::set_main_scene(scene& s)
	{
		pimpl->set_main_scene(s);
	}

	void module::on_window_events(std::span<window_event const> events)
	{
		pimpl->on_window_events(events);
	}

	void module::pre_update()
	{
		pimpl->pre_update();
	}

	bool module::render()
	{
		return pimpl->render();
	}

	window const& module::get_window(window_id id) const noexcept
	{
		return pimpl->get_window(id);
	}
}
