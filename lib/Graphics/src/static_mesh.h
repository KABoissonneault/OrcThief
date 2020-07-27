#pragma once

#include "Ogre/Prerequisites.h"
#include "Ogre/Mesh2.h"

#include "graphics/mesh_definition.h"

namespace ot::graphics
{
	Ogre::MeshPtr make_static_mesh(ogre::string const& name, mesh_definition const& mesh);
	Ogre::MeshPtr make_wireframe_mesh(ogre::string const& name, mesh_definition const& mesh);
}
