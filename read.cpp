#include "value.hpp"

#include <iterator>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_match_attr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/optional.hpp>

using namespace boost::spirit::qi;
namespace lisp = klmr::lisp;

BOOST_FUSION_ADAPT_STRUCT(lisp::symbol, (std::string, repr))
BOOST_FUSION_ADAPT_STRUCT(lisp::literal<bool>, (bool, value))
BOOST_FUSION_ADAPT_STRUCT(lisp::literal<double>, (double, value))
BOOST_FUSION_ADAPT_STRUCT(lisp::literal<std::string>, (std::string, value))
BOOST_FUSION_ADAPT_STRUCT(lisp::list, (std::vector<lisp::value>, values))

template <typename Iterator>
struct lisp_grammar : grammar<Iterator, lisp::value(), ascii::space_type> {
    template <typename Signature>
    using rule_t = rule<Iterator, Signature, ascii::space_type>;

    rule_t<lisp::value()> start;
    rule_t<lisp::symbol()> symbol;
    rule_t<lisp::value()> literal;
    rule_t<lisp::literal<bool>()> boolean;
    rule_t<lisp::literal<double>()> number;
    rule_t<lisp::literal<std::string>()> string;
    rule_t<lisp::list()> list;

    lisp_grammar() : lisp_grammar::base_type{start} {
        start %= symbol | literal | list;
        symbol %= eps >> lexeme[+(char_("A-Za-z") | '-' | char_("+*/%~&|^!=<>?"))];
        literal %= boolean | number | string;
        boolean %=
            lit("#t")[_val = boost::phoenix::construct<lisp::literal<bool>>(true)] |
            lit("#f")[_val = boost::phoenix::construct<lisp::literal<bool>>(false)];
        number %= double_;
        string %= eps >> lexeme['"' >> *(char_ - '"') >> '"'];
        list %= '(' >> *start >> ')';
    }
};

auto read(std::istream& input) -> boost::optional<lisp::value> {
    using underlying_t = std::istreambuf_iterator<char>;
    using iterator_t = boost::spirit::multi_pass<underlying_t>;

    static lisp_grammar<iterator_t> grammar{};
    auto output = lisp::value{};
    auto begin = boost::spirit::make_default_multi_pass(underlying_t{input});
    auto const end = boost::spirit::make_default_multi_pass(underlying_t{});
    auto success = phrase_parse(begin, end, grammar, ascii::space, output) and begin == end;
    return boost::optional<lisp::value>{success, output};
}
