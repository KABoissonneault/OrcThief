#pragma once

#include "egfx/node/object.h"
#include "egfx/color.h"
#include "egfx/mesh_definition.fwd.h"

#include "math/line.h"
#include "math/transform_matrix.h"

#include <span>
#include <string>

namespace ot::egfx::node
{
	// Node for creating graphics manually
	// Manuals are based in sections. Each "begin" to "end" creates a new section
	// Each section has its own material and type
	class manual : public object
	{
		friend manual create_manual(object_ref parent);
	public:
		// Removes all sections
		void clear() noexcept;

		// Add a section of a single line
		void add_line(std::string const& datablock, math::line line);

		// Add a section of lines
		void add_lines(std::string const& datablock, std::span<math::line const> lines);

		// Add a section of points
		void add_points(std::string const& datablock, std::span<math::point3f const> points);

		// Add section of triangles for the entire mesh
		void add_mesh(std::string const& datablock, mesh_definition const& mesh, math::transform_matrix const& t);

		// Add section of lines for the entire mesh
		void add_wiremesh(std::string const& datablock, mesh_definition const& mesh, math::transform_matrix const& t);

		// Add a section consisting of a single face of a mesh
		void add_face(std::string const& datablock, face::cref face, math::transform_matrix const& t);
	};

	[[nodiscard]] manual create_manual(object_ref parent);
}
