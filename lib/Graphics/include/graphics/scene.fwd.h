#pragma once

#include "core/uptr.h"

namespace ot::graphics
{
	class scene_impl;

	class scene
	{
		uptr<scene_impl, fwd_delete<scene_impl>> pimpl;

		friend void init_scene(scene&, uptr<scene_impl, fwd_delete<scene_impl>> ptr);
		friend scene_impl& get_impl(scene&);
	};
}

extern template struct ot::fwd_delete<ot::graphics::scene_impl>;
