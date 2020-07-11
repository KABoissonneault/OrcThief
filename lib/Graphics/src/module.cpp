#include "graphics/module.h"

#include "Ogre/Compositor/CompositorManager2.h"

const Ogre::ColourValue k_background_color(.8f, 0.4f, 0.4f);

namespace ot::graphics
{
	void module::initialize(char const* window_title, int number_threads)
	{
		auto& root = Ogre::Root::getSingleton();

		main_window = ot::graphics::window::create(window_title);
		scene_manager = root.createSceneManager(Ogre::ST_GENERIC, number_threads);
		main_camera = scene_manager->createCamera("Main Camera");

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		compositor_manager->createBasicWorkspaceDef("Test Workspace", k_background_color);
		main_workspace = compositor_manager->addWorkspace(scene_manager, main_window.get_render_texture(), main_camera, "Test Workspace", true /*enabled*/);
	}

	void module::on_window_events(std::span<window_event const> events)
	{
		for (ot::graphics::window_event const& event : events)
		{
			if (event.id != main_window.get_window_id())
			{
				continue;
			}

			std::visit([this](auto e)
				{
					if constexpr (std::is_same_v<decltype(e), window_event::moved>)
						main_window.on_moved(e.new_x, e.new_y);
					else if constexpr (std::is_same_v<decltype(e), window_event::resized>)
						main_window.on_resized(e.new_width, e.new_height);
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_gained>)
						main_window.on_focus_gained();
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_lost>)
						main_window.on_focus_lost();
				}, event.data);
		}
	}

	bool module::render()
	{
		if (main_window.is_visible())
			return Ogre::Root::getSingleton().renderOneFrame();

		return true;
	}
}