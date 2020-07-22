#pragma once

#include "Ogre/Prerequisites.h"
#include "Ogre/Mesh2.h"

#include "math/mesh.h"

namespace ot::graphics
{
	Ogre::MeshPtr make_static_mesh(ogre::string const& name, math::mesh const& mesh);
}
