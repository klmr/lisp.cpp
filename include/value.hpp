#ifndef KLMR_LISP_CPP_VALUE_HPP
#define KLMR_LISP_CPP_VALUE_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/variant.hpp>
#include <boost/range/sub_range.hpp>

namespace klmr { namespace lisp {

struct symbol {
    std::string repr;

    symbol() = default;
    symbol(std::string repr) : repr{std::move(repr)} {}
    symbol(char const* repr) : repr{repr} {}
};

inline auto operator ==(symbol const& lhs, symbol const& rhs) -> bool {
    return lhs.repr == rhs.repr;
}

inline auto operator !=(symbol const& lhs, symbol const& rhs) -> bool {
    return not (lhs == rhs);
}

template <typename T>
struct literal {
    T value;

    literal() = default;
    literal(T const& value) : value{value} {}
};

enum class call_type { macro, call };

template <call_type C>
struct callable;

using macro = callable<call_type::macro>;

using call = callable<call_type::call>;

struct list;

using value = boost::variant<
    symbol,
    literal<bool>,
    literal<double>,
    literal<std::string>,
    boost::recursive_wrapper<macro>,
    boost::recursive_wrapper<call>,
    boost::recursive_wrapper<list>
>;

struct list {
    using range = boost::sub_range<std::vector<value>>;
    using const_range = boost::sub_range<std::vector<value> const>;
    std::vector<value> values;

    list() = default;

    list(std::initializer_list<value> values) : values(values) {}

    list(const_range::iterator begin, const_range::iterator end)
        : values(begin, end) {}
};

inline auto head(list const& list) -> value const& {
    return list.values[0];
}

inline auto tail(list const& list) -> list::const_range {
    return {begin(list.values) + 1, end(list.values)};
}

inline auto values(list const& list) -> decltype(list.values) const& {
    return list.values;
}

inline auto empty(list const& list) -> bool {
    return list.values.empty();
}

inline auto length(list const& list) -> std::size_t {
    return list.values.size();
}

inline auto begin(list const& list) -> decltype(begin(list.values)) {
    return begin(list.values);
}

inline auto end(list const& list) -> decltype(end(list.values)) {
    return end(list.values);
}

struct environment;

template <call_type C>
struct callable {
    using iterator = list::const_range::iterator;
    using function_type = std::function<value(environment&)>;

    callable(environment& parent, std::vector<symbol> const& formals, function_type lambda);

    callable(environment& parent, symbol const& arglist, function_type lambda);

    // FIXME This leaks memory for “upward funargs”.
    environment* parent;
    boost::variant<std::vector<symbol>, symbol> formals;
    function_type lambda;

    auto operator ()(environment& env, iterator begin, iterator end) const -> value;
};

inline auto as_symbol(value const& value) -> symbol {
    return boost::get<symbol>(value);
}

template <typename T>
inline auto as_literal(value const& value) -> literal<T> {
    return boost::get<literal<T>>(value);
}

template <typename T>
inline auto as_literal(T const& value) -> literal<T> {
    return {value};
}

template <typename T>
inline auto as_raw(literal<T> const& value) -> T {
    return value.value;
}

template <typename T>
inline auto as_raw(value const& value) -> T {
    return as_raw(as_literal<T>(value));
}

inline auto as_list(value const& value) -> list {
    return boost::get<list>(value);
}

auto is_true(value const& value) -> bool;

auto operator <<(std::ostream& out, symbol const& sym) -> std::ostream&;

template <typename T>
auto operator <<(std::ostream& out, literal<T> const& lit) -> std::ostream&;

auto operator <<(std::ostream& out, macro const& macro) -> std::ostream&;

auto operator <<(std::ostream& out, call const& call) -> std::ostream&;

auto operator <<(std::ostream& out, list const& list) -> std::ostream&;

auto to_string(value const& val) ->  std::string;

extern const list nil;

} } // namespace klmr::lisp

namespace std {
    template <>
    struct hash<::klmr::lisp::symbol> {
        auto operator ()(::klmr::lisp::symbol const& value) const -> size_t {
            return std::hash<decltype(value.repr)>{}(value.repr);
        }
    };
} // namespace std

#endif // ndef KLMR_LISP_CPP_VALUE_HPP
