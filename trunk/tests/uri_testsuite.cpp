// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2009  Braden McDaniel
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

# include <boost/test/unit_test.hpp>
# include <uri/grammar.hpp>

using namespace uri;

void basic()
{
    using boost::spirit::space_p;

    uri_grammar<> g;
    const std::string uri = "http://user@example.com:80/foo/bar?attr=val#frag";
    BOOST_CHECK(parse(uri.begin(), uri.end(), g, space_p).full);
}

void absolute_uri()
{
    using boost::spirit::space_p;

    absolute_uri_grammar<> g;
    const std::string uri = "http://user@example.com:80/foo/bar?attr=val";
    BOOST_CHECK(parse(uri.begin(), uri.end(), g, space_p).full);
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("uri_grammar");
    suite->add(BOOST_TEST_CASE(&basic));
    suite->add(BOOST_TEST_CASE(&absolute_uri));
    return suite;
}
