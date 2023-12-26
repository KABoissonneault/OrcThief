#include "basic_mesh_repo.h"

#include "math/plane.h"

#include "egfx/mesh_definition.h"

namespace ot::dedit
{
	namespace
	{
		math::plane const cube_planes[6] =
		{
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		auto const sqrt_half = 0.70710678118654752440084436210485f;
		math::plane const octagon_planes[] =
		{
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 0, 1}, 0.5},
			{{0, 0, -1}, 0.5},
			{{sqrt_half, 0, sqrt_half}, 0.5},
			{{sqrt_half, 0, -sqrt_half}, 0.5},
			{{-sqrt_half, 0, sqrt_half}, 0.5},
			{{-sqrt_half, 0, -sqrt_half}, 0.5},
		};

		auto const sqrt3_half = 0.86602540378f;
		math::plane const hex_planes[] =
		{
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0.5, 0, sqrt3_half}, 0.5},
			{{-0.5, 0, sqrt3_half}, 0.5},
			{{0.5, 0, -sqrt3_half}, 0.5},
			{{-0.5, 0, -sqrt3_half}, 0.5},
		};

		math::plane const tri_planes[] =
		{
			{{sqrt3_half, 0, 0.5}, 0.5},
			{{-sqrt3_half, 0, 0.5}, 0.5},
			{{0, 0, -1}, 0.5},
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
		};

		math::plane const pyramid_planes[] =
		{
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};
	}

	basic_mesh_repo::basic_mesh_repo()
		: cube(std::make_shared<egfx::mesh_definition>(cube_planes))
		, octagonal_prism(std::make_shared<egfx::mesh_definition>(octagon_planes))
		, hex_prism(std::make_shared<egfx::mesh_definition>(hex_planes))
		, tri_prism(std::make_shared<egfx::mesh_definition>(tri_planes))
		, square_pyramid(std::make_shared<egfx::mesh_definition>(pyramid_planes))
	{

	}
}
