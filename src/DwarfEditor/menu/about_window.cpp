#include "about_window.h"

#include "imgui/module.h"

#include <string>
#include <fstream>

#include <imgui.h>
#include <imgui_markdown.h>

namespace ot::dedit::about_window
{
	static std::string about_content;
	static bool content_loaded;

	void load_content()
	{
		unload_content();

		std::string const doc_path = imgui::get_doc_path("about.md");
		std::ifstream doc_file(doc_path);
		if (!doc_file)
			return;

		doc_file.seekg(0, std::ios::end);
		about_content.resize(doc_file.tellg());
		doc_file.seekg(0, std::ios::beg);
		doc_file.read(about_content.data(), about_content.size());

		content_loaded = true;
	}

	void unload_content()
	{
		about_content.clear();
		content_loaded = false;
	}

	bool has_content()
	{
		return content_loaded;
	}

	const ImVec2 k_default_size(640, 480);

	void draw(bool* open)
	{
		ImGui::SetNextWindowSize(k_default_size, ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("About Dwarf Editor", open))
		{
			ImGui::End();
			return;
		}

		ImGui::MarkdownConfig config;
		imgui::initialize_markdown(config);

		ImGui::Markdown(about_content.data(), about_content.size(), config);

		ImGui::End();
	}
}
