#pragma once

#include "graphics/node/static_mesh.h"
#include "object.h"

#include "Ogre/Prerequisites.h"
#include "Ogre/Mesh2.h"
#include "Ogre/SceneNode.h"

#include "graphics/mesh_definition.h"

namespace ot::graphics::node
{
	void init_static_mesh(static_mesh& smesh, Ogre::SceneNode* snode, Ogre::MeshPtr mptr);
	Ogre::MeshPtr& get_mesh_ptr(static_mesh& smesh);

	static_mesh make_static_mesh(Ogre::SceneManager& scene_manager, ogre::string const& name, mesh_definition const& mesh);
	static_mesh make_wireframe_mesh(Ogre::SceneManager& scene_manager, ogre::string const& name, mesh_definition const& mesh);
}
