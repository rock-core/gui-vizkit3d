#ifndef _SIMPLE_TEST_H_
#define _SIMPLE_TEST_H_


// In general you should check boost testing website if you require some help
// http://www.boost.org/docs/libs/1.44_0/libs/test/html/index.html ( or more recent versions)
//
// This file intends to just briefly remind you of some of the possible setups

#include <typeinfo>
#include <boost/mpl/list.hpp>

BOOST_AUTO_TEST_SUITE(simple_test_suite)

typedef boost::mpl::list<char, short, unsigned short, unsigned int, int, long, float, double, std::string> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(simple_template_test, T, test_types)
{
	T value;
	std::string id = typeid(T).name();
	BOOST_TEST_MESSAGE("Executing simple template test with type: " << id);
	BOOST_REQUIRE(1 == 1);
}

BOOST_AUTO_TEST_CASE(simple_test)
{
	BOOST_TEST_MESSAGE("Executing simple test");

	// You can use the following setups: 
	// BOOST_CHECK_MESSAGE(1 == 1, "Send test sucessfully");
	// BOOST_CHECK( 1 == 1);
	// BOOST_CHECK_EQUAL(1, 1);
	// BOOST_WARN( 1 > 2);
	// BOOST_REQUIRE_EQUAL(1,2);
	// BOOST_FAIL("Should never reach the line");
	// BOOST_ERROR("Some error");
 
	// Using the BOOST_TEST_* macro requires the log level to be
	// set, i.e. the active log level needs to be set to equal or below "message"-level
	//
	// In order to see the output, call the unitest executable with arguments : '--log_level=message'	
	// Log levels available are:
	// all, success, test_suite, unit_scope, message, warning, error, cpp_exception,
	// system_error, fatal_error, nothing
	//
	// BOOST_TEST_CHECKPOINT("Checkpoint message");
	// 
	
}


BOOST_AUTO_TEST_SUITE_END()
#endif

