#include "map_handler.h"

#include "application.h"
#include "console.h"
#include "platform/file_dialog.h"
#include "serialize/serialize_map.h"

#include <fmt/format.h>

namespace ot::dedit
{
	template<typename Application>
	void map_handler<Application>::new_map()
	{
		derived& app = static_cast<derived&>(*this);
		map& m = app.get_current_map();

		// TODO: confirmation on dirty map
		m.clear();
		app.map_path.clear();

		console::log("Starting new map");
	}

	template<typename Application>
	void map_handler<Application>::open_map()
	{
		derived& app = static_cast<derived&>(*this);

		// TODO: confirmation on dirty map
		platform::open_file_dialog(".dem", [&app](std::error_code ec, std::string file_path)
		{
			map& m = app.get_current_map();

			if (ec)
			{
				if (ec != std::errc::operation_canceled)
					console::error(fmt::format("Open file failed ({})", ec.message()));
				return;
			}

			m.clear();
			app.map_path.clear();

			std::FILE* file = std::fopen(file_path.c_str(), "r");
			assert(file != nullptr);
			if (!serialize::fread(m, file))
			{
				m.clear();
				console::error(fmt::format("Failed loading map '{}'", file_path));
			} 
			else
			{

				app.map_path = std::move(file_path);
				console::log(fmt::format("Opened map '{}'", app.map_path));
			}
			std::fclose(file);
		});
	}

	template<typename Application>
	void map_handler<Application>::save_map()
	{
		derived& app = static_cast<derived&>(*this);
		map& m = app.get_current_map();

		if (app.map_path.empty())
		{
			save_map_as();
		} 
		else
		{
			std::FILE* file = std::fopen(app.map_path.c_str(), "w");
			if (file == nullptr)
			{
				console::error(fmt::format("Could not open '{}' for writing", app.map_path));
				return;
			}

			if (!serialize::fwrite(m, file))
			{
				console::error(fmt::format("Failed to save map '{}'", app.map_path));
			} 
			else
			{
				console::log(fmt::format("Saved map '{}'", app.map_path));
			}
			std::fclose(file);
		}
	}

	template<typename Application>
	void map_handler<Application>::save_map_as()
	{
		derived& app = static_cast<derived&>(*this);

		platform::file_type file_type;
		file_type.name = "DwarfEditor Map";
		std::string_view const extension = ".dem";
		file_type.extensions = std::span<std::string_view const>(&extension, 1);
		platform::save_file_dialog(file_type, [&app](std::error_code ec, std::string file_path)
		{
			map& m = app.get_current_map();

			if (ec)
			{
				if (ec != std::errc::operation_canceled)
					console::error(fmt::format("Save file failed ({})", ec.message()));
				return;
			}

			std::FILE* file = std::fopen(file_path.c_str(), "w");
			if (file == nullptr)
			{
				console::error(fmt::format("Could not open '{}' for writing", file_path));
				return;
			}

			if (!serialize::fwrite(m, file))
			{
				console::error(fmt::format("Failed to save map as '{}'", file_path));
			} 
			else
			{
				app.map_path = std::move(file_path);
				console::log(fmt::format("Saved map as '{}'", app.map_path));
			}
			std::fclose(file);
		});
	}

	template class map_handler<application>;
}
