// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2006  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# include <boost/array.hpp>
# include <boost/test/unit_test.hpp>
# include <uri/grammar.hpp>

using namespace uri;

void basic()
{
    using boost::spirit::space_p;

    grammar<> g;
    const std::string uri = "http://user@example.com:80/foo/bar?attr=val#frag";
    BOOST_CHECK(parse(uri.begin(), uri.end(), g, space_p).full);
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("uri_grammar");
    suite->add(BOOST_TEST_CASE(&basic));
    return suite;
}
