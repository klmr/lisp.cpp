#include "eval.hpp"

namespace klmr { namespace lisp {

struct eval_call_visitor : boost::static_visitor<value> {
    environment& env;
    list::const_range args;

    eval_call_visitor(environment& env, list::const_range args) :
        env{env}, args{args} {}

    auto operator ()(call const& call) const -> value {
        auto evaluated_args = std::vector<value>(std::distance(begin(args), end(args)));
        std::transform(begin(args), end(args), begin(evaluated_args),
            [this](value const& v) { return eval(v, env); });
        return call(env, begin(evaluated_args), end(evaluated_args));
    }

    auto operator ()(macro const& macro) const -> value {
        return macro(env, begin(args), end(args));
    }

    template <typename T>
    auto operator ()(T const&) const -> value {
        throw "Invalid symbol where call was expected";
    }
};

struct eval_visitor : boost::static_visitor<value> {
    environment& env;

    eval_visitor(environment& env) : env{env} {}

    auto operator ()(symbol const& sym) const -> value {
        return env[sym];
    }

    template <typename T>
    auto operator ()(literal<T> const& lit) const -> value {
        return lit;
    }

    auto operator ()(list const& cons) const -> value {
        auto&& call = eval(head(cons), env);
        return boost::apply_visitor(eval_call_visitor{env, tail(cons)}, call);
    }

    template <typename T>
    auto operator ()(T const&) const -> value {
        // Everything else is invalid.
        throw "invalid node type";
    }
};

auto eval(value expr, environment& env) -> value {
    return boost::apply_visitor(eval_visitor{env}, expr);
}

} } // namespace klmr::lisp
