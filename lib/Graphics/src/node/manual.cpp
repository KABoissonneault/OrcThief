#include "node/manual.h"

#include "mesh_definition.h"

#include "ogre_conversion.h"

#include "Ogre/SceneManager.h"

namespace ot::graphics::node
{
	namespace
	{
		Ogre::uint32 add_line_data(Ogre::ManualObject& manual_object, math::line const l, Ogre::uint32 current_index)
		{
			manual_object.position(to_ogre_vector(l.a));
			manual_object.position(to_ogre_vector(l.b));
			manual_object.line(current_index, current_index + 1);
			return current_index + 2;
		}
		
		Ogre::uint32 add_triangle(Ogre::ManualObject& manual_object, math::point3d v1, math::point3d v2, math::point3d v3, Ogre::uint32 current_index)
		{
			manual_object.position(to_ogre_vector(v1));
			manual_object.position(to_ogre_vector(v2));
			manual_object.position(to_ogre_vector(v3));
			manual_object.triangle(current_index, current_index + 1, current_index + 2);
			return current_index + 3;
		}
	}

	Ogre::ManualObject& get_manual(manual& m)
	{
		Ogre::SceneNode& manual_node = get_scene_node(m);
		Ogre::MovableObject* movable_object = manual_node.getAttachedObject(0);
		assert(movable_object != nullptr);
		assert(movable_object->getMovableType() == "ManualObject2");
		return static_cast<Ogre::ManualObject&>(*movable_object);
	}

	Ogre::ManualObject const& get_manual(manual const& m)
	{
		// getAttachObject only works on non-const...
		return get_manual(const_cast<manual&>(m));
	}

	void manual::clear() noexcept
	{
		get_manual(*this).clear();
	}

	void manual::add_line(std::string const& datablock, math::line line)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_LINE_LIST);

		manual_object.position(to_ogre_vector(line.a));
		manual_object.position(to_ogre_vector(line.b));
		manual_object.line(0, 1);

		manual_object.end();
	}

	void manual::add_lines(std::string const& datablock, std::span<math::line const> lines)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_LINE_LIST);

		Ogre::uint32 current_index = 0;
		for (math::line const l : lines)
		{
			current_index = add_line_data(manual_object, l, current_index);
		}

		manual_object.end();
	}

	void manual::add_points(std::string const& datablock, std::span<math::point3d const> points)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_POINT_LIST);
		
		Ogre::uint32 current_index = 0;
		for (math::point3d const p : points)
		{
			manual_object.position(to_ogre_vector(p));
			manual_object.index(current_index++);
		}

		manual_object.end();
	}

	void manual::add_mesh(std::string const& datablock, mesh_definition const& mesh, math::transformation const& t)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_TRIANGLE_LIST);

		Ogre::uint32 current_index = 0;
		for_each_triangle(mesh, [&current_index, &manual_object, &t](vertex::cref first_vertex, vertex::cref second_vertex, vertex::cref third_vertex)
		{
			math::point3d const p1 = transform(first_vertex.get_position(), t);
			math::point3d const p2 = transform(second_vertex.get_position(), t);
			math::point3d const p3 = transform(third_vertex.get_position(), t);
			current_index = add_triangle(manual_object, p1, p2, p3, current_index);
		});

		manual_object.end();
	}

	void manual::add_wiremesh(std::string const& datablock, mesh_definition const& mesh, math::transformation const& t)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_LINE_LIST);

		Ogre::uint32 current_index = 0;
		for (half_edge::cref const he : mesh.get_half_edges())
		{
			math::line const local_line = he.get_line();
			math::line const world_line = transform(local_line, t);
			current_index = add_line_data(manual_object, world_line, current_index);
		}

		manual_object.end();
	}

	void manual::add_face(std::string const& datablock, face::cref face, math::transformation const& t)
	{
		Ogre::ManualObject& manual_object = get_manual(*this);

		manual_object.begin(datablock, Ogre::OT_TRIANGLE_LIST);

		Ogre::uint32 current_index = 0;

		for_each_triangle(face, [&current_index, &manual_object, &t](vertex::cref first_vertex, vertex::cref second_vertex, vertex::cref third_vertex)
		{
			math::point3d const p1 = transform(first_vertex.get_position(), t);
			math::point3d const p2 = transform(second_vertex.get_position(), t);
			math::point3d const p3 = transform(third_vertex.get_position(), t);
			current_index = add_triangle(manual_object, p1, p2, p3, current_index);
		});

		manual_object.end();
	}

	manual create_manual(object_ref parent)
	{
		Ogre::SceneManager& scene_manager = *get_scene_node(parent).getCreator();
		Ogre::ManualObject* manual_object = scene_manager.createManualObject(Ogre::SCENE_DYNAMIC);
		Ogre::SceneNode* manual_node = scene_manager.getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
		manual_node->attachObject(manual_object);

		manual m;
		m.ref = make_object_ref(*manual_node);
		return m;
	}
}
