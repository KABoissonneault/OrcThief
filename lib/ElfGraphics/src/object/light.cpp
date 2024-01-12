#include "light.h"

#include "node.h"
#include "object.h"

#include "Ogre/SceneManager.h"
#include "Ogre/SceneNode.h"

#include <numbers>

namespace ot::egfx
{
	static_assert(static_cast<int>(light_type::directional) == Ogre::Light::LightTypes::LT_DIRECTIONAL);
	static_assert(static_cast<int>(light_type::point) == Ogre::Light::LightTypes::LT_POINT);
	static_assert(static_cast<int>(light_type::spotlight) == Ogre::Light::LightTypes::LT_SPOTLIGHT);

	namespace detail
	{
		light_ref make_light_ref(void* pimpl)
		{
			light_ref ref;
			detail::init_object_ref(ref, pimpl);
			return ref;
		}

		light_cref make_light_cref(void const* pimpl)
		{
			light_cref ref;
			detail::init_object_cref(ref, pimpl);
			return ref;
		}

		template<typename Derived>
		light_type light_const_impl<Derived>::get_light_type() const noexcept
		{
			return static_cast<light_type>(get_light(static_cast<Derived const&>(*this)).getType());
		}

		template<typename Derived>
		float light_const_impl<Derived>::get_power_scale() const noexcept
		{
			// Because no HDR, we scale up the intended power scale by pi
			// Scale back down for the external user
			return get_light(static_cast<Derived const&>(*this)).getPowerScale() * std::numbers::inv_pi_v<float>;
		}

		template<typename Derived>
		color light_const_impl<Derived>::get_diffuse() const noexcept
		{
			Ogre::ColourValue const& value = get_light(static_cast<Derived const&>(*this)).getDiffuseColour();
			return color{ value.r, value.g, value.b, value.a };
		}

		template<typename Derived>
		float light_const_impl<Derived>::get_attenuation_range() const noexcept
		{
			return get_light(static_cast<Derived const&>(*this)).getAttenuationRange();
		}

		template<typename Derived>
		float light_const_impl<Derived>::get_attenuation_const() const noexcept
		{
			return get_light(static_cast<Derived const&>(*this)).getAttenuationConstant();
		}

		template<typename Derived>
		float light_const_impl<Derived>::get_attenuation_linear() const noexcept
		{
			return get_light(static_cast<Derived const&>(*this)).getAttenuationLinear();
		}

		template<typename Derived>
		float light_const_impl<Derived>::get_attenuation_quadratic() const noexcept
		{
			return get_light(static_cast<Derived const&>(*this)).getAttenuationQuadric();
		}
	}

	light_ref make_light_ref(Ogre::Light& l) noexcept
	{
		return detail::make_light_ref(&l);
	}

	light_cref make_light_cref(Ogre::Light const& l) noexcept
	{
		return detail::make_light_cref(&l);
	}

	Ogre::Light& get_light(light_ref ref) noexcept
	{
		return static_cast<Ogre::Light&>(get_object(ref));
	}

	Ogre::Light const& get_light(light_cref ref) noexcept
	{
		return static_cast<Ogre::Light const&>(get_object(ref));
	}

	void light_ref::set_light_type(light_type new_type) const noexcept
	{
		get_light(*this).setType(static_cast<Ogre::Light::LightTypes>(new_type));
	}

	void light_ref::set_power_scale(float value) const noexcept
	{
		get_light(*this).setPowerScale(value * std::numbers::pi_v<float>);
	}

	void light_ref::set_diffuse(color value) const noexcept
	{
		get_light(*this).setDiffuseColour(value.r, value.g, value.b);
	}

	void light_ref::set_attenuation(float range, float constant, float linear, float quad) const noexcept
	{
		get_light(*this).setAttenuation(range, constant, linear, quad);
	}

	template<>
	light_ref ref_cast<light_ref>(object_ref ref)
	{		
		assert(ref.is_a(object_type::light));
		return make_light_ref(static_cast<Ogre::Light&>(get_object(ref)));
	}

	light_cref::light_cref(light_ref rhs) noexcept
	{
		detail::init_object_cref(*this, detail::get_object_impl(rhs));
	}
	
	template<>
	light_cref ref_cast<light_cref>(object_cref ref)
	{
		assert(ref.is_a(object_type::light));
		return make_light_cref(static_cast<Ogre::Light const&>(get_object(ref)));
	}

	light_ref add_light(node_ref owner, light_type type)
	{
		Ogre::SceneNode& owner_node = get_scene_node(owner);
		Ogre::SceneManager& scene_manager = *owner_node.getCreator();
		Ogre::Light* light_object = scene_manager.createLight();
		light_object->setPowerScale(Ogre::Math::PI); // Apparently, "PBS" makes light weaker by a factor of PI when HDR is not used
		light_object->setType(static_cast<Ogre::Light::LightTypes>(type));


		switch (type)
		{
		case light_type::point:
		case light_type::spotlight:
			light_object->setAttenuationBasedOnRadius(10.f, 0.1f);
			break;
		}

		owner_node.attachObject(light_object);

		return make_light_ref(*light_object);
	}

	template class detail::light_const_impl<light_cref>;
	template class detail::light_const_impl<light_ref>;
}
