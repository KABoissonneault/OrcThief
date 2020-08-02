#include "window.h"

#include "Ogre/Window.h"

namespace ot::graphics
{
	window::window(Ogre::Window* render, window_id id) noexcept
		: render(render)
		, id(id)
	{

	}

	int window::get_width() const noexcept
	{
		return static_cast<int>(render->getWidth());
	}

	int window::get_height() const noexcept
	{
		return static_cast<int>(render->getHeight());
	}

	int get_width(window const& w) noexcept
	{
		return w.get_width();
	}

	int get_height(window const& w) noexcept
	{
		return w.get_height();
	}
}