#pragma once

#include "math/vector3.h"
#include "math/plane.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <cstdio>
#include <span>

namespace ot::dedit::serialize
{
	bool fwrite(math::point3f const& p, std::FILE* stream);
	bool fread(math::point3f& p, std::FILE* stream);
	bool fwrite(std::span<math::point3f const> p, std::FILE* stream);
	bool fread(std::span<math::point3f> p, std::FILE* stream);

	bool fwrite(math::vector3f const& v, std::FILE* stream);
	bool fread(math::vector3f& v, std::FILE* stream);
	bool fwrite(std::span<math::vector3f const> v, std::FILE* stream);
	bool fread(std::span<math::vector3f> v, std::FILE* stream);

	bool fwrite(math::plane const& p, std::FILE* stream);
	bool fread(math::plane& p, std::FILE* stream);
	bool fwrite(std::span<math::plane const> p, std::FILE* stream);
	bool fread(std::span<math::plane> p, std::FILE* stream);

	bool fwrite(math::quaternion const& q, std::FILE* stream);
	bool fread(math::quaternion& q, std::FILE* stream);
	bool fwrite(std::span<math::quaternion const> q, std::FILE* stream);
	bool fread(std::span<math::quaternion> q, std::FILE* stream);

	bool fwrite(math::scales const& s, std::FILE* stream);
	bool fread(math::scales& s, std::FILE* stream);
	bool fwrite(std::span<math::scales const> s, std::FILE* stream);
	bool fread(std::span<math::scales> s, std::FILE* stream);
}
