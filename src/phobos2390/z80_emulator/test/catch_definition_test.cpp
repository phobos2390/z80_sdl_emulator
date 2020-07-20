/// @file phobos2390/test_catch_main/catch_definition_test.cpp
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

TEST_CASE( "catch_definition_test", "boilerplate" )
{
    REQUIRE(1 == 1);
}
