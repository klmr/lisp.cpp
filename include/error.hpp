#ifndef KLMR_LISP_CPP_ERRORS_HPP
#define KLMR_LISP_CPP_ERRORS_HPP

#include "value.hpp"

#include <stdexcept>

namespace klmr { namespace lisp {

struct error : std::logic_error {
    using std::logic_error::logic_error;
};

struct name_error : error {
    name_error(symbol const& sym) : error{"Name not found: " + sym.repr} {}
};

struct invalid_node : error {
    invalid_node(value const& val)
        : error{"Unexpected value " + to_string(val) + " at this place"} {}

    invalid_node(value const& val, std::string const& expected)
        : error{"Invalid value " + to_string(val) + ", expected " + expected} {}
};

struct value_error : error {
    value_error(std::string const& str) : error{str} {}
};

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_ERRORS_HPP
