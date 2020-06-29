#include <math/float.h>

#include <catch2/catch.hpp>

TEST_CASE("almost_equal identity", "[math]")
{
	REQUIRE(ot::math::almost_equal(0.f, 0.f));
	REQUIRE(ot::math::almost_equal(0., 0.));
}

TEST_CASE("almost_equal negative zero", "[math]")
{
	REQUIRE(ot::math::almost_equal(-0.f, 0.f));
	REQUIRE(ot::math::almost_equal(-0., 0.));
}