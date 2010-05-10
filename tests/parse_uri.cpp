// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2009  Braden McDaniel
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

# include <iostream>
# include <argp.h>
# include <uri/grammar.hpp>

extern "C" {
    const char * argp_program_version = "parse-uri " PACKAGE_VERSION;
    const char * argp_program_bug_address = PACKAGE_BUGREPORT;

    error_t parse_opt(int key, char * arg, argp_state * state);
}

namespace {

    enum option_id {
        scheme_id = 128,
        userinfo_id,
        host_id,
        port_id,
        path_id,
        query_id,
        fragment_id
    };

    argp_option options[] = {
        { "scheme", scheme_id, 0, 0, "Print the scheme", 0 },
        { "userinfo", userinfo_id, 0, 0, "Print the user information", 0 },
        { "host", host_id, 0, 0, "Print the host", 0 },
        { "port", port_id, 0, 0, "Print the port", 0 },
        { "path", path_id, 0, 0, "Print the path", 0 },
        { "query", query_id, 0, 0, "Print the query", 0 },
        { "fragment", fragment_id, 0, 0, "Print the fragment", 0 },
        { 0, 0, 0, 0, 0, 0 }
    };

    struct arguments {
        bool print_scheme, print_userinfo, print_host, print_port, print_path,
            print_query, print_fragment;
        std::string uri;

        arguments():
            print_scheme(false),
            print_userinfo(false),
            print_host(false),
            print_port(false),
            print_path(false),
            print_query(false),
            print_fragment(false)
        {}
    };

    class print_actions {
    public:
        template <typename T>
        struct action {
            explicit action(const bool print):
                print_(print)
            {}

            void operator()(const T & attr,
                            boost::spirit::qi::unused_type,
                            boost::spirit::qi::unused_type) const
            {
                if (this->print_) { std::cout << attr << std::endl; }
            }

        private:
            bool print_;
        };

        print_actions(const bool print_scheme,
                      const bool print_userinfo,
                      const bool print_host,
                      const bool print_port,
                      const bool print_path,
                      const bool print_query,
                      const bool print_fragment):
            scheme(print_scheme),
            userinfo(print_userinfo),
            host(print_host),
            port(print_port),
            path(print_path),
            query(print_query),
            fragment(print_fragment)
        {}

        action<std::string> scheme, userinfo, path, query, fragment, host, port;
    };
}

int main(int argc, char * argv[])
{
    using namespace boost::spirit;
    using namespace std;

    arguments arguments;
    char args_doc[] = "URI";
    char * const doc = 0;
    argp_child * const children = 0;
    char *(*help_filter)(int key, const char * text, void * input) = 0;
    char * argp_domain = 0;
    argp argp = {
        options,
        parse_opt,
        args_doc,
        doc,
        children,
        help_filter,
        argp_domain
    };
    int uri_arg_index;
    argp_parse(&argp, argc, argv, 0, &uri_arg_index, &arguments);

    print_actions actions(arguments.print_scheme,
                          arguments.print_userinfo,
                          arguments.print_host,
                          arguments.print_port,
                          arguments.print_path,
                          arguments.print_query,
                          arguments.print_fragment);

    if (arguments.uri.empty()) {
        cerr << argv[0] << ": required URI argument not given" << endl;
        return EXIT_FAILURE;
    }
    uri::components<string::iterator> c;
    uri::grammar<string::iterator> g(c);
    string::iterator pos = arguments.uri.begin();
    if (!parse(pos, arguments.uri.end(), g)) { return EXIT_FAILURE; }

    if (arguments.print_scheme) {
        cout << string(c.scheme.begin(), c.scheme.end()) << endl;
    }
    if (arguments.print_userinfo) {
        cout << string(c.userinfo.begin(), c.userinfo.end()) << endl;
    }
    if (arguments.print_host) {
        cout << string(c.host.begin(), c.host.end()) << endl;
    }
    if (arguments.print_port) {
        cout << string(c.port.begin(), c.port.end()) << endl;
    }
    if (arguments.print_path) {
        cout << string(c.path.begin(), c.path.end()) << endl;
    }
    if (arguments.print_query) {
        cout << string(c.query.begin(), c.query.end()) << endl;
    }
    if (arguments.print_fragment) {
        cout << string(c.fragment.begin(), c.fragment.end()) << endl;
    }
}

error_t parse_opt(int key, char * arg, argp_state * state)
{
    arguments & args = *static_cast<arguments *>(state->input);

    switch (key) {
    case scheme_id:
        args.print_scheme = true;
        break;
    case userinfo_id:
        args.print_userinfo = true;
        break;
    case host_id:
        args.print_host = true;
        break;
    case port_id:
        args.print_port = true;
        break;
    case path_id:
        args.print_path = true;
        break;
    case query_id:
        args.print_query = true;
        break;
    case fragment_id:
        args.print_fragment = true;
        break;
    case ARGP_KEY_ARG:
        args.uri = arg;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
