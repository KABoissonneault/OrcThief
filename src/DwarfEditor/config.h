#pragma once

#include <string>
#include <optional>

#include "Ogre/ConfigFile.h"

#include "egfx/color.h"
#include "math/vector3.h"

#include <string_view>

namespace ot::dedit
{
	class config
	{
	public:
		class core_config
		{
		public:
			[[nodiscard]] bool load(Ogre::ConfigFile const& config);

			[[nodiscard]] std::string_view get_name() const noexcept
			{
				return name;
			}

			[[nodiscard]] std::string_view get_editor_resource_root() const noexcept
			{
				return editor_resource_root;
			}

		private:
			std::string name;
			std::string editor_resource_root;
		};

		class scene_config
		{
		public:
			[[nodiscard]] bool load(Ogre::ConfigFile const& config);

			struct ambiant_light
			{
				egfx::color upper_hemisphere;
				egfx::color lower_hemisphere;
				math::vector3f hemisphere_direction;

				[[nodiscard]] bool load(std::string_view value);
			};

			[[nodiscard]] std::string_view get_workspace() const noexcept
			{
				return workspace;
			}
			[[nodiscard]] std::optional<ambiant_light> get_ambient_light() const noexcept { return light; }

		private:
			std::string workspace;
			std::optional<ambiant_light> light;
		};

		class game_config
		{
		public:
			class core_config
			{
			public:
				[[nodiscard]] bool load(Ogre::ConfigFile const& config);

				[[nodiscard]] std::string_view get_name() const noexcept
				{
					return name;
				}

				[[nodiscard]] std::string_view get_game_resource_root() const noexcept
				{
					return game_resource_root;
				}

			private:
				std::string name;
				std::string game_resource_root;
			};

			core_config const& get_core() const noexcept { return core; }

			[[nodiscard]] bool load(Ogre::ConfigFile const& config);

		private:
			core_config core;
		};

		[[nodiscard]] core_config const& get_core() const noexcept { return core; }
		[[nodiscard]] scene_config const& get_scene() const noexcept { return scene; }
		[[nodiscard]] bool has_game_config() const noexcept { return has_game; }
		[[nodiscard]] game_config const& get_game() const noexcept { return game; }

		[[nodiscard]] bool load(Ogre::ConfigFile const& editor_config, Ogre::ConfigFile const* game_config);

	private:
		core_config core;
		scene_config scene;
		game_config game;

		bool has_game = false;
	};
}