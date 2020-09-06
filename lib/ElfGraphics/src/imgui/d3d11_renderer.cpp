#include "imgui/d3d11_renderer.h"

#include "Ogre/Root.h"

#include <imgui_impl_dx11.h>
#include <d3d11.h>

namespace ot::egfx::imgui
{
	bool d3d11_renderer::initialize()
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const renderSystem = root.getRenderSystem();

		ID3D11Device* d3dDevice = nullptr;
		renderSystem->getCustomAttribute("D3DDEVICE", &d3dDevice);
		if (d3dDevice == nullptr)
			return false;

		ID3D11DeviceContext* immediateContext;
		d3dDevice->GetImmediateContext(&immediateContext);

		return ImGui_ImplDX11_Init(d3dDevice, immediateContext);
	}

	void d3d11_renderer::shutdown()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void d3d11_renderer::new_frame()
	{
		ImGui_ImplDX11_NewFrame();
	}

	void d3d11_renderer::render()
	{
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
