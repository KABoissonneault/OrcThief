#pragma once

#include "egfx/object/light.fwd.h"
#include "egfx/object/object.h"

namespace ot::egfx
{
	namespace detail
	{
		light_ref make_light_ref(void*);
		light_cref make_light_cref(void const*);
	}

	class light_ref : public object_ref 
	{ 
		friend light_ref detail::make_light_ref(void*);

	public:
		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return object_type::light; }

		[[nodiscard]] light_type get_light_type() const noexcept;
	};

	extern template light_ref ref_cast<light_ref>(object_ref);

	class light_cref : public object_cref
	{
		light_cref() = default;

		friend light_cref detail::make_light_cref(void const*);

	public:
		light_cref(light_ref rhs) noexcept;

		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return object_type::light; }

		[[nodiscard]] light_type get_light_type() const noexcept;
	};

	extern template light_cref ref_cast<light_cref>(object_cref);

	[[nodiscard]] void add_directional_light(node_ref owner);
}
