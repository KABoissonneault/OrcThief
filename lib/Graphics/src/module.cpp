#include "graphics/module.h"

#include "module.h"
#include "scene.h"

#include "Ogre/RenderSystem.h"
#include "Ogre/Compositor/Manager2.h"
#include "Ogre/Components/Overlay/Manager.h"
#include "Ogre/Window.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>

#include <d3d11.h>

const Ogre::ColourValue k_background_color(0.2f, 0.8f, 0.6f);
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

		if (render_system->getName() == ogre::render_system::d3d11::name)
		{
			ID3D11Device* device = nullptr;
			render_window->getCustomAttribute(ogre::render_system::d3d11::attribute::device, &device);
			assert(device != nullptr);

			ID3D11DeviceContext* device_context;
			device->GetImmediateContext(&device_context);
			assert(device_context != nullptr);

			if (!ImGui_ImplDX11_Init(device, device_context))
			{
				std::fprintf(stderr, "error [Graphics]: cannot initialize ImGui DX11\n");
				return false;
			}
		}
		else
		{
			std::printf("error [Graphics]: unsupported render system '%s'\n", render_system->getName().c_str());
			return false;
		}

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		compositor_manager->createBasicWorkspaceDef(k_workspace_def, k_background_color);

		overlay_system.reset(new Ogre::v1::OverlaySystem);

		return true;
	}

	module::impl::~impl()
	{
		ImGui_ImplDX11_Shutdown();
	}

	auto module::impl::create_scene(size_t num_threads) -> scene
	{
		scene s;
		init_scene(s, uptr<scene_impl, fwd_delete<scene_impl>>{ new scene_impl(num_threads, main_window.get_window().getTexture(), k_workspace_def) });
		
		Ogre::SceneManager& scene_manager = get_impl(s).get_scene_manager();
		scene_manager.addRenderQueueListener(overlay_system.get());
		scene_manager.getRenderQueue()->setSortRenderQueue(Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId, Ogre::RenderQueue::StableSort);
		return s;
	}

	void module::impl::on_window_events(std::span<window_event const> events)
	{
		for (ot::graphics::window_event const& event : events)
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

	void module::impl::update(math::seconds dt)
	{
		(void)dt;
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

	window const& module::get_window(window_id id) const noexcept
	{
		return pimpl->get_window(id);
	}
}
