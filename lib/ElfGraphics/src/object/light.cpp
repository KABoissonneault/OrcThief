#include "light.h"

#include "node.h"
#include "object.h"

#include "Ogre/SceneManager.h"
#include "Ogre/SceneNode.h"

namespace ot::egfx
{
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

	light_type light_cref::get_light_type() const noexcept
	{
		return static_cast<light_type>(get_light(*this).getType());
	}

	template<>
	light_cref ref_cast<light_cref>(object_cref ref)
	{
		assert(ref.is_a(object_type::light));
		return make_light_cref(static_cast<Ogre::Light const&>(get_object(ref)));
	}

	void add_directional_light(node_ref owner)
	{
		Ogre::SceneNode& owner_node = get_scene_node(owner);
		Ogre::SceneManager& scene_manager = *owner_node.getCreator();
		Ogre::Light* light_object = scene_manager.createLight();
		light_object->setPowerScale(Ogre::Math::PI); // Apparently, "PBS" makes light weaker by a factor of PI when HDR is not used
		light_object->setType(Ogre::Light::LT_DIRECTIONAL);

		owner_node.attachObject(light_object);
	}
}
