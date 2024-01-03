#pragma once

#include "core/build_config.h"

#include <vector>
#include <string>

namespace ot::egfx
{
	struct material_handle_t
	{
#if OT_BUILD_DEBUG
		alignas(unsigned) char storage[sizeof(unsigned) + 32];
#else
		alignas(unsigned) char storage[sizeof(unsigned)];
#endif

		// Returns whether the handle is null or not. Non-null handles can still be invalid 
		[[nodiscard]] bool is_null() const;
				
		// Returns whether the two handles are the same
		[[nodiscard]] bool operator==(material_handle_t const& rhs) const;

		// Returns a debug text representation
		[[nodiscard]] std::string to_debug_string() const;
	};

	std::vector<material_handle_t> get_registered_materials();
	std::string get_material_name(material_handle_t const& h);
}
