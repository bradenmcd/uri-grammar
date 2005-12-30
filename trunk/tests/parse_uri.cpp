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
# include <boost/program_options.hpp>
# include <uri/grammar.hpp>

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

    action scheme, scheme_specific_part, userinfo, host, port, authority, path,
                             query, fragment;
};

int main(int argc, char * argv[])
try {
    using namespace boost::program_options;
    using namespace boost::spirit;
    using namespace std;

    options_description desc("Options");
    desc.add_options()
        ("help,h", "display this help and exit")
        ("scheme", "print scheme of URI")
        ("scheme-specific-part", "print scheme-specific part of URI")
        ("userinfo", "print userinfo of URI")
        ("host", "print host of URI")
        ("port", "print port of URI")
        ("authority", "print authority of URI")
        ("path", "print path of URI")
        ("query", "print query of URI")
        ("fragment", "print fragment of URI")
        ;
    parsed_options parsed = parse_command_line(argc, argv, desc);
    variables_map vm;
    store(parsed, vm);
    notify(vm);
    if (vm.count("help")) {
        cout << "Usage: " << argv[0] << " [OPTION]... URI" << endl
             << "Print the different parts of URI." << endl
             << endl
             << desc
             << endl
             << "Report bugs to <" << PACKAGE_BUGREPORT << '>' << endl;
        return EXIT_SUCCESS;
    }

    print_actions actions(vm.count("scheme"),
                          vm.count("scheme-specific-part"),
                          vm.count("userinfo"),
                          vm.count("host"),
                          vm.count("port"),
                          vm.count("authority"),
                          vm.count("path"),
                          vm.count("query"),
                          vm.count("fragment"));

    const string uri = argv[parsed.options.size()];
    uri::grammar<print_actions> g(actions);
    if (!parse(uri.begin(), uri.end(), g, space_p).full) {
        return EXIT_FAILURE;
    }
} catch (boost::program_options::unknown_option & ex) {
    std::cerr << argv[0] << ": " << ex.what() << std::endl;
    return EXIT_FAILURE;
}
