#pragma once

#include "graphics/node/object.h"
#include "graphics/color.h"
#include "graphics/mesh_definition.fwd.h"

#include "math/line.h"
#include "math/transformation.h"

#include <span>
#include <string>

namespace ot::graphics
{
	namespace node
	{
		// Node for creating graphics manually
		// Manuals are based in sections. Each "begin" to "end" creates a new section
		// Each section has its own material and type
		class manual : public object
		{
		public:
			// Removes all sections
			void clear() noexcept;

			// Add a section of lines
			void add_lines(std::string const& datablock, std::span<math::line const> lines);

			// Add a section of points
			void add_points(std::string const& datablock, std::span<math::point3d const> points);

			// Add section of triangles for the entire mesh
			void add_mesh(std::string const& datablock, mesh_definition const& mesh, math::transformation const& t);

			// Add section of lines for the entire mesh
			void add_wiremesh(std::string const& datablock, mesh_definition const& mesh, math::transformation const& t);

			// Add a section consisting of a single face of a mesh
			void add_face(std::string const& datablock, mesh_definition const& mesh, face::id face, math::transformation const& t);
		};

		manual create_manual(object& parent);
	}
}
