#ifndef KLMR_LISP_CPP_EVAL_HPP
#define KLMR_LISP_CPP_EVAL_HPP

#include "value.hpp"
#include "environment.hpp"

namespace klmr { namespace lisp {

auto eval(value expr, environment& env) -> value;

} } // namespace klmr::lisp

#endif // ndef KLMR_LISP_CPP_EVAL_HPP
