// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2009, 2010  Braden McDaniel
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

# include <uri/grammar.hpp>
# include <boost/program_options.hpp>
# include <iostream>

int main(int argc, char * argv[])
{
    using namespace boost::spirit;
    using namespace boost::program_options;
    using namespace std;

    try {
        options_description visible_opts("Allowed options");
        visible_opts.add_options()
            ("help",     "Print help message")
            ("scheme",   "Print the scheme")
            ("userinfo", "Print the user information")
            ("host",     "Print the host")
            ("port",     "Print the port")
            ("path",     "Print the path")
            ("query",    "Print the query")
            ("fragment", "Print the fragment");

        options_description hidden_opts("Hidden options");
        hidden_opts.add_options()
            ("uri", value<string>(), "URI");

        options_description all_opts;
        all_opts.add(visible_opts).add(hidden_opts);

        positional_options_description pos_desc;
        pos_desc.add("uri", 1);

        variables_map option_map;
        store(
            command_line_parser(argc, argv)
                .options(all_opts)
                .positional(pos_desc).run(),
            option_map);
        notify(option_map);

        if (option_map.count("help")) {
            cout << "Usage: parse-uri [options] URI\n"
                 << visible_opts;
            return EXIT_SUCCESS;
        }

        if (!option_map.count("uri")) {
            cerr << argv[0] << ": required URI argument not given" << endl;
            return EXIT_FAILURE;
        }
        uri::components<string::const_iterator> c;
        uri::grammar<string::const_iterator> g(c);
        const string & uri = option_map["uri"].as<string>();
        string::const_iterator pos = uri.begin();
        if (!parse(pos, uri.end(), g)) { return EXIT_FAILURE; }

        if (option_map.count("scheme")) {
            cout << string(c.scheme.begin(), c.scheme.end()) << endl;
        }
        if (option_map.count("userinfo")) {
            cout << string(c.userinfo.begin(), c.userinfo.end()) << endl;
        }
        if (option_map.count("host")) {
            cout << string(c.host.begin(), c.host.end()) << endl;
        }
        if (option_map.count("port")) {
            cout << string(c.port.begin(), c.port.end()) << endl;
        }
        if (option_map.count("path")) {
            cout << string(c.path.begin(), c.path.end()) << endl;
        }
        if (option_map.count("query")) {
            cout << string(c.query.begin(), c.query.end()) << endl;
        }
        if (option_map.count("fragment")) {
            cout << string(c.fragment.begin(), c.fragment.end()) << endl;
        }
    } catch (const exception & ex) {
        cerr << "error: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}
