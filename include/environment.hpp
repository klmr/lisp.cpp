#ifndef KLMR_LISP_CPP_ENVIRONMENT_HPP
#define KLMR_LISP_CPP_ENVIRONMENT_HPP

#include "value.hpp"
#include "error.hpp"

#include <unordered_map>
#include <vector>

namespace klmr { namespace lisp {

struct environment {
    environment(environment* parent = nullptr) : parent{parent} {}

    environment(environment&, value const& arglist, call::iterator a, call::iterator b);

    auto operator [](symbol const& sym) -> value&;

    auto add(symbol const& sym, value val) -> void;

    friend auto parent(environment const&) -> environment*;

private:
    std::unordered_map<symbol, value> frame;
    environment* parent;
};

auto get_global_environment() -> environment;

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_ENVIRONMENT_HPP
