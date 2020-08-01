#pragma once

#include "graphics/scene.fwd.h"
#include "graphics/node/static_mesh.fwd.h"
#include "graphics/node/light.fwd.h"
#include "core/uptr.h"

#include <string>

namespace ot::graphics
{
	class camera;
	class mesh_definition;

	[[nodiscard]] node::static_mesh make_static_mesh_node(scene& scene, std::string const& name, mesh_definition const& mesh);
	[[nodiscard]] node::directional_light make_directional_light(scene& scene);

	[[nodiscard]] camera& get_camera(scene& scene);
}
