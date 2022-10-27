#pragma once

#include "core/uptr.h"
#include "imgui/renderer.h"

#include "compositor.h"

namespace ot::egfx::imgui
{
	class system : public pass_provider
	{
		uptr<renderer> render;
		Ogre::IdString imgui_id;
		pass_registry* pass_reg;

	public:
		bool initialize(pass_registry& r);
		void shutdown();
		void pre_update(Ogre::Camera const& scene_camera);
		
		// pass_provider
		virtual Ogre::IdString const& get_id() const override;
		virtual compositor_pass_def* make_pass_def(Ogre::CompositorTargetDef* parentTargetDef, Ogre::CompositorNodeDef* parentNodeDef) override;
		virtual Ogre::CompositorPass* make_pass(compositor_pass_def const* definition
			, Ogre::Camera* defaultCamera
			, Ogre::CompositorNode* parentNode
			, Ogre::RenderTargetViewDef const* rtvDef
			, Ogre::SceneManager* sceneManager
		) override;

		bool load_texture(std::span<unsigned char> image_rgba_data, int pitch, texture& t);
		void free_texture(texture& t);
	};

}
