#include "eval.hpp"

#include <iostream>
#include <boost/optional.hpp>

using klmr::lisp::value;
using klmr::lisp::environment;

auto read(std::istream& input) -> boost::optional<value> {
    (void) input;
    using namespace klmr::lisp;

    static auto items = std::vector<value> {
        list{
            symbol{"define"},
            symbol{"square"},
            list{
                symbol{"lambda"},
                list{symbol{"n"}},
                list{
                    symbol{"*"},
                    symbol{"n"},
                    symbol{"n"}
                }
            }
        },

        symbol{"square"},

        list{
            symbol{"square"},
            literal<double>{4}
        },

        literal<double>{42},

        literal<std::string>{"Hello world!"},
    };

    static auto current = 0u;

    auto expr = boost::optional<value>{current < items.size(), items[current++]}; // FIXME placeholder
    if (expr)
        std::cout << *expr << '\n';
    return expr;
}

auto get_global_environment() -> environment {
    using namespace klmr::lisp;
    auto env = environment{};
    env.set(symbol{"*"},
        call{env, {"a", "b"}, [] (environment& env) {
            return as_literal(as_raw<double>(env["a"]) * as_raw<double>(env["b"]));
        }}
    );
    env.set(symbol{"quote"},
        macro{env, {"expr"}, [] (environment& env) { return env["expr"]; }}
    );
    env.set(symbol{"lambda"},
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
    env.set(symbol{"define"},
        macro{env, {"name", "expr"}, [] (environment& env) {
            auto&& name = as_symbol(env["name"]);
            parent(env)->set(name, eval(env["expr"], env));
            return eval(nil, env);
        }}
    );
    return env;
}

auto repl(std::string prompt) -> void {
    auto global_env = get_global_environment();

    for (;;) {
        std::cout << prompt << std::flush;
        auto&& expr = read(std::cin);
        if (not expr)
            return;
        auto&& result = eval(*expr, global_env);
        std::cout << result << '\n';
    }
}

auto main() -> int
try {
    repl(">>> ");
} catch (klmr::lisp::name_error const& msg) {
    std::cerr << msg.what() << '\n';
    return 1;
}
