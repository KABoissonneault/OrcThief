#include "Ogre/Root.h"

namespace ot::ogre
{
	auto render_window_misc_params::set_external_window_handle(string value) -> render_window_misc_params&
	{
		static const string external_window("externalWindowHandle");
		params.insert_or_assign(external_window, std::move(value));
		return *this;
	}

	auto render_window_misc_params::set_hardware_gamma_conversion(bool value) -> render_window_misc_params&
	{
		static const string gamma("gamma");
		params.insert_or_assign(gamma, value ? "Yes" : "No");
		return *this;
	}

	auto render_window_misc_params::set_vsync(bool value) -> render_window_misc_params&
	{
		static const string vsync("vsync");
		params.insert_or_assign(vsync, value ? "Yes" : "No");
		return *this;
	}

	auto render_window_misc_params::set_reverse_depth(bool value) -> render_window_misc_params&
	{
		static const string reverse_depth("reverse_depth");
		params.insert_or_assign(reverse_depth, value ? "Yes" : "No");
		return *this;
	}
}
