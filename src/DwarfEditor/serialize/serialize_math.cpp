#include "serialize_math.h"

namespace ot::dedit::serialize
{
	bool fwrite(math::point3f const& p, std::FILE* stream)
	{
		static_assert(sizeof(math::point3f) == 3 * sizeof(float));
		return ::fwrite(&p, sizeof(float), 3, stream) == 3;
	}

	bool fread(math::point3f& p, std::FILE* stream)
	{
		return ::fread(&p, sizeof(float), 3, stream) == 3;
	}

	bool fwrite(std::span<math::point3f const> p, std::FILE* stream)
	{
		return ::fwrite(p.data(), sizeof(math::point3f), p.size(), stream) == p.size();
	}

	bool fread(std::span<math::point3f> p, std::FILE* stream)
	{
		return ::fread(p.data(), sizeof(math::point3f), p.size(), stream) == p.size();
	}

	bool fwrite(math::vector3f const& v, std::FILE* stream)
	{
		static_assert(sizeof(math::vector3f) == 3 * sizeof(float));
		return ::fwrite(&v, sizeof(float), 3, stream) == 3;
	}

	bool fread(math::vector3f& v, std::FILE* stream)
	{
		return ::fread(&v, sizeof(float), 3, stream) == 3;
	}

	bool fwrite(std::span<math::vector3f const> v, std::FILE* stream)
	{
		return ::fwrite(v.data(), sizeof(math::vector3f), v.size(), stream) == v.size();
	}

	bool fread(std::span<math::vector3f> v, std::FILE* stream)
	{
		return ::fread(v.data(), sizeof(math::vector3f), v.size(), stream) == v.size();
	}

	bool fwrite(math::plane const& m, std::FILE* stream)
	{
		static_assert(sizeof(math::plane) == 4 * sizeof(float));
		return ::fwrite(&m, sizeof(float), 4, stream) == 4;
	}

	bool fread(math::plane& m, std::FILE* stream)
	{
		return ::fread(&m, sizeof(float), 4, stream) == 4;
	}

	bool fwrite(std::span<math::plane const> p, std::FILE* stream)
	{
		return ::fwrite(p.data(), sizeof(math::plane), p.size(), stream) == p.size();
	}

	bool fread(std::span<math::plane> p, std::FILE* stream)
	{
		return ::fread(p.data(), sizeof(math::plane), p.size(), stream) == p.size();
	}

	bool fwrite(math::quaternion const& q, std::FILE* stream)
	{
		static_assert(sizeof(math::quaternion) == 4 * sizeof(float));
		return ::fwrite(&q, sizeof(float), 4, stream) == 4;
	}

	bool fread(math::quaternion& q, std::FILE* stream)
	{
		return ::fread(&q, sizeof(float), 4, stream) == 4;
	}

	bool fwrite(std::span<math::quaternion const> q, std::FILE* stream)
	{
		return ::fwrite(&q, sizeof(math::quaternion), q.size(), stream) == q.size();
	}

	bool fread(std::span<math::quaternion> q, std::FILE* stream)
	{
		return ::fread(&q, sizeof(math::quaternion), q.size(), stream) == q.size();
	}

	static_assert(sizeof(math::point3f) == 3 * sizeof(float));

	bool fwrite(math::scales const& s, std::FILE* stream)
	{
		
		return ::fwrite(&s, sizeof(float), 3, stream) == 3;
	}

	bool fread(math::scales& s, std::FILE* stream)
	{
		return ::fread(&s, sizeof(float), 3, stream) == 3;
	}

	bool fwrite(std::span<math::scales const> s, std::FILE* stream)
	{
		return ::fwrite(s.data(), sizeof(math::scales), s.size(), stream) == s.size();
	}

	bool fread(std::span<math::scales> s, std::FILE* stream)
	{
		return ::fread(s.data(), sizeof(math::scales), s.size(), stream) == s.size();
	}
}
