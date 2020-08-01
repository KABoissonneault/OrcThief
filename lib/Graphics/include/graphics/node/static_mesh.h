#pragma once

#include "graphics/node/static_mesh.fwd.h"
#include "graphics/node/object.h"

#include <string>

namespace ot::graphics
{
	class scene;
	class mesh_definition;

	namespace node
	{
		[[nodiscard]] static_mesh create_static_mesh(scene& scene, object& parent, std::string const& name, mesh_definition const& mesh);
		[[nodiscard]] static_mesh create_static_wireframe_mesh(scene& scene, object& parent, std::string const& name, mesh_definition const& mesh);
	}
}
