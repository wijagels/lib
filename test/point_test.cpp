#include "Point.hpp"
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Point test
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(simple) { Point<3> p{}; }
