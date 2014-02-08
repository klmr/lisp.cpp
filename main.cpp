#include "eval.hpp"
#include "read.hpp"
#include "error.hpp"

#include <iostream>
#include <iterator>
#include <boost/optional.hpp>

using klmr::lisp::value;
using klmr::lisp::environment;
using klmr::lisp::get_global_environment;

auto repl(std::string prompt) -> void {
    auto global_env = get_global_environment();

    for (;;) {
        std::cout << prompt << std::flush;
        auto line = std::string{};
        if (not getline(std::cin, line))
            return;

        try {
            auto&& expr = klmr::lisp::read_full(begin(line), end(line));
            if (not expr)
                return;
            auto&& result = eval(*expr, global_env);
            std::cout << result << '\n';
        }
        catch (klmr::lisp::error const& err) {
            std::cerr << err.what() << '\n';
        }
    }
}

auto main() -> int {
    repl(">>> ");
}
