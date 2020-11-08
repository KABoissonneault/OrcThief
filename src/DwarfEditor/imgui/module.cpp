#include "module.h"

#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>
#include <im3d.h>
#include <IconsFontAwesome5.h>

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

	bool build_fonts(std::filesystem::path const& resource_folder)
	{
		ImGuiIO& io = ImGui::GetIO();

		std::string const default_path = (resource_folder / "Font/ProggyClean.ttf").string();
		io.Fonts->AddFontFromFileTTF(default_path.c_str(), 13.f);

		std::string const icon_font_path = (resource_folder / "Font/fontawesome-free-5.15.1-desktop/otfs/Font Awesome 5 Free-Solid-900.otf").string();
		ImFontConfig c;
		c.GlyphMinAdvanceX = 13.f;
		c.MergeMode = true; // Merge with default
		static ImWchar const icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF(icon_font_path.c_str(), 13.f, &c, icon_ranges);

		return ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);
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
