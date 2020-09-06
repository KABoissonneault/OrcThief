#pragma once

#include "egfx/node/mesh.h"
#include "object.h"

#include "Ogre/Mesh2.h"

namespace ot::egfx::node
{
	void init_mesh(mesh& smesh, Ogre::SceneNode* snode, Ogre::MeshPtr mptr) noexcept;
	Ogre::MeshPtr const& get_mesh_ptr(mesh const& smesh) noexcept;
	Ogre::MeshPtr& get_mesh_ptr(mesh& smesh) noexcept;
	Ogre::MeshPtr&& get_mesh_ptr(mesh&& smesh) noexcept;
}
