#include "ImGui.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>
#include <ImGuizmo.h>

#include "Ogre/Root.h"

namespace ot::dedit::imgui
{
	bool initialize(SDL_Window& window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 1.0f;

		if (!ImGui_ImplSDL2_InitForD3D(&window))
			return false;

		return true;
	}

	void new_frame(SDL_Window& window)
	{
		ImGui_ImplSDL2_NewFrame(&window);
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void render()
	{
		ImGui::Render();
	}
	
	void end_frame()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui::EndFrame();
	}

	void shutdown()
	{
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}
