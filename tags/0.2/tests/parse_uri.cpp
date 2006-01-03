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
        scheme_specific_part_id,
        userinfo_id,
        host_id,
        port_id,
        authority_id,
        path_id,
        query_id,
        fragment_id
    };

    argp_option options[] = {
        { "scheme", scheme_id, 0, 0, "Print the scheme", 0 },
        { "scheme-specific-part", scheme_specific_part_id, 0, 0,
          "Print the scheme-specific part", 0 },
        { "userinfo", userinfo_id, 0, 0, "Print the user information", 0 },
        { "host", host_id, 0, 0, "Print the host", 0 },
        { "port", port_id, 0, 0, "Print the port", 0 },
        { "authority", authority_id, 0, 0, "Print the authority", 0 },
        { "path", path_id, 0, 0, "Print the path", 0 },
        { "query", query_id, 0, 0, "Print the query", 0 },
        { "fragment", fragment_id, 0, 0, "Print the fragment", 0 },
        { 0, 0, 0, 0, 0, 0 }
    };

    struct arguments {
        bool print_scheme, print_scheme_specific_part, print_userinfo,
            print_host, print_port, print_authority, print_path, print_query,
            print_fragment;
        std::string uri;

        arguments():
            print_scheme(false),
            print_scheme_specific_part(false),
            print_userinfo(false),
            print_host(false),
            print_port(false),
            print_authority(false),
            print_path(false),
            print_query(false),
            print_fragment(false)
        {}
    };

    class print_actions {
    public:
        struct action {
            explicit action(const bool print):
                print_(print)
            {}

            template <typename Iterator>
            void operator()(const Iterator & first,
                            const Iterator & last) const
            {
                if (this->print_) {
                    std::cout << std::string(first, last) << std::endl;
                }
            }

        private:
            bool print_;
        };

        print_actions(const bool print_scheme,
                      const bool print_scheme_specific_part,
                      const bool print_userinfo,
                      const bool print_host,
                      const bool print_port,
                      const bool print_authority,
                      const bool print_path,
                      const bool print_query,
                      const bool print_fragment):
            scheme(print_scheme),
            scheme_specific_part(print_scheme_specific_part),
            userinfo(print_userinfo),
            host(print_host),
            port(print_port),
            authority(print_authority),
            path(print_path),
            query(print_query),
            fragment(print_fragment)
        {}

        action scheme, scheme_specific_part, userinfo, host, port, authority,
                                 path, query, fragment;
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
                          arguments.print_scheme_specific_part,
                          arguments.print_userinfo,
                          arguments.print_host,
                          arguments.print_port,
                          arguments.print_authority,
                          arguments.print_path,
                          arguments.print_query,
                          arguments.print_fragment);

    if (arguments.uri.empty()) {
        cerr << argv[0] << ": required URI argument not given" << endl;
        return EXIT_FAILURE;
    }
    uri::uri_grammar<print_actions> g(actions);
    if (!parse(arguments.uri.begin(), arguments.uri.end(), g, space_p).full) {
        return EXIT_FAILURE;
    }
}

error_t parse_opt(int key, char * arg, argp_state * state)
{
    arguments & args = *static_cast<arguments *>(state->input);

    switch (key) {
    case scheme_id:
        args.print_scheme = true;
        break;
    case scheme_specific_part_id:
        args.print_scheme_specific_part = true;
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
    case authority_id:
        args.print_authority = true;
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
