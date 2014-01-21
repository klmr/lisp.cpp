#ifndef KLMR_LISP_CPP_ENVIRONMENT_HPP
#define KLMR_LISP_CPP_ENVIRONMENT_HPP

#include "value.hpp"

#include <unordered_map>

namespace klmr { namespace lisp {

struct environment {
    environment(environment* parent = nullptr) : parent{parent} {}

    auto operator [](symbol const& sym) -> value&;

    auto set(symbol const& sym, value val) -> void;

private:
    std::unordered_map<symbol, value> frame;
    environment* parent;
};

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_ENVIRONMENT_HPP
