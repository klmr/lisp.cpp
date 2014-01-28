#include "value.hpp"
#include "environment.hpp"

#include <iostream>

namespace klmr { namespace lisp {

template <call_type C>
callable<C>::callable(
        environment& parent,
        std::vector<symbol> const& formals,
        typename callable<C>::function_type lambda)
    : parent{parent}, formals{formals}, lambda{lambda} {}

template <call_type C>
auto callable<C>::operator ()(
        environment& env,
        iterator begin,
        iterator end) const -> value {
    environment frame(env, formals, begin, end);
    return lambda(frame);
}

template struct callable<call_type::call>;
template struct callable<call_type::macro>;

template struct literal<bool>;
template struct literal<double>;
template struct literal<std::string>;

auto operator <<(std::ostream& out, symbol const& sym) -> std::ostream& {
    return out << sym.repr;
}

template <typename T>
auto operator <<(std::ostream& out, literal<T> const& lit) -> std::ostream& {
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

list const nil{};

} } // namespace klmr::lisp
