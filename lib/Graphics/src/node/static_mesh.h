#pragma once

#include "graphics/node/static_mesh.h"

#include "Ogre/Mesh2.h"

namespace ot::graphics::node
{
	void init_static_mesh(static_mesh& smesh, Ogre::SceneNode* snode, Ogre::MeshPtr mptr);
	Ogre::MeshPtr& get_mesh_ptr(static_mesh& smesh);
}
