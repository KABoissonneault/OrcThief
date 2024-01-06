#pragma once

#include "egfx/object/mesh.h"
#include "object.h"

#include "Ogre/Mesh2.h"

namespace ot::egfx
{
	void init_mesh(mesh& smesh, Ogre::MeshPtr mptr) noexcept;
	Ogre::MeshPtr const& get_mesh_ptr(mesh const& smesh) noexcept;
	Ogre::MeshPtr& get_mesh_ptr(mesh& smesh) noexcept;
	Ogre::MeshPtr&& get_mesh_ptr(mesh&& smesh) noexcept;

	item_ref make_item_ref(Ogre::Item&) noexcept;
	item_cref make_item_cref(Ogre::Item const&) noexcept;
	Ogre::Item& get_item(item_ref i) noexcept;
	Ogre::Item const& get_item(item_cref i) noexcept;
}
