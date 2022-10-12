#include "scene/scene.h"

#include "Ogre/PlatformInformation.h"

#include "egfx/module.h"
#include "egfx/mesh_definition.h"
#include "egfx/immediate.h"
#include "egfx/object/camera.h"
#include "egfx/node/light.h"

#include "config.h"

#include <entt/entt.hpp>

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

		struct directional_light
		{
			egfx::node::directional_light node;
		};

		struct im_mesh
		{
			egfx::mesh_definition mesh;
			math::transform_matrix transform;
		};

		struct floating_object
		{
			float time_offset = 0.f;
		};

		struct text_tag
		{
			math::vector3f displacement;
			std::string text;
		};
	}

	scene::scene(egfx::module& gfx_module, config const& program_config)
		: gfx(&gfx_module)
		, gfx_scene(gfx->create_scene(std::string(program_config.get_scene().get_workspace()), get_number_threads() - 1))
	{
		if (auto const maybe_ambiant = program_config.get_scene().get_ambient_light())
		{
			auto const& ambiant_light = *maybe_ambiant;
			gfx_scene.set_ambiant_light(ambiant_light.upper_hemisphere, ambiant_light.upper_hemisphere, ambiant_light.hemisphere_direction);
		}

		egfx::object::camera_ref const camera = gfx_scene.get_camera();
		camera.set_position({ 0.0f, 2.0f, 5.5f });
		camera.look_at({ 0.0f, 0.0f, 0.0f });

		entt::entity const main_light = scene_entities.emplace_back(scene_registry.create());
		directional_light& light_component = scene_registry.emplace<directional_light>(main_light, egfx::node::create_directional_light(gfx_scene.get_root_node()));
		light_component.node.set_position({ 10.0f, 10.0f, 10.0f });
		light_component.node.set_direction(normalized(math::vector3f{ -1.0f, -1.0f, -1.0f }));

		entt::entity const cube1 = scene_entities.emplace_back(scene_registry.create());
		scene_registry.emplace<im_mesh>(cube1, make_cube()
			, math::transform_matrix::from_components(math::vector3f(0.f, 0.f, 0.f), math::rotation_matrix::identity())
			);
		scene_registry.emplace<floating_object>(cube1);
		scene_registry.emplace<text_tag>(cube1, math::vector3f(0.0f, 1.0f, 0.0f), "Hello, World!");

		entt::entity const cube2 = scene_entities.emplace_back(scene_registry.create());
		scene_registry.emplace<im_mesh>(cube2, make_cube()
			, math::transform_matrix::from_components(math::vector3f(-2.f, 0.f, 0.f), math::rotation_matrix::identity())
			);
		
		entt::entity const cube3 = scene_entities.emplace_back(scene_registry.create());
		scene_registry.emplace<im_mesh>(cube3, make_cube()
			, math::transform_matrix::from_components(math::vector3f(2.f, 0.f, 0.f), math::rotation_matrix::identity())
			);
		scene_registry.emplace<floating_object>(cube3, 1.f);
	}

	void scene::update(math::seconds dt)
	{
		scene_registry.view<im_mesh, floating_object>().each([dt](auto, im_mesh& mesh, floating_object& floater)
		{
			float const increment = 3.1415f * 0.5f * dt.count();
			mesh.transform.rotate_y(increment);

			floater.time_offset += dt.count();

			math::vector3f pos = mesh.transform.get_displacement();
			pos.y = std::sinf(floater.time_offset);
			mesh.transform.set_displacement(pos);
		});
	}

	void scene::render()
	{
		scene_registry.view<im_mesh>().each([](auto, im_mesh const& mesh)
		{
			egfx::im::draw_mesh(mesh.mesh, mesh.transform, egfx::color{ 0.5, 0.5, 0.5 });
			egfx::im::draw_wiremesh(mesh.mesh, mesh.transform);
		});
		
		scene_registry.view<im_mesh, text_tag>().each([](auto, im_mesh const& mesh, text_tag const& text)
		{
			auto const text_position = mesh.transform.get_displacement() + text.displacement;
			Im3d::Text(text_position, 2.f, Im3d::Color_Green, Im3d::TextFlags_Default, text.text.c_str());
		});
		
	}

	egfx::object::camera_cref scene::get_camera() const noexcept
	{
		return gfx_scene.get_camera();
	}
}