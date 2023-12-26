#pragma once

#include "egfx/mesh_definition.fwd.h"

#include <memory>

namespace ot::dedit
{	
	class basic_mesh_repo
	{
		std::shared_ptr<egfx::mesh_definition const> cube;
		std::shared_ptr<egfx::mesh_definition const> octagonal_prism;
		std::shared_ptr<egfx::mesh_definition const> hex_prism;
		std::shared_ptr<egfx::mesh_definition const> tri_prism;
		std::shared_ptr<egfx::mesh_definition const> square_pyramid;

	public:
		basic_mesh_repo();

		std::shared_ptr<egfx::mesh_definition const> get_cube() const noexcept { return cube; }
		std::shared_ptr<egfx::mesh_definition const> get_octagonal_prism() const noexcept { return octagonal_prism; }
		std::shared_ptr<egfx::mesh_definition const> get_hex_prism() const noexcept { return hex_prism; }
		std::shared_ptr<egfx::mesh_definition const> get_tri_prism() const noexcept { return tri_prism; }
		std::shared_ptr<egfx::mesh_definition const> get_square_pyramid() const noexcept { return square_pyramid; }
	};
}