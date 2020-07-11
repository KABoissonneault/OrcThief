#pragma once

#include "graphics/window_type.h"
#include "SDL2/window.h"
#include "Ogre/Window.h"

namespace ot::graphics
{
	class window
	{
		ot::sdl::unique_window physical_window;
		Ogre::Window* render_window;

		window(SDL_Window* physical_window, Ogre::Window* render_window)
			: physical_window(physical_window)
			, render_window(render_window)
		{

		}
	public:
		window() = default;

		static window create(char const* title);

		Ogre::TextureGpu* get_render_texture() const noexcept
		{
			return render_window->getTexture();
		}

		window_id get_window_id() const noexcept;

		bool is_visible() const noexcept
		{
			return render_window->isVisible();
		}

		void on_moved(int new_x, int new_y)
		{
			render_window->windowMovedOrResized();
		}

		void on_resized(int width, int height)
		{
			render_window->windowMovedOrResized();
		}

		void on_focus_gained()
		{
			render_window->setFocused(true);
		}

		void on_focus_lost()
		{
			render_window->setFocused(false);
		}
	};
}