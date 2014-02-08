#include "value.hpp"
#include "environment.hpp"

#include <iostream>

namespace klmr { namespace lisp {

template <call_type C>
callable<C>::callable(
        environment& parent,
        std::vector<symbol> const& formals,
        typename callable<C>::function_type lambda)
    : parent{&parent}, formals{formals}, lambda{lambda} {}

template <call_type C>
callable<C>::callable(
        environment& parent,
        symbol const& arglist,
        typename callable<C>::function_type lambda)
    : parent{&parent}, formals{arglist}, lambda{lambda} {}

template <typename Iter>
struct make_env : boost::static_visitor<environment> {
    environment& env;
    Iter begin;
    Iter end;

    make_env(environment& env, Iter begin, Iter end)
        : env{env}, begin{begin}, end{end} {}

    auto operator ()(symbol const& arglist) const -> environment {
        auto argpack = std::vector<value>{list(begin, end)};
        return environment{env, {arglist}, argpack.begin(), argpack.end()};
    }

    auto operator ()(std::vector<symbol> const& formals) const -> environment {
        return environment{env, formals, begin, end};
    }
};

template <call_type C>
auto callable<C>::operator ()(
        environment& env,
        iterator begin,
        iterator end) const -> value {
    // If `formals` happens to be an identifier then we need to construct a
    // `list` of arguments first, and pass that as the one argument to the
    // constructor of `environment`.
    auto&& frame = boost::apply_visitor(make_env<iterator>{env, begin, end}, formals);
    return lambda(frame);
}

template struct callable<call_type::call>;
template struct callable<call_type::macro>;

template struct literal<bool>;
template struct literal<double>;
template struct literal<std::string>;

struct is_true_visitor : boost::static_visitor<bool> {
    auto operator ()(literal<bool> const& lit) const -> bool {
        return as_raw(lit);
    }

    auto operator ()(list const& list) const -> bool {
        return not empty(list);
    }

    template <typename T>
    auto operator ()(T const&) const -> bool {
        return false;
    }
};

auto is_true(value const& value) -> bool {
    // We call “false” a boolean `#f` literal and the empty list. Everything
    // else is “true”.
    return boost::apply_visitor(is_true_visitor{}, value);
}

auto operator <<(std::ostream& out, symbol const& sym) -> std::ostream& {
    return out << sym.repr;
}

template <typename T>
auto operator <<(std::ostream& out, literal<T> const& lit) -> std::ostream& {
    return out << lit.value;
}

template <>
auto operator << <bool>(std::ostream& out, literal<bool> const& lit) -> std::ostream& {
    return out << (as_raw(lit) ? "#t" : "#f");
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

list const nil{symbol{"quote"}, list{}};

} } // namespace klmr::lisp
