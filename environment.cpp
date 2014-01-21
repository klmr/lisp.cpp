#include "environment.hpp"

#include <utility>

namespace klmr { namespace lisp {

auto environment::operator[](symbol const& sym) -> value& {
    for (auto current = this; current != nullptr; current = current->parent) {
        auto&& it = current->frame.find(sym);
        if (it != current->frame.end())
            return it->second;
    }

    throw "Not found"; // TODO Replace by real exception type.
}

auto environment::set(symbol const& sym, value val) -> void {
    frame.emplace(sym, std::forward<value>(val));
}

} } // namespace klmr::lisp
