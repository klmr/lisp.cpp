#include "environment.hpp"

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
        set(sym, *a++);
}

auto environment::operator[](symbol const& sym) -> value& {
    for (auto current = this; current != nullptr; current = current->parent) {
        auto&& it = current->frame.find(sym);
        if (it != current->frame.end())
            return it->second;
    }

    throw name_error{sym};
}

auto environment::set(symbol const& sym, value val) -> void {
    frame.emplace(sym, std::forward<value>(val));
}

} } // namespace klmr::lisp
