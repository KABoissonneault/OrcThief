#pragma once

#include "egfx/overlay/container.h"

#include <string>

namespace ot::egfx::overlay
{
	// A simple rectangular overlay container
	class panel : public container
	{

	};

	[[nodiscard]] panel create_panel(std::string const& name);
}
