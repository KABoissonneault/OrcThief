#include "core/float.h"

#include <catch2/catch.hpp>

TEST_CASE("float equal", "[core]")
{
	REQUIRE(ot::float_eq(0.0f, 0.0f));
	REQUIRE(ot::float_eq(1.0f, 1.0f));
	REQUIRE(ot::float_eq(FLT_MAX, FLT_MAX));
	REQUIRE(ot::float_eq(FLT_MIN, FLT_MIN));
	REQUIRE(ot::float_eq(0.0f, 0.0f, 8));
	REQUIRE(ot::float_eq(42.f, 42.f, 8));
	REQUIRE(ot::float_eq(0.1f, std::nextafter(0.1f, 1.0f)));
	REQUIRE(ot::float_eq(1.0f, std::nextafter(1.0f, 2.0f)));
	REQUIRE(ot::float_eq(1'000'000.f, std::nextafter(1'000'000.f, 2'000'000.f)));
	REQUIRE(ot::float_eq(-1.f, -1.f));
	REQUIRE(ot::float_eq(-1.f, std::nextafter(-1.f, -2.f)));


	REQUIRE(ot::float_eq(0.1f + 0.2f, 0.3f));
	REQUIRE(ot::float_eq(1.1f + 0.1f, 1.2f));
	REQUIRE(ot::float_eq(500'000.f + 100'000.f, 600'000.f));
}

TEST_CASE("float not equal", "[core]")
{
	REQUIRE(!ot::float_eq(1.0f, 2.0f));
	REQUIRE(!ot::float_eq(1.0f, 0.1f));
	REQUIRE(!ot::float_eq(0.1f, 0.2f));
	REQUIRE(!ot::float_eq(-0.1f, -0.2f));
}