#include "value.hpp"
#include "environment.hpp"

#include <iostream>
#include <sstream>

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

auto as_symbol(value const& value) -> symbol {
    return boost::get<symbol>(value);
}

template <typename T>
auto as_literal(value const& value) -> literal<T> try {
    return boost::get<literal<T>>(value);
}
catch (boost::bad_get const& err) {
    throw value_error{"Invalid argument type"};
}

template auto as_literal<bool>(value const&) -> literal<bool>;
template auto as_literal<double>(value const&) -> literal<double>;
template auto as_literal<std::string>(value const&) -> literal<std::string>;

template <typename T>
auto as_literal(T const& value) -> literal<T> {
    return {value};
}

template auto as_literal<bool>(bool const&) -> literal<bool>;
template auto as_literal<double>(double const&) -> literal<double>;
template auto as_literal<std::string>(std::string const&) -> literal<std::string>;

template <typename T>
auto as_raw(literal<T> const& value) -> T {
    return value.value;
}

template auto as_raw<bool>(literal<bool> const&) -> bool;
template auto as_raw<double>(literal<double> const&) -> double;
template auto as_raw<std::string>(literal<std::string> const&) -> std::string;

template <typename T>
auto as_raw(value const& value) -> T {
    return as_raw(as_literal<T>(value));
}

template auto as_raw<bool>(value const&) -> bool;
template auto as_raw<double>(value const&) -> double;
template auto as_raw<std::string>(value const&) -> std::string;

auto as_list(value const& value) -> list {
    return boost::get<list>(value);
}

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

auto to_string(value const& val) -> std::string {
    std::ostringstream ostr;
    ostr << val;
    return ostr.str();
}

list const nil{symbol{"quote"}, list{}};

} } // namespace klmr::lisp
