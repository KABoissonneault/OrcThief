#include <math/plane.h>

#include <catch2/catch.hpp>

TEST_CASE("plane::distance_to identity", "[math]")
{
	ot::math::plane p{ {1, 0, 0}, 1 };
	ot::math::point3d v{ 1, 0, 0 };
	REQUIRE(ot::float_eq(p.distance_to(v), 0.));
}

TEST_CASE("plane::distance_to simple", "[math]")
{
	ot::math::plane p{ {1, 0, 0}, 0 };
	ot::math::point3d v{ 1, 0, 0 };
	REQUIRE(ot::float_eq(p.distance_to(v), 1.));
}

TEST_CASE("find_intersection origin", "[math]")
{
	ot::math::plane const plane1{ {1,0,0}, 0 }, plane2{ {0,1,0}, 0 }, plane3{ {0,0,1}, 0 };
	auto const maybe_intersection = ot::math::find_intersection(plane1, plane2, plane3);

	REQUIRE(maybe_intersection);
	REQUIRE(float_eq(*maybe_intersection, {0, 0, 0}));
}

TEST_CASE("find_intersection axis", "[math]")
{
	ot::math::plane const plane1{ {1,0,0}, 1 }, plane2{ {0,1,0}, 0 }, plane3{ {0,0,1}, 0 };
	auto const maybe_intersection = ot::math::find_intersection(plane1, plane2, plane3);

	REQUIRE(maybe_intersection);
	REQUIRE(float_eq(*maybe_intersection, { 1, 0, 0 }));
}

TEST_CASE("find_intersection corner", "[math]")
{
	ot::math::plane const plane1{ {1,0,0}, 1 }, plane2{ {0,1,0}, 1 }, plane3{ {0,0,1}, 1 };
	auto const maybe_intersection = ot::math::find_intersection(plane1, plane2, plane3);

	REQUIRE(maybe_intersection);
	REQUIRE(float_eq(*maybe_intersection, { 1, 1, 1 }));
}
