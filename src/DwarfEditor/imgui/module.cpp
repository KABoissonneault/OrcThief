#include "module.h"

#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>
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

		io.Fonts->AddFontDefault();
		bool const build_result = ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);
		if (!build_result)
			return false;

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
