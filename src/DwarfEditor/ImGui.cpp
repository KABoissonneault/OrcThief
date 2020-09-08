#include "ImGui.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>

#include "Ogre/Root.h"

namespace ot::dedit
{
	bool initialize_imgui(SDL_Window& window)
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

	void shutdown_imgui()
	{
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}