#pragma once

#include "egfx/object/light.fwd.h"
#include "egfx/object/object.h"
#include "egfx/color.h"

#include "core/directive.h"

#include <string_view>

namespace ot::egfx
{
	namespace detail
	{
		light_ref make_light_ref(void*);
		light_cref make_light_cref(void const*);

		template<typename Derived>
		class light_const_impl
		{
			using derived = Derived;

		public:
			// Gets the light type
			[[nodiscard]] light_type get_light_type() const noexcept;

			// Gets the power scale of the light
			[[nodiscard]] float get_power_scale() const noexcept;

			// Gets the diffuse color of the light
			[[nodiscard]] color get_diffuse() const noexcept;

			// Gets the upper range of the light attenuation
			[[nodiscard]] float get_attenuation_range() const noexcept;

			// Gets the constant factor of the light attenuation formula
			[[nodiscard]] float get_attenuation_const() const noexcept;

			// Gets the linear factor of the light attenuation formula
			[[nodiscard]] float get_attenuation_linear() const noexcept;

			// Gets the quadratic factor of the light attenuation formula
			[[nodiscard]] float get_attenuation_quadratic() const noexcept;
		};
	}

	inline constexpr std::string_view as_string(light_type t)
	{
		switch (t)
		{
		case light_type::directional: return "Directional";
		case light_type::point: return "Point";
		case light_type::spotlight: return "Spotlight";
		}

		OT_UNREACHABLE();
	}

	class light_ref : public object_ref, public detail::light_const_impl<light_ref>
	{ 
		friend light_ref detail::make_light_ref(void*);

	public:
		static constexpr object_type type = object_type::light;

		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return type; }
		
		// Sets the light type
		void set_light_type(light_type new_type) const noexcept;

		// Sets the power scale of the light
		void set_power_scale(float value) const noexcept;

		// Gets the diffuse color of the light
		void set_diffuse(color value) const noexcept;

		// Sets the light attenuation parameters
		void set_attenuation(float range, float constant, float linear, float quad) const noexcept;
	};

	extern template light_ref ref_cast<light_ref>(object_ref);

	class light_cref : public object_cref, public detail::light_const_impl<light_cref>
	{
		light_cref() = default;

		friend light_cref detail::make_light_cref(void const*);

	public:
		light_cref(light_ref rhs) noexcept;

		static constexpr object_type type = object_type::light;

		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return type; }
	};

	extern template light_cref ref_cast<light_cref>(object_cref);

	extern template class detail::light_const_impl<light_cref>;
	extern template class detail::light_const_impl<light_ref>;

	light_ref add_light(node_ref owner, light_type type);
}
