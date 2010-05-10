// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2009  Braden McDaniel
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

# define BOOST_TEST_MAIN
# define BOOST_TEST_MODULE uri_grammar

# include <boost/test/unit_test.hpp>
# include <uri/grammar.hpp>

using std::string;

BOOST_AUTO_TEST_CASE(basic)
{
    const string uri = "http://user@example.com:80/foo/bar?attr=val#frag";
    uri::components<string::const_iterator> c;
    uri::grammar<string::const_iterator> g(c);
    string::const_iterator pos = uri.begin();
    BOOST_CHECK(parse(pos, uri.end(), g));
}

BOOST_AUTO_TEST_CASE(absolute_uri)
{
    const string uri = "http://user@example.com:80/foo/bar?attr=val";
    uri::components<string::const_iterator> c;
    uri::grammar<string::const_iterator> g(c);
    string::const_iterator pos = uri.begin();
    BOOST_CHECK(parse(pos, uri.end(), g));
}

BOOST_AUTO_TEST_CASE(encoded_tilde)
{
    const string uri = "http://user@example.com:80/%7Euser/foo/bar?attr=val";
    uri::components<string::const_iterator> c;
    uri::grammar<string::const_iterator> g(c);
    string::const_iterator pos = uri.begin();
    BOOST_CHECK(parse(pos, uri.end(), g));
}

BOOST_AUTO_TEST_CASE(no_scheme)
{
    const string uri = "//user@example.com:80/foo/bar?attr=val#frag";
    uri::components<string::const_iterator> c;
    uri::grammar<string::const_iterator> g(c);
    string::const_iterator pos = uri.begin();
    BOOST_CHECK(parse(pos, uri.end(), g));
}
