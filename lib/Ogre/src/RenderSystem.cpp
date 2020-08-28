#include "Ogre/RenderSystem.h"

#include "Ogre/Prerequisites.h"

namespace ot::ogre::render_system
{
	bool get_fullscreen(config_option_map const& config, bool& result)
	{
		static const string fullscreen("Full Screen");

		auto const found = config.find(fullscreen);
		if (found == config.end())
		{
			return false;
		}

		bool error;
		result = Ogre::StringConverter::parseBool(found->second.currentValue, false, &error);
		return !error;
	}

	bool get_resolution(config_option_map const& config, int& width, int& height)
	{
		static const string video_mode("Video Mode");

		auto const found = config.find(video_mode);
		if (found == config.end())
		{
			return false;
		}

		Ogre::String const& value = found->second.currentValue;
		auto const width_start = value.find_first_of("0123456789");
		auto const width_end = value.find(' ', width_start);
		auto const height_start = width_end + 3; 
		auto const height_end = value.find(' ', height_start);

		width = Ogre::StringConverter::parseInt(value.substr(width_start, width_end - width_start), -1);
		height = Ogre::StringConverter::parseInt(value.substr(height_start, height_end - height_start), -1);

		if (width < 0 || height < 0)
		{
			return false;
		}
		
		return true;
	}

	bool get_hardware_gamma_conversion(config_option_map const& config, bool& result)
	{
		static const string gamma("sRGB Gamma Conversion");

		auto const found = config.find(gamma);
		if (found == config.end())
		{
			return false;
		}

		bool error;
		result = Ogre::StringConverter::parseBool(found->second.currentValue, false, &error);
		return !error;
	}

	bool get_vsync(config_option_map const& config, bool& result)
	{
		static const string vsync("VSync");

		auto const found = config.find(vsync);
		if (found == config.end())
		{
			return false;
		}

		bool error;
		result = Ogre::StringConverter::parseBool(found->second.currentValue, false, &error);
		return !error;
	}

	namespace d3d11
	{
		std::string const name("Direct3D11 Rendering Subsystem");

		namespace attribute
		{
			Ogre::IdString const device("D3DDEVICE");
		}
	}
}
