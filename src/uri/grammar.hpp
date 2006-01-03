// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// uri_grammar
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef URI_GRAMMAR_HPP
#   define URI_GRAMMAR_HPP

#   include <boost/spirit.hpp>
#   include <boost/spirit/phoenix.hpp>

namespace uri {

    template <typename SpiritActor, typename Iterator1, typename Iterator2>
    class assign_iterators_base {
        SpiritActor actor_;
        Iterator1 begin_;
        Iterator2 end_;

    public:
        typedef assign_iterators_base<SpiritActor, Iterator1, Iterator2>
            this_type;

        assign_iterators_base(const SpiritActor & actor,
                              const Iterator1 & begin,
                              const Iterator2 & end):
            actor_(actor),
            begin_(begin),
            end_(end)
        {}

        template <typename T>
        struct result {
            typedef void type;
        };

        template <typename Tuple>
        typename phoenix::actor_result<this_type, Tuple>::type
        eval(Tuple) const
        {
            this->actor_(this->begin_(), this->end_());
        }
    };

    template <typename SpiritActor, typename Iterator1, typename Iterator2>
    inline
    phoenix::actor<assign_iterators_base<SpiritActor, Iterator1, Iterator2> >
    assign_iterators(const SpiritActor & actor,
                     const Iterator1 & begin,
                     const Iterator2 & end)
    {
        return assign_iterators_base<SpiritActor, Iterator1, Iterator2>(actor,
                                                                        begin,
                                                                        end);
    }


    class null_actions {
    public:
        struct null_action {
            template <typename Iterator>
            void operator()(const Iterator &, const Iterator &) const
            {}
        };

        null_action scheme, scheme_specific_part, userinfo, host, port,
            authority, path, query, fragment;
    };


    struct uri_reserved_parser :
        public boost::spirit::char_parser<uri_reserved_parser> {

        typedef uri_reserved_parser self_t;

        template <typename CharT>
        bool test(CharT ch) const
        {
            return ch == ';'
                || ch == '/'
                || ch == '?'
                || ch == ':'
                || ch == '@'
                || ch == '&'
                || ch == '='
                || ch == '+'
                || ch == '$'
                || ch == ',';
        }
    };

    const uri_reserved_parser uri_reserved_p = uri_reserved_parser();


    struct uri_unreserved_parser :
        public boost::spirit::char_parser<uri_unreserved_parser> {

        typedef uri_unreserved_parser self_t;

        template <typename CharT>
        bool test(CharT ch) const
        {
            using namespace std;
            return isalnum(char_traits<CharT>::to_int_type(ch))
                || ch == '-'
                || ch == '_'
                || ch == '.'
                || ch == '!'
                || ch == '~'
                || ch == '*'
                || ch == '\''
                || ch == '('
                || ch == ')';
        }
    };

    const uri_unreserved_parser uri_unreserved_p = uri_unreserved_parser();


    struct uric_grammar :
        public boost::spirit::grammar<uric_grammar> {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type uric, escaped;

            explicit definition(const uric_grammar & self)
            {
                using namespace boost::spirit;

                uric
                    =   uri_reserved_p
                    |   uri_unreserved_p
                    |   escaped
                    ;

                escaped
                    =   '%' >> xdigit_p >> xdigit_p
                    ;
            }

            const boost::spirit::rule<ScannerT> & start() const
            {
                return this->uric;
            }
        };
    };


    template <typename Actions = null_actions>
    struct uri_authority_grammar :
        public boost::spirit::grammar<uri_authority_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            struct server_closure :
                boost::spirit::closure<server_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename server_closure::member1 userinfo_begin;
                typename server_closure::member2 userinfo_end;
            };

            typedef boost::spirit::rule<ScannerT> rule_type;
            typedef boost::spirit::rule<ScannerT,
                                        typename server_closure::context_t>
                server_rule_type;

            rule_type authority;
            rule_type reg_name;
            server_rule_type server;
            rule_type userinfo;
            rule_type hostport;
            rule_type host;
            rule_type hostname;
            rule_type domainlabel;
            rule_type toplabel;
            rule_type ipv4address;
            rule_type port;
            rule_type escaped;

            explicit definition(const uri_authority_grammar & self)
            {
                using namespace boost::spirit;
                using namespace phoenix;

                authority
                    =   (server | reg_name)[ self.actions.authority ]
                    ;

                reg_name
                    =  +(   uri_unreserved_p
                        |   escaped
                        |   '$'
                        |   ','
                        |   ';'
                        |   ':'
                        |   '@'
                        |   '&'
                        |   '='
                        |   '+'
                        )
                    ;

                server
                    =  !(
                           !(
                                userinfo[
                                    server.userinfo_begin = arg1,
                                    server.userinfo_end = arg2
                                ] >> '@'
                            )[
                                assign_iterators(self.actions.userinfo,
                                                 server.userinfo_begin,
                                                 server.userinfo_end)
                            ]
                            >> hostport
                        )
                    ;

                userinfo
                    =  *(   uri_unreserved_p
                        |   escaped
                        |   ';'
                        |   ':'
                        |   '&'
                        |   '='
                        |   '+'
                        |   '$'
                        |   ','
                        )
                    ;

                hostport
                    =   host >> !(':' >> port)
                    ;

                host
                    =   (hostname | ipv4address)[ self.actions.host ]
                    ;

                hostname
                    =   *(domainlabel >> '.') >> toplabel >> !ch_p('.')
                    ;

                domainlabel
                    =   alnum_p >> *(*ch_p('-') >> alnum_p)
                    ;

                toplabel
                    =   alpha_p >> *(*ch_p('-') >> alnum_p)
                    ;

                ipv4address
                    =   +digit_p >> '.' >> +digit_p >> '.' >> +digit_p >> '.'
                        >> +digit_p
                    ;

                port
                    =   (*digit_p)[ self.actions.port ]
                    ;

                escaped
                    =   '%' >> xdigit_p >> xdigit_p
                    ;
            }

            const boost::spirit::rule<ScannerT> & start() const
            {
                return this->authority;
            }
        };

        const Actions & actions;

        explicit uri_authority_grammar(const Actions & actions = Actions()):
            actions(actions)
        {}
    };


    template <typename Actions>
    struct uri_abs_path_grammar :
        public boost::spirit::grammar<uri_abs_path_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type abs_path;
            rule_type path_segments;
            rule_type segment;
            rule_type param;
            rule_type pchar;
            rule_type query;
            rule_type escaped;

            explicit definition(const uri_abs_path_grammar & self)
            {
                using namespace boost::spirit;
                using namespace phoenix;

                abs_path
                    =   ('/' >> path_segments)[ self.actions.path ]
                    ;

                path_segments
                    =   segment >> *('/' >> segment)
                    ;

                segment
                    =   *pchar >> *(';' >> param)
                    ;

                param
                    =   *pchar
                    ;

                pchar
                    =   uri_unreserved_p
                    |   escaped
                    |   ':'
                    |   '@'
                    |   '&'
                    |   '='
                    |   '+'
                    |   '$'
                    |   ','
                    ;

                escaped
                    =   '%' >> xdigit_p >> xdigit_p
                    ;
            }

            const boost::spirit::rule<ScannerT> & start() const
            {
                return this->abs_path;
            }
        };

        const Actions & actions;

        explicit uri_abs_path_grammar(const Actions & actions = Actions()):
            actions(actions)
        {}
    };


    template <typename Actions = null_actions>
    struct absolute_uri_grammar :
        public boost::spirit::grammar<absolute_uri_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            struct absolute_uri_closure :
                boost::spirit::closure<absolute_uri_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename absolute_uri_closure::member1 scheme_begin;
                typename absolute_uri_closure::member2 scheme_end;
            };

            struct server_closure :
                boost::spirit::closure<server_closure,
                                       typename ScannerT::iterator_t,
                                       typename ScannerT::iterator_t> {
                typename server_closure::member1 userinfo_begin;
                typename server_closure::member2 userinfo_end;
            };

            typedef boost::spirit::rule<ScannerT> rule_type;
            typedef boost::spirit::rule<
                ScannerT,
                typename absolute_uri_closure::context_t>
                absolute_uri_rule_type;
            typedef boost::spirit::rule<ScannerT,
                                        typename server_closure::context_t>
                server_rule_type;

            absolute_uri_rule_type absolute_uri;
            rule_type scheme;
            rule_type hier_part;
            rule_type opaque_part;
            rule_type net_path;
            uri_abs_path_grammar<Actions> abs_path;
            uri_authority_grammar<Actions> authority;
            rule_type query;
            rule_type uric_no_slash;
            uric_grammar uric;

            explicit definition(const absolute_uri_grammar & self):
                abs_path(self.actions),
                authority(self.actions)
            {
                using namespace boost::spirit;
                using namespace phoenix;

                absolute_uri
                    =   (
                            scheme[
                                absolute_uri.scheme_begin = arg1,
                                absolute_uri.scheme_end = arg2
                            ] >> ':'
                        )[
                            assign_iterators(self.actions.scheme,
                                             absolute_uri.scheme_begin,
                                             absolute_uri.scheme_end)
                        ] >> (hier_part | opaque_part)[
                            self.actions.scheme_specific_part
                        ]
                    ;

                scheme
                    =   (alpha_p >> *(alpha_p | digit_p | '+' | '-' | '.'))
                    ;

                hier_part
                    =   (net_path | abs_path) >> !('?' >> query)
                    ;

                opaque_part
                    =   uric_no_slash >> *uric
                    ;

                uric_no_slash
                    =   uric - '/'
                    ;

                net_path
                    =   "//" >> authority >> !abs_path
                    ;

                query
                    =   (*uric)[ self.actions.query ]
                    ;
            }

            const absolute_uri_rule_type & start() const
            {
                return this->absolute_uri;
            }
        };

        const Actions & actions;

        explicit absolute_uri_grammar(const Actions & actions = Actions()):
            actions(actions)
        {}
    };


    template <typename Actions = null_actions>
    struct uri_grammar : public boost::spirit::grammar<uri_grammar<Actions> > {

        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_type;

            rule_type uri_reference;
            absolute_uri_grammar<Actions> absolute_uri;
            rule_type relative_uri;
            rule_type net_path;
            uri_abs_path_grammar<Actions> abs_path;
            rule_type rel_path;
            rule_type rel_segment;
            uri_authority_grammar<Actions> authority;
            rule_type query;
            rule_type fragment;
            uric_grammar uric;
            rule_type escaped;

            explicit definition(const uri_grammar & self):
                absolute_uri(self.actions),
                abs_path(self.actions),
                authority(self.actions)
            {
                using namespace boost::spirit;
                using namespace phoenix;

                BOOST_SPIRIT_DEBUG_NODE(uri_reference);
                BOOST_SPIRIT_DEBUG_NODE(absolute_uri);
                BOOST_SPIRIT_DEBUG_NODE(net_path);
                BOOST_SPIRIT_DEBUG_NODE(abs_path);
                BOOST_SPIRIT_DEBUG_NODE(rel_path);
                BOOST_SPIRIT_DEBUG_NODE(rel_segment);
                BOOST_SPIRIT_DEBUG_NODE(authority);
                BOOST_SPIRIT_DEBUG_NODE(query);
                BOOST_SPIRIT_DEBUG_NODE(fragment);
                BOOST_SPIRIT_DEBUG_NODE(uric);
                BOOST_SPIRIT_DEBUG_NODE(escaped);

                uri_reference
                    =   !(absolute_uri | relative_uri) >> !('#' >> fragment)
                    ;

                relative_uri
                    =   (net_path | abs_path | rel_path) >> !('?' >> query)
                    ;

                net_path
                    =   "//" >> authority >> !abs_path
                    ;

                rel_path
                    =   (rel_segment >> !abs_path)[ self.actions.path ]
                    ;

                rel_segment
                    =  +(   uri_unreserved_p
                        |   escaped
                        |   ';'
                        |   '@'
                        |   '&'
                        |   '='
                        |   '+'
                        |   '$'
                        |   ','
                        )
                    ;

                query
                    =   (*uric)[ self.actions.query ]
                    ;

                fragment
                    =   (*uric)[ self.actions.fragment ]
                    ;

                escaped
                    =   '%' >> xdigit_p >> xdigit_p
                    ;
            }

            const boost::spirit::rule<ScannerT> & start() const
            {
                return this->uri_reference;
            }
        };

        const Actions & actions;

        explicit uri_grammar(const Actions & actions = Actions()):
            actions(actions)
        {}
    };

} // namespace uri

# endif // ifndef URI_GRAMMAR_HPP
