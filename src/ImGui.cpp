#include "ImGui.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>

#include "Ogre/Root.h"
#include "Ogre/Direct3D11/RenderSystem.h"

namespace ot
{
	bool initialize_imgui(SDL_Window& window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		if (!ImGui_ImplSDL2_InitForD3D(&window))
			return false;

		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const render_system = root.getRenderSystem();
		if (render_system == nullptr || render_system->getName() != "Direct3D11 Rendering Subsystem")
			return false;

		auto& direct3d_system = *static_cast<Ogre::D3D11RenderSystem*>(render_system);
		Ogre::D3D11Device& device = direct3d_system._getDevice();

		if(!ImGui_ImplDX11_Init(device.get(), device.GetImmediateContext()))
			return false;

		return true;
	}

	void shutdown_imgui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}
}
