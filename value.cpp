#include "value.hpp"
#include "environment.hpp"

#include <iostream>

namespace klmr { namespace lisp {

template <call_type C>
template <typename F>
callable<C>::callable(environment& parent, std::vector<std::string> formals, F lambda)
    : parent{parent}, formals{formals}, lambda{lambda} {}

auto operator <<(std::ostream& out, symbol const& sym) -> std::ostream& {
    return out << sym.repr;
}

auto operator <<(std::ostream& out, literal const& lit) -> std::ostream& {
    return out << lit.value;
}

auto operator <<(std::ostream& out, macro const&) -> std::ostream& {
    return out << "#macro";
}

auto operator <<(std::ostream& out, call const&) -> std::ostream& {
    return out << "#call";
}

auto operator <<(std::ostream& out, list const& list) -> std::ostream& {
    out << '(';
    auto first = true;
    for (auto&& value : values(list)) {
        if (first) first = false;
        else out << ' ';
        out << value;
    }
    return out << ')';
}

} } // namespace klmr::lisp
