#include "window.h"

#include "Ogre/Window.h"

namespace ot::egfx
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

	bool window::has_focus() const noexcept
	{
		return render->isFocused();
	}

	int get_width(window const& w) noexcept
	{
		return w.get_width();
	}

	int get_height(window const& w) noexcept
	{
		return w.get_height();
	}

	bool has_focus(window const& w) noexcept
	{
		return w.has_focus();
	}
}
