#include "eval.hpp"

#include <iostream>

using klmr::lisp::value;
using klmr::lisp::environment;

auto read(std::string const& input) -> value {
    (void) input;
    using namespace klmr::lisp;

    static auto items = std::vector<value> {
        list{
            symbol{"define"},
            symbol{"double"},
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

        list{
            symbol{"double"},
            literal<double>{4}
        }
    };

    static auto current = 0u;

    return current < items.size() ? items[current++] : value{nil}; // FIXME placeholder
}

auto get_global_environment() -> environment {
    using namespace klmr::lisp;
    auto env = environment{};
    env.set(symbol{"*"},
        call{env, {"a", "b"}, [] (environment& env) {
            return as_literal(as_raw<double>(env["a"]) * as_raw<double>(env["b"]));
        }}
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
            return nil;
        }}
    );
    return env;
}

auto repl(std::string prompt) -> void {
    auto input = std::string{};
    auto global_env = get_global_environment();
    for (;;) {
        std::cout << prompt << std::flush;
        if (not getline(std::cin, input))
            return;
        auto&& result = eval(read(input), global_env);
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
