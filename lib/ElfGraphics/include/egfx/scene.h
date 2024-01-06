#pragma once

#include "egfx/scene.fwd.h"
#include "egfx/node.fwd.h"
#include "egfx/object/object.fwd.h"
#include "egfx/object/camera.fwd.h"
#include "egfx/color.h"

#include "math/ray.h"
#include "math/unit/time.h"

#include "core/uptr.h"

#include <vector>

namespace ot::egfx
{
	class scene_impl;

	class scene
	{
		uptr<scene_impl, fwd_delete<scene_impl>> pimpl;

		friend void init_scene(scene&, uptr<scene_impl, fwd_delete<scene_impl>> ptr);
		friend scene_impl& get_impl(scene&);
		friend scene_impl const& get_impl(scene const&);

	public:
		// Returns the main camera the scene is rendered to 
		[[nodiscard]] camera_ref get_camera() noexcept;
		[[nodiscard]] camera_cref get_camera() const noexcept;
		[[nodiscard]] node_ref get_root_node() noexcept;

		void update(math::seconds dt);

		void set_ambiant_light(color upper_hemisphere, color lower_hemisphere, math::vector3f direction);

		// Casts the ray in the scene, and detects the nearest object
		std::vector<object_id> raycast_objects(math::ray r) const;
	};	
}

extern template struct ot::fwd_delete<ot::egfx::scene_impl>;
