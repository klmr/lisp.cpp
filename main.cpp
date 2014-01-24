#include "eval.hpp"

#include <iostream>

using klmr::lisp::value;
using klmr::lisp::environment;

auto read(std::string const& input) -> value {
    (void) input;
    using namespace klmr::lisp;

    return list{
        symbol{"define"},
        list{
            symbol{"lambda"},
            list{symbol{"n"}},
            list{
                symbol{"*"},
                symbol{"n"},
                symbol{"n"}
            }
        }
    }; // FIXME placeholder
}

auto get_global_environment() -> environment {
    using klmr::lisp::symbol;
    auto env = environment{};
    env.set(symbol{"lambda"}, symbol{"#lambda"});
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
