#include "imgui/system.h"

#include "core/fwd_delete.h"

#include "Ogre/Root.h"
#include "Ogre/Compositor/Manager2.h"
#include "Ogre/Compositor/Workspace.h"
#include "Ogre/Compositor/NodeDef.h"
#include "Ogre/Compositor/Pass.h"
#include "Ogre/Compositor/PassDef.h"
#include "Ogre/Compositor/PassProvider.h"

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

		class pass_def : public Ogre::CompositorPassDef
		{
		public:
			pass_def(Ogre::CompositorTargetDef* parentTargetDef)
				: Ogre::CompositorPassDef(Ogre::PASS_CUSTOM, parentTargetDef)
			{
				mProfilingId = "ImGui";
			}
		};
	}

	class pass_provider : public Ogre::CompositorPassProvider
	{
		renderer* render;
	public:
		pass_provider(renderer* render)
			: render(render)
		{

		}

		virtual pass_def* addPassDef(Ogre::CompositorPassType passType
			, Ogre::IdString customId
			, Ogre::CompositorTargetDef* parentTargetDef
			, Ogre::CompositorNodeDef* parentNodeDef
		) override {
			(void)passType;
			(void)parentNodeDef;

			if (customId == k_compositorPassDefId)
				return OGRE_NEW pass_def(parentTargetDef);

			return nullptr;
		}

		virtual pass* addPass(const Ogre::CompositorPassDef* definition
			, Ogre::Camera* defaultCamera
			, Ogre::CompositorNode* parentNode
			, const Ogre::RenderTargetViewDef* rtvDef
			, Ogre::SceneManager* sceneManager
		) override {
			return OGRE_NEW pass(definition, parentNode, render);
		}
	};

	bool system::initialize(Ogre::CompositorWorkspaceDef* compositor_workspace_def)
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const renderSystem = root.getRenderSystem();
		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
		{
			render.reset(new d3d11_renderer);
		}
		else
		{
			return false;
		}

		provider.reset(new pass_provider(render.get()));

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		compositor_manager->setCompositorPassProvider(provider.get());
		Ogre::CompositorNodeDef* const imgui_node_def = compositor_manager->addNodeDefinition("ImGuiNode");
		compositor_workspace_def->addNodeAlias("ImGuiNode", "ImGuiNode");

		imgui_node_def->setNumTargetPass(1);
		imgui_node_def->addRenderTextureView("ImGuiRT");
		Ogre::CompositorTargetDef* const imgui_target_def = imgui_node_def->addTargetPass("ImGuiRT");
		imgui_target_def->addPass(Ogre::PASS_CUSTOM, k_compositorPassDefId);

		return render->initialize();
	}

	void system::shutdown()
	{
		render->shutdown();
	}

	void system::new_frame()
	{
		render->new_frame();
	}
}

template struct ot::fwd_delete<ot::egfx::imgui::pass_provider>;