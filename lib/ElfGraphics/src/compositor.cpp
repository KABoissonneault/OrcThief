#include "compositor.h"

#include <cassert>

namespace ot::egfx
{
	compositor_pass_def::compositor_pass_def(Ogre::CompositorTargetDef* parent_target_def)
		: Ogre::CompositorPassDef(Ogre::PASS_CUSTOM, parent_target_def)
	{

	}

	void pass_registry::register_pass(pass_provider* p)
	{
		providers.push_back(p);
	}

	void pass_registry::unregister_pass(pass_provider* p)
	{
		auto const it = std::find(providers.begin(), providers.end(), p);
		if (it == providers.end())
			return;

		providers.erase(it);
	}

	Ogre::CompositorPassDef* pass_registry::addPassDef(Ogre::CompositorPassType pass_type
		, Ogre::IdString custom_id
		, Ogre::CompositorTargetDef* parent_target_def
		, Ogre::CompositorNodeDef* parent_node_def
	)
	{
		assert(pass_type == Ogre::PASS_CUSTOM);

		for (auto const provider : providers)
		{
			if (custom_id == provider->get_id())
				return provider->make_pass_def(parent_target_def, parent_node_def);
		}

		return nullptr;
	}

	Ogre::CompositorPass* pass_registry::addPass(Ogre::CompositorPassDef const* definition
		, Ogre::Camera* default_camera
		, Ogre::CompositorNode* parent_node
		, const Ogre::RenderTargetViewDef* rtv_def
		, Ogre::SceneManager* scene_manager
	)
	{
		auto const concrete_def = static_cast<compositor_pass_def const*>(definition);
		Ogre::IdString const& custom_id = concrete_def->get_id();

		for (auto const provider : providers)
		{
			if (custom_id == provider->get_id())
				return provider->make_pass(concrete_def, default_camera, parent_node, rtv_def, scene_manager);
		}

		throw std::runtime_error("Unknown pass found in compositor");
	}
}
