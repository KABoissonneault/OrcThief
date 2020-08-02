#pragma once

#include "graphics/scene.fwd.h"
#include "graphics/node/object.fwd.h"

#include "math/unit/time.h"

#include "core/uptr.h"

namespace ot::graphics
{
	class scene_impl;
	class camera;

	class scene
	{
		uptr<scene_impl, fwd_delete<scene_impl>> pimpl;

		friend void init_scene(scene&, uptr<scene_impl, fwd_delete<scene_impl>> ptr);
		friend scene_impl& get_impl(scene&);

	public:
		[[nodiscard]] camera& get_camera() noexcept;
		[[nodiscard]] node::object& get_root_node() noexcept;
		void update(math::seconds dt);
	};	
}

extern template struct ot::fwd_delete<ot::graphics::scene_impl>;
