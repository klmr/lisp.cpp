#include "environment.hpp"
#include "eval.hpp"

#include <functional>
#include <numeric>
#include <utility>
#include <vector>

namespace klmr { namespace lisp {

environment::environment(
        environment& parent,
        std::vector<symbol> formals,
        call::iterator a, call::iterator b)
        : parent{&parent} {
    assert(static_cast<decltype(formals.size())>(std::distance(a, b)) == formals.size());
    for (auto&& sym : formals)
        add(sym, *a++);
}

auto environment::operator[](symbol const& sym) -> value& {
    for (auto current = this; current != nullptr; current = current->parent) {
        auto&& it = current->frame.find(sym);
        if (it != current->frame.end())
            return it->second;
    }

    throw name_error{sym};
}

auto environment::add(symbol const& sym, value val) -> void {
    frame.emplace(sym, std::forward<value>(val));
}

auto parent(environment const& env) -> environment* {
    return env.parent;
}

template <typename R, template<typename> class Op>
struct binary_operation : boost::static_visitor<R> {
    template <typename T>
    auto operator ()(literal<T> const& a, literal<T> const& b) const -> R {
        return Op<T>{}(as_raw(a), as_raw(b));
    }

    template <typename T, typename U>
    auto operator ()(T const&, U const&) const -> R {
        throw value_error{"Mismatching operand types for =="};
    }
};

template <typename T>
struct not_equal_to : std::binary_negate<std::equal_to<T>> {
    not_equal_to() : std::binary_negate<std::equal_to<T>>{std::equal_to<T>{}} {}
};

auto get_global_environment() -> environment {
    auto env = environment{};

#   define VAR_OPERATOR(name, op, type) \
    env.add(symbol{name}, \
        call{env, "args", [] (environment& env) { \
            auto&& args = as_list(env["args"]); \
            return std::accumulate( \
                std::next(begin(args)), end(args), \
                *begin(args), [] (value const& a, value const& b) { \
                    return literal<type>{as_raw<type>(a) op as_raw<type>(b)}; \
                }); \
        }} \
    )

    VAR_OPERATOR("+", +, double);
    VAR_OPERATOR("-", -, double);
    VAR_OPERATOR("*", *, double);
    VAR_OPERATOR("/", /, double);
    VAR_OPERATOR("and", &&, bool);
    VAR_OPERATOR("or", ||, bool);

#   undef VAR_OPERATOR

#   define BIN_OPERATOR(name, op, ret) \
    env.add(symbol{name}, \
        call{env, {"a", "b"}, [] (environment& env) { \
            auto&& a = env["a"]; \
            auto&& b = env["b"]; \
            return literal<ret>{boost::apply_visitor(binary_operation<ret, op>{}, a, b)}; \
        }} \
    )

    BIN_OPERATOR("==", std::equal_to, bool);
    BIN_OPERATOR("!=", not_equal_to, bool);
    BIN_OPERATOR("<", std::less, bool);
    BIN_OPERATOR(">", std::greater, bool);
    BIN_OPERATOR("<=", std::less_equal, bool);
    BIN_OPERATOR(">=", std::greater_equal, bool);

#   undef BIN_OPERATOR

    env.add(symbol{"quote"},
        macro{env, std::vector<symbol>{"expr"},
            [] (environment& env) { return env["expr"]; }}
    );

    env.add(symbol{"lambda"},
        macro{env, {"args", "expr"}, [] (environment& env) {
            auto&& args = as_list(env["args"]);
            auto formals = std::vector<symbol>(length(args));
            std::transform(begin(args), end(args), begin(formals), as_symbol);
            auto expr = env["expr"];
            // FIXME Capture by value incurs expensive copy. Solved in C++14.
            return call{env, formals, [expr](environment& frame) {
                return eval(expr, frame);
            }};
        }}
    );

    env.add(symbol{"define"},
        macro{env, {"name", "expr"}, [] (environment& env) {
            auto&& name = as_symbol(env["name"]);
            parent(env)->add(name, eval(env["expr"], *parent(env)));
            return eval(nil, env);
        }}
    );

    env.add(symbol{"if"},
        macro{env, {"cond", "conseq", "alt"}, [] (environment& env) {
            auto&& cond = eval(env["cond"], *parent(env));
            return eval(is_true(cond) ? env["conseq"] : env["alt"], *parent(env));
        }}
    );

    env.add(symbol{"set!"},
        macro{env, {"name", "expr"}, [] (environment& env) {
            auto&& name = as_symbol(env["name"]);
            (*parent(env))[name] = eval(env["expr"], *parent(env));
            return eval(nil, env);
        }}
    );

    env.add(symbol{"begin"},
        macro{env, "args", [] (environment& env) {
            auto&& result = value{};
            for (auto&& expr : as_list(env["args"]))
                result = eval(expr, *parent(env));
            return result;
        }}
    );

    return env;
}

} } // namespace klmr::lisp
