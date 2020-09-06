#pragma once

#include "egfx/overlay/element.h"

namespace ot::egfx::overlay
{
	// A container is an overlay element that contains other sub-elements.
	// Only containers can be added to an overlay surface
	class container : public element
	{
	public:
		void add_child(element& c);
	};
}
