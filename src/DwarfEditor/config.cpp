#include "config.h"

#include <charconv>
#include <filesystem>

namespace ot::dedit
{
	namespace
	{
		std::error_code parse_float(float& value, std::string_view& line)
		{
			while (std::isspace(line.front())) line.remove_prefix(1);
			std::from_chars_result const result = std::from_chars(std::to_address(line.begin()), std::to_address(line.end()), value);
			std::error_code const error = std::make_error_code(result.ec);
			if (error)
				return error;
			line.remove_prefix(std::distance(std::to_address(line.begin()), result.ptr));
			return {};
		}

		std::error_code parse_colour_value(egfx::color& color, std::string_view& line)
		{
			while (std::isspace(line.front())) line.remove_prefix(1);
			if (!line.starts_with('('))
				return std::make_error_code(std::errc::invalid_argument);
			line.remove_prefix(1);

			auto const parse_component = [&line] (float& value) -> std::error_code
			{
				return parse_float(value, line);
			};

			std::error_code error;

			error = parse_component(color.r);
			if (error)
				return error;

			error = parse_component(color.g);
			if (error)
				return error;

			error = parse_component(color.b);
			if (error)
				return error;

			while (std::isspace(line.front())) line.remove_prefix(1);
			if (!line.starts_with(')'))
				return std::make_error_code(std::errc::invalid_argument);
			line.remove_prefix(1);

			color.a = 1.0f;

			return {};
		}

		std::error_code parse_vector3(math::vector3f& vector, std::string_view& line)
		{
			while (std::isspace(line.front())) line.remove_prefix(1);
			if (!line.starts_with('('))
				return std::make_error_code(std::errc::invalid_argument);
			line.remove_prefix(1);

			auto const parse_component = [&line](float& value) -> std::error_code
			{
				return parse_float(value, line);
			};

			std::error_code error;

			error = parse_component(vector.x);
			if (error)
				return error;

			error = parse_component(vector.y);
			if (error)
				return error;

			error = parse_component(vector.z);
			if (error)
				return error;

			while (std::isspace(line.front())) line.remove_prefix(1);
			if (!line.starts_with(')'))
				return std::make_error_code(std::errc::invalid_argument);
			line.remove_prefix(1);

			return {};
		}

		std::error_code parse_ambiant_light(config::scene_config::ambiant_light& light, std::string_view& line)
		{
			std::error_code error;

			error = parse_colour_value(light.upper_hemisphere, line);
			if (error)
				return error;

			error = parse_colour_value(light.lower_hemisphere, line);
			if (error)
				return error;

			error = parse_vector3(light.hemisphere_direction, line);
			if (error)
				return error;

			return {};
		}
	}

	bool config::load(Ogre::ConfigFile const& editor_config, Ogre::ConfigFile const* game_config)
	{
		if (!core.load(editor_config) || !scene.load(editor_config))
			return false;

		if (game_config != nullptr)
		{
			if (!game.load(*game_config))
				return false;

			has_game = true;
		}

		return true;
	}

	bool config::core_config::load(Ogre::ConfigFile const& config)
	{
		name = config.getSetting("Name", "Core");
		if (name.empty())
		{
			std::printf("error [DwarfEditor]: Required Editor [Core] config key Name has no value");
			return false;
		}

		editor_resource_root = config.getSetting("ResourceRoot", "Core");
		if (editor_resource_root.empty())
		{
			std::printf("error [DwarfEditor]: Required Editor [Core] config key ResourceRoot has no value");
			return false;
		}

		std::filesystem::path const resource_path(editor_resource_root);
		if (!exists(resource_path) || !is_directory(resource_path))
		{
			std::printf("error [DwarfEditor]: Editor [Core] field ResourceRoot value '%s' does not refer to a valid directory", editor_resource_root.c_str());
			return false;
		}

		return true;
	}

	bool config::scene_config::load(Ogre::ConfigFile const& config)
	{
		workspace = config.getSetting("Workspace", "Scene");
		if (workspace.empty())
		{
			std::printf("error [DwarfEditor]: Required config key Workspace under [Scene] has no value");
			return false;
		}

		ambiant_light temp_light;
		if (temp_light.load(config.getSetting("AmbiantLight", "Scene")))
			light = temp_light;

		return true;
	}

	bool config::scene_config::ambiant_light::load(std::string_view line)
	{
		if (line.empty())
			return false;

		std::string_view const original_line = line;
		std::error_code const error = parse_ambiant_light(*this, line);
		if (error)
		{
			std::printf("warning [DwarfEditor]: Config key AmbiantLight under [Scene] has invalid value: %.*s", static_cast<int>(original_line.size()), original_line.data());
			return false;
		}

		return true;
	}

	bool config::game_config::load(Ogre::ConfigFile const& config)
	{
		return core.load(config);
	}

	bool config::game_config::core_config::load(Ogre::ConfigFile const& config)
	{
		name = config.getSetting("Name", "Core");
		if (name.empty())
		{
			std::printf("error [DwarfEditor]: Required Game config key Name under [Core] has no value");
			return false;
		}

		game_resource_root = config.getSetting("ResourceRoot", "Core");
		if (game_resource_root.empty())
		{
			std::printf("error [DwarfEditor]: Required Game config key ResourceRoot under [Core] has no value");
			return false;
		}

		std::filesystem::path const resource_path(game_resource_root);
		if (!exists(resource_path) || !is_directory(resource_path))
		{
			std::printf("error [DwarfEditor]: Game [Core] field ResourceRoot value '%s' does not refer to a valid directory", game_resource_root.c_str());
			return false;
		}

		return true;
	}
}
