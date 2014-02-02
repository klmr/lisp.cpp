#include "value.hpp"

#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_match_attr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
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
    static lisp_grammar<std::string::iterator> grammar{};
    auto output = lisp::value{};
    //auto&& success = input >> phrase_match(grammar, ascii::space, output);
    //DEBUG
    auto in = std::string{};
    getline(input, in);
    auto e = end(in);
    auto&& success = phrase_parse(begin(in), e, grammar, ascii::space, output);
    //\DEBUG
    return boost::optional<lisp::value>{success, output};
}

auto main() -> int {
    auto prompt = ">>> ";
    for (;;) {
        std::cout << prompt << std::flush;
        auto&& expr = read(std::cin);
        if (not expr)
            break;
        std::cout << *expr << '\n';
    }
}
