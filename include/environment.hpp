#ifndef KLMR_LISP_CPP_ENVIRONMENT_HPP
#define KLMR_LISP_CPP_ENVIRONMENT_HPP

#include "value.hpp"

#include <unordered_map>
#include <vector>

namespace klmr { namespace lisp {

struct name_error : std::logic_error {
    name_error(symbol sym) : std::logic_error{"Not found: " + sym.repr} {}
};

struct environment {
    environment(environment* parent = nullptr) : parent{parent} {}

    environment(environment&, std::vector<symbol>, call::iterator a, call::iterator b);

    auto operator [](symbol const& sym) -> value&;

    auto set(symbol const& sym, value val) -> void;

private:
    std::unordered_map<symbol, value> frame;
    environment* parent;
};

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_ENVIRONMENT_HPP
