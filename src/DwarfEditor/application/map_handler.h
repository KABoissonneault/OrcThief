#pragma once

#include "map.h"
#include "egfx/node/object.h"

namespace ot::dedit
{
	template<typename Application>
	class map_handler
	{
		using derived = Application;

		std::string map_path;

	public:
		void new_map();
		void open_map();
		void save_map();
		void save_map_as();
	};

	extern template class map_handler<class application>;
}