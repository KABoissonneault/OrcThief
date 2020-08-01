#include "graphics/module.h"

#include "module.h"
#include "scene.h"

#include "Ogre/RenderSystem.h"
#include "Ogre/Compositor/CompositorManager2.h"
#include "Ogre/Window.h"


const Ogre::ColourValue k_background_color(.8f, 0.4f, 0.4f);
const Ogre::String k_workspace_def("DefaultWorkspace");

namespace ot::graphics
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

	void module::impl::initialize(window_parameters const& window_params)
	{
		auto& root = Ogre::Root::getSingleton();
		auto const& config = root.getRenderSystem()->getConfigOptions();

		auto const misc_params = ot::ogre::render_window_misc_params{}
			.set_external_window_handle(window_params.window_handle)
			.set_hardware_gamma_conversion(get_hardware_gamma_conversion(config))
			.set_vsync(get_vsync(config))
			.params;
		render_window = root.createRenderWindow(window_params.window_handle, window_params.width, window_params.height, window_params.fullscreen, &misc_params);
		render_window_id = window_params.event_id;

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		compositor_manager->createBasicWorkspaceDef(k_workspace_def, k_background_color);
	}

	auto module::impl::create_scene(size_t num_threads) -> scene
	{
		scene s;
		init_scene(s, uptr<scene_impl, fwd_delete<scene_impl>>{ new scene_impl(num_threads, render_window->getTexture(), k_workspace_def) });
		return s;
	}

	void module::impl::on_window_events(std::span<window_event const> events)
	{
		for (ot::graphics::window_event const& event : events)
		{
			if (event.id != render_window_id)
			{
				continue;
			}

			std::visit([this](auto e)
			{
				if constexpr (std::is_same_v<decltype(e), window_event::moved>)
					render_window->windowMovedOrResized();
				else if constexpr (std::is_same_v<decltype(e), window_event::resized>)
					render_window->windowMovedOrResized();
				else if constexpr (std::is_same_v<decltype(e), window_event::focus_gained>)
					render_window->setFocused(true);
				else if constexpr (std::is_same_v<decltype(e), window_event::focus_lost>)
					render_window->setFocused(false);
			}, event.data);
		}
	}

	void module::impl::update(math::seconds dt)
	{
		(void)dt;
	}

	bool module::impl::render()
	{
		if (render_window->isVisible())
			return Ogre::Root::getSingleton().renderOneFrame();

		return true;
	}

	module::module()
		: pimpl(new impl)
	{

	}

	module::~module() = default;

	void module::initialize(window_parameters const& window_params)
	{
		pimpl->initialize(window_params);
	}

	auto module::create_scene(size_t number_threads) -> scene
	{
		return pimpl->create_scene(number_threads);
	}

	void module::on_window_events(std::span<window_event const> events)
	{
		pimpl->on_window_events(events);
	}

	void module::update(math::seconds dt)
	{
		pimpl->update(dt);
	}

	bool module::render()
	{
		return pimpl->render();
	}
}
