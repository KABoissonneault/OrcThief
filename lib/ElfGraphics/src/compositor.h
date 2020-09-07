#pragma once

#include "Ogre/Compositor/PassDef.h"
#include "Ogre/Compositor/Pass.h"
#include "Ogre/Compositor/PassProvider.h"

#include <vector>

namespace ot::egfx
{
	class compositor_pass_def : public Ogre::CompositorPassDef
	{
	protected:
		compositor_pass_def(Ogre::CompositorTargetDef* parent_target_def);

	public:

		virtual Ogre::IdString const& get_id() const = 0;
	};

	class pass_provider
	{
	public:
		virtual Ogre::IdString const& get_id() const = 0;
		virtual compositor_pass_def* make_pass_def(Ogre::CompositorTargetDef* parent_target_def, Ogre::CompositorNodeDef* parent_node_def) = 0;
		virtual Ogre::CompositorPass* make_pass(compositor_pass_def const* definition
			, Ogre::Camera* default_camera
			, Ogre::CompositorNode* parent_node
			, Ogre::RenderTargetViewDef const* rtv_def
			, Ogre::SceneManager* scene_manager
		) = 0;
	};

	class pass_registry : public Ogre::CompositorPassProvider
	{
		std::vector<pass_provider*> providers;

		virtual Ogre::CompositorPassDef* addPassDef(Ogre::CompositorPassType pass_type
			, Ogre::IdString custom_id
			, Ogre::CompositorTargetDef* parent_target_def
			, Ogre::CompositorNodeDef* parent_node_def
		) override;

		virtual Ogre::CompositorPass* addPass(const Ogre::CompositorPassDef* definition
			, Ogre::Camera* default_camera
			, Ogre::CompositorNode* parent_node
			, const Ogre::RenderTargetViewDef* rtv_def
			, Ogre::SceneManager* scene_manager
		) override;

	public:
		void register_pass(pass_provider* p);
		void unregister_pass(pass_provider* p);
	};
}
