#pragma once

#include "graphics/scene.fwd.h"
#include "graphics/node/object.fwd.h"

#include "math/ray.h"
#include "math/unit/time.h"

#include "core/uptr.h"

#include <optional>

namespace ot::graphics
{
	class scene_impl;
	class camera;

	class scene
	{
		uptr<scene_impl, fwd_delete<scene_impl>> pimpl;

		friend void init_scene(scene&, uptr<scene_impl, fwd_delete<scene_impl>> ptr);
		friend scene_impl& get_impl(scene&);
		friend scene_impl const& get_impl(scene const&);

	public:
		// Returns the main camera the scene is rendered to 
		[[nodiscard]] camera& get_camera() noexcept;
		[[nodiscard]] camera const& get_camera() const noexcept;
		[[nodiscard]] node::object& get_root_node() noexcept;

		void update(math::seconds dt);

		// Casts the ray in the scene, and detects the nearest object
		std::optional<node::object_id> raycast_object(math::ray r) const;
	};	
}

extern template struct ot::fwd_delete<ot::graphics::scene_impl>;
