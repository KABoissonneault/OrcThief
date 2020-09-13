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

			std::string_view get_resource_root() const noexcept 
			{
				return resource_root;
			}

		private:
			std::string resource_root;
		};

		class scene_config
		{
		public:
			[[nodiscard]] bool load(Ogre::ConfigFile const& config);

			struct ambiant_light
			{
				egfx::color upper_hemisphere;
				egfx::color lower_hemisphere;
				math::vector3d hemisphere_direction;

				[[nodiscard]] bool load(std::string_view value);
			};

			std::string_view get_workspace() const noexcept
			{
				return workspace;
			}
			std::optional<ambiant_light> get_ambient_light() const noexcept { return light; }

		private:
			std::string workspace;
			std::optional<ambiant_light> light;
		};

		core_config const& get_core() const noexcept { return core; }
		scene_config const& get_scene() const noexcept { return scene; }

		[[nodiscard]] bool load(Ogre::ConfigFile const& config);

	private:
		core_config core;
		scene_config scene;
	};
}