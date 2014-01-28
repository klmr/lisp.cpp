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

struct literal {
    double value;
};

enum class call_type { macro, call };

template <call_type C>
struct callable;

using macro = callable<call_type::macro>;

using call = callable<call_type::call>;

struct list;

using value = boost::variant<
    symbol,
    literal,
    boost::recursive_wrapper<macro>,
    boost::recursive_wrapper<call>,
    boost::recursive_wrapper<list>
>;

struct list {
    using range = boost::sub_range<std::vector<value>>;
    using const_range = boost::sub_range<std::vector<value> const>;
    std::vector<value> values;

    template <typename... Ts>
    list(Ts&&... values) : values{std::forward<Ts>(values)...} {}
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

    // FIXME This leaks memory for “upward funargs”.
    environment& parent;
    std::vector<symbol> formals;
    function_type lambda;

    auto operator ()(environment& env, iterator begin, iterator end) const -> value;
};

inline auto as_symbol(value const& value) -> symbol {
    return boost::get<symbol>(value);
}

inline auto as_literal(value const& value) -> literal {
    return boost::get<literal>(value);
}

inline auto as_list(value const& value) -> list {
    return boost::get<list>(value);
}

auto operator <<(std::ostream& out, symbol const& sym) -> std::ostream&;

auto operator <<(std::ostream& out, literal const& lit) -> std::ostream&;

auto operator <<(std::ostream& out, macro const& macro) -> std::ostream&;

auto operator <<(std::ostream& out, call const& call) -> std::ostream&;

auto operator <<(std::ostream& out, list const& list) -> std::ostream&;

extern const value nil;

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
