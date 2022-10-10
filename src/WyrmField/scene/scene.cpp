#include "scene/scene.h"

#include "Ogre/PlatformInformation.h"

#include "egfx/module.h"
#include "egfx/mesh_definition.h"
#include "egfx/immediate.h"
#include "egfx/object/camera.h"
#include "egfx/node/light.h"

#include "config.h"

#include <im3d.h>

namespace ot::wf
{
	namespace
	{
		size_t get_number_threads()
		{
			return Ogre::PlatformInformation::getNumLogicalCores();
		}

		egfx::mesh_definition make_cube()
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

			return egfx::mesh_definition(cube_planes);
		}
		static egfx::mesh_definition const cube_definition = make_cube();
	}

	scene::scene(egfx::module& gfx_module, config const& program_config)
		: gfx(&gfx_module)
		, gfx_scene(gfx->create_scene(std::string(program_config.get_scene().get_workspace()), get_number_threads() - 1))
		, cube_transform(math::transform_matrix::identity())
	{
		if (auto const maybe_ambiant = program_config.get_scene().get_ambient_light())
		{
			auto const& ambiant_light = *maybe_ambiant;
			gfx_scene.set_ambiant_light(ambiant_light.upper_hemisphere, ambiant_light.upper_hemisphere, ambiant_light.hemisphere_direction);
		}

		egfx::object::camera_ref const camera = gfx_scene.get_camera();
		camera.set_position({ 0.0f, 2.0f, 5.5f });
		camera.look_at({ 0.0f, 0.0f, 0.0f });

		main_light = egfx::node::create_directional_light(gfx_scene.get_root_node());
		main_light.set_position({ 10.0f, 10.0f, 10.0f });
		main_light.set_direction(normalized(math::vector3f{ -1.0f, -1.0f, -1.0f }));
	}

	void scene::update(math::seconds dt)
	{
		float const increment = 3.1415f * 0.5f * dt.count();
		cube_transform.rotate_y(increment);

		y_buffer += dt.count();

		math::vector3f pos = cube_transform.get_displacement();
		pos.y = std::sinf(y_buffer);
		cube_transform.set_displacement(pos);
	}

	void scene::render()
	{
		egfx::im::draw_mesh(cube_definition, cube_transform, egfx::color{ 0.5, 0.5, 0.5 });
		egfx::im::draw_wiremesh(cube_definition, cube_transform);

		auto const text_position = cube_transform.get_displacement() + math::vector3f(0.0f, 1.0f, 0.0f);
		Im3d::Text(text_position, 2.f, Im3d::Color_Green, Im3d::TextFlags_Default, "Hello, World!");
	}

	egfx::object::camera_cref scene::get_camera() const noexcept
	{
		return gfx_scene.get_camera();
	}
}