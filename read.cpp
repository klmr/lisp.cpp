#include "read.hpp"
#include "value.hpp"

#include <iterator>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_match_attr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/optional.hpp>

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(klmr::lisp::symbol, (std::string, repr))
BOOST_FUSION_ADAPT_STRUCT(klmr::lisp::literal<bool>, (bool, value))
BOOST_FUSION_ADAPT_STRUCT(klmr::lisp::literal<double>, (double, value))
BOOST_FUSION_ADAPT_STRUCT(klmr::lisp::literal<std::string>, (std::string, value))
BOOST_FUSION_ADAPT_STRUCT(klmr::lisp::list, (std::vector<klmr::lisp::value>, values))

namespace klmr { namespace lisp {

template <typename Iterator>
struct lisp_grammar : qi::grammar<Iterator, value(), qi::ascii::space_type> {
    template <typename Signature>
    using rule_t = qi::rule<Iterator, Signature, qi::ascii::space_type>;

    rule_t<value()> start;
    rule_t<symbol()> sym;
    rule_t<value()> lit;
    rule_t<literal<bool>()> boolean;
    rule_t<literal<double>()> number;
    rule_t<literal<std::string>()> string;
    rule_t<list()> lst;

    lisp_grammar() : lisp_grammar::base_type{start} {
        using namespace qi;
        using namespace boost::phoenix;
        start %= sym | lit | lst;
        sym %= eps >> lexeme[+(char_("A-Za-z") | char_('-') | char_("+*/%~&|^!=<>?"))];
        lit %= boolean | number | string;
        boolean %=
            qi::lit("#t")[_val = construct<literal<bool>>(true)] |
            qi::lit("#f")[_val = construct<literal<bool>>(false)];
        number %= double_;
        string %= eps >> lexeme['"' >> *(char_ - '"') >> '"'];
        lst %= '(' >> *start >> ')';
    }
};

template <typename Iterator>
auto read(Iterator& begin, Iterator end) -> boost::optional<value> {
    static lisp_grammar<Iterator> grammar{};
    auto output = value{};
    auto success = phrase_parse(begin, end, grammar, qi::ascii::space, output);
    return boost::optional<value>{success, output};
}

template auto read<std::string::iterator>(std::string::iterator&, std::string::iterator)
    -> boost::optional<value>;

template auto read<stream_iterator_t>(stream_iterator_t&, stream_iterator_t)
    -> boost::optional<value>;

} } // namespace klmr::lisp
