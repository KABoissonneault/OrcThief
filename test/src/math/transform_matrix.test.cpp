#include "math/transform_matrix.h"

#include <catch2/catch.hpp>

#include <numbers>

using ot::float_eq;

TEST_CASE("rotation_matrix identity", "[math]")
{
	auto const mat = ot::math::rotation_matrix::identity();
	REQUIRE(float_eq(mat.determinant(), 1.0f));
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			if (i == j)
			{
				REQUIRE(mat.elements[i * 3 + j] == 1.f);
			}
			else
			{
				REQUIRE(mat.elements[i * 3 + j] == 0.f);
			}
		}
	}
	REQUIRE(float_eq(mat, invert(mat)));
}

TEST_CASE("rotation_matrix rotx", "[math]")
{
	auto const mat = ot::math::rotation_matrix::rotx(std::numbers::pi_v<float>);
	ot::math::vector3f const value{ 0, 1, 0 };
	REQUIRE(float_eq(rotate(value, mat), ot::math::vector3f{ 0, -1, 0 }));
	REQUIRE(float_eq(mat * mat, ot::math::rotation_matrix::rotx(std::numbers::pi_v<float> * 2.f)));
}

TEST_CASE("rotation_matrix invert", "[math]")
{
	auto const mat = ot::math::rotation_matrix::rotx(std::numbers::pi_v<float>);
	auto const inv = invert(mat);
	REQUIRE(float_eq(mat * inv, ot::math::rotation_matrix::identity()));
}

TEST_CASE("transform_matrix identity", "[math]")
{
	auto const mat = ot::math::transform_matrix::identity();
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			if (i == j)
			{
				REQUIRE(mat.elements[i * 4 + j] == 1.f);
			} else
			{
				REQUIRE(mat.elements[i * 4 + j] == 0.f);
			}
		}
	}
	REQUIRE(float_eq(mat, invert(mat)));
}

TEST_CASE("transform_matrix", "[math]")
{
	auto const rot = ot::math::rotation_matrix::rotx(std::numbers::pi_v<float> / 3.f);
	ot::math::vector3f dis{ 2.f, 5.f, 0.f };
	auto const mat = ot::math::transform_matrix::from_components(dis, rot, 1.f);
	auto const inv = invert(mat);

	REQUIRE(float_eq(mat.get_displacement(), dis));
	REQUIRE(float_eq(mat.get_rotation(), rot));
	REQUIRE(float_eq(mat.get_scale(), 1.f));
	REQUIRE(float_eq(mat * inv, ot::math::transform_matrix::identity()));
}
