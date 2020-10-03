#pragma once

#include "egfx/mesh_definition.h"
#include "egfx/color.h"

#include "math/transform_matrix.h"

// The functions in this namespace must be called between the module's "pre_update" and "render"
// Allows rendering "immediately" on top of the screen, over the scene
// "Immediate" means it will only exist on the next "render". It needs to be redrawn every frame
namespace ot::egfx::im
{
	void draw_wiremesh(mesh_definition const& m, math::transform_matrix const& t, float size = 1.f, color c = color::white());
	void draw_face(face::cref face, math::transform_matrix const& t, color c = color::white());
}
