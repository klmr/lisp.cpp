#ifndef KLMR_LISP_CPP_READ_HPP
#define KLMR_LISP_CPP_READ_HPP

#include "value.hpp"

#include <boost/optional.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace klmr { namespace lisp {

template <typename Iterator>
auto read(Iterator& begin, Iterator end) -> boost::optional<value>;

// For parsing a whole range into a single expression, e.g. in a REPL.
template <typename Iterator>
inline auto read_full(Iterator begin, Iterator end) -> boost::optional<value> {
    auto&& result = read(begin, end);
    return boost::optional<value>{result and begin == end, *result};
}

namespace detail {
    using underlying_t = std::istreambuf_iterator<char>;
}

using stream_iterator_t = boost::spirit::multi_pass<detail::underlying_t>;

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_READ_HPP
