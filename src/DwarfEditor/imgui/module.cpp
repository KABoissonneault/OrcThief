#include "module.h"

#include "config.h"

#include "platform/file_dialog.h"

#include <format>
#include <imgui.h>
#include <misc/freetype/imgui_freetype.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include <imgui_markdown.h>
#include <IconsFontAwesome5.h>
#include "Ogre/Root.h"

namespace ot::dedit::imgui
{
	static config const* program_config;

	static ImFont* H1;
	static ImFont* H2;
	static ImFont* H3;

	static std::string get_font(std::string_view font_name)
	{
		assert(program_config != nullptr);
		std::string_view const resource_root = program_config->get_core().get_resource_root();
		return std::format("{}/Font/{}", resource_root, font_name);
	}

	std::string get_doc_path(std::string_view doc_name)
	{
		assert(program_config != nullptr);
		std::string_view const resource_root = program_config->get_core().get_resource_root();
		return std::format("{}/Doc/{}", resource_root, doc_name);
	}

	bool initialize(SDL_Window& window, config const& config)
	{
		program_config = &config;

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

	bool build_fonts()
	{
		ImGuiIO& io = ImGui::GetIO();

		std::string const default_path = get_font("Cousine-Regular.ttf");
		io.Fonts->AddFontFromFileTTF(default_path.c_str(), 13.f);

		std::string const icon_font_path = get_font("fontawesome-free-5.15.1-desktop/otfs/Font Awesome 5 Free-Solid-900.otf");
		ImFontConfig c;
		c.GlyphMinAdvanceX = 13.f;
		c.MergeMode = true; // Merge with default
		static ImWchar const icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF(icon_font_path.c_str(), 13.f, &c, icon_ranges);

		H1 = io.Fonts->AddFontFromFileTTF(default_path.c_str(), 24.f);
		H2 = io.Fonts->AddFontFromFileTTF(default_path.c_str(), 18.f);
		H3 = io.Fonts->AddFontFromFileTTF(default_path.c_str(), 16.f);

		return true;
	}

	void pre_update()
	{
		ImGui_ImplSDL2_NewFrame();
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
		program_config = nullptr;
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	bool has_mouse()
	{
		return ImGui::GetIO().WantCaptureMouse;
	}

	static void link_callback(ImGui::MarkdownLinkCallbackData data)
	{
		if (!data.isImage)
		{
			std::string_view const url(data.link, data.linkLength);
			platform::open_resource(url);
		}
	}

	void initialize_markdown(ImGui::MarkdownConfig& config)
	{
		config.linkCallback = link_callback;

		config.tooltipCallback = nullptr;

		config.headingFormats[0] = { H1, true };
		config.headingFormats[1] = { H2, true };
		config.headingFormats[2] = { H3, false };
	}
}
