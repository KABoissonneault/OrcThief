#include "module.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>
#include <ImGuizmo.h>
#include <im3d.h>

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

	void pre_update(SDL_Window& window)
	{
		ImGui_ImplSDL2_NewFrame(&window);
	}

	void end_frame()
	{
		ImGui::EndFrame(); // in case rendering was skipped

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void shutdown()
	{
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	bool has_mouse()
	{
		return ImGui::GetIO().WantCaptureMouse;
	}
}
