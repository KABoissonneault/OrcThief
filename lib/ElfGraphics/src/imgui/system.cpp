#include "imgui/system.h"

#include "core/fwd_delete.h"

#include "Ogre/Root.h"
#include "Ogre/Compositor/Pass.h"
#include "Ogre/Compositor/PassDef.h"

#include "imgui/d3d11_renderer.h"

namespace ot::egfx::imgui
{
	namespace
	{
		const char* const k_compositorPassDefId = "imgui";

		class pass : public Ogre::CompositorPass
		{
			renderer* render;
		public:
			pass(const Ogre::CompositorPassDef* definition, Ogre::CompositorNode* parentNode, renderer* render)
				: Ogre::CompositorPass(definition, parentNode)
				, render(render)
			{

			}

			virtual void execute(const Ogre::Camera* camera)
			{
				(void)camera;
				render->render();
			}
		};

		class pass_def : public compositor_pass_def
		{
			Ogre::IdString id;
		public:
			pass_def(Ogre::CompositorTargetDef* parentTargetDef, Ogre::IdString const& imgui_id)
				: compositor_pass_def(parentTargetDef)
				, id(imgui_id)
			{
				mProfilingId = "ImGui";
			}

			Ogre::IdString const& get_id() const override
			{
				return id;
			}
		};
	}

	bool system::initialize(pass_registry& r)
	{
		pass_reg = &r;
		r.register_pass(this);

		imgui_id = "imgui";

		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const renderSystem = root.getRenderSystem();
		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
		{
			render.reset(new d3d11_renderer);
		}
		else
		{
			// TODO: more ImGui renderers
			return false;
		}

		return render->initialize();
	}

	void system::shutdown()
	{
		render->shutdown();

		pass_reg->unregister_pass(this);
	}

	void system::new_frame()
	{
		render->new_frame();
	}

	Ogre::IdString const& system::get_id() const
	{
		return imgui_id;
	}

	compositor_pass_def* system::make_pass_def(Ogre::CompositorTargetDef* parentTargetDef, Ogre::CompositorNodeDef* parentNodeDef)
	{
		return OGRE_NEW pass_def(parentTargetDef, imgui_id);
	}

	Ogre::CompositorPass* system::make_pass(compositor_pass_def const* definition
		, Ogre::Camera* default_camera
		, Ogre::CompositorNode* parent_node
		, Ogre::RenderTargetViewDef const* rtv_def
		, Ogre::SceneManager* scene_manager
	)
	{
		return OGRE_NEW pass(definition, parent_node, render.get());
	}
}
