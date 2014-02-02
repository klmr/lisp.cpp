#ifndef KLMR_LISP_CPP_READ_HPP
#define KLMR_LISP_CPP_READ_HPP

#include "value.hpp"

#include <boost/optional.hpp>

namespace klmr { namespace lisp {

template <typename Iterator>
auto read(Iterator& begin, Iterator end) -> boost::optional<value>;

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_READ_HPP
