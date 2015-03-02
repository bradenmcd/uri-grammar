// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
#define BOOST_TEST_MODULE uri
#include <uri/grammar.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(ipv4)
{
    using namespace boost::spirit;

    uri::ipv4_grammar<std::string::const_iterator> g;

    const std::string good_addrs[] = {
        "127.0.0.1",
        "255.255.255.255",
        "0.0.0.255"
    };
    for (const auto & addr : good_addrs) {
        auto pos = addr.begin();
        BOOST_CHECK(qi::parse(pos, addr.end(), g));
        BOOST_CHECK(pos == addr.end());
    }

    const std::string bad_addrs[] = {
        "0.0.0.256"
    };
    for (const auto & addr : bad_addrs) {
        auto pos = addr.begin();
        BOOST_CHECK(!qi::phrase_parse(pos, addr.end(), g, qi::space)
                    || pos != addr.end());
    }
}

BOOST_AUTO_TEST_CASE(ipv6)
{
    namespace qi = boost::spirit::qi;

    uri::ipv6_grammar<std::string::const_iterator> g;

    const std::string addrs[] = {
        "0000:0000:0000:0000:0000:0000:127.0.0.1",
        "0000:0000:0000:0000:0000:0000:0000:0000",
        "0:0:0:0:0:0:0:0",
        "::0000:0000:0000:0000:0000:0000:0000",
        "0000::0000:0000:0000:0000:0000:0000",
        "::0000:0000:0000:0000:0000:0000",
        "0000::0000:0000:0000:0000:0000:0000",
        "::0000:0000:0000:0000:0000",
        "0000:0000::0000:0000:0000:0000:0000",
        "::0000:0000:0000:0000",
        "0000:0000:0000::0000:0000:0000:0000",
        "::0000:0000:0000",
        "0000:0000:0000:0000::0000:0000:0000",
        "::0000:0000",
        "0000:0000:0000:0000:0000::0000:0000",
        "::0000",
        "0000:0000:0000:0000:0000:0000::0000",
        "::",
        "0000:0000:0000:0000:0000:0000:0000::",
        "0000:0000:0000:0000:0000:0000::",
        "0000:0000:0000:0000:0000::",
        "0000:0000:0000:0000::",
        "0000:0000:0000::",
        "0000:0000::",
        "0000::"
    };

    for (const auto & addr : addrs) {
        auto pos = addr.begin();
        BOOST_CHECK(qi::parse(pos, addr.end(), g));
        BOOST_CHECK(pos == addr.end());
    }
}
