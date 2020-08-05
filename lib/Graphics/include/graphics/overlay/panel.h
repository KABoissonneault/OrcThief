#pragma once

#include "graphics/overlay/container.h"

#include <string>

namespace ot::graphics::overlay
{
	// A simple rectangular overlay container
	class panel : public container
	{

	};

	[[nodiscard]] panel create_panel(std::string const& name);
}
