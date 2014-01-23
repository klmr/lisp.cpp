Lisp in C++
===========

Purpose
-------

This is a playground for some ideas inspired by [Peter Norvig][] and [Anthony Hay][].

Peter wrote a nifty little Lisp (actually, Scheme) implementation in just 90
lines of Python code. Anthony attempted the same feat in C++, and fared quite
well with it.

Unlike their approaches, we focus on *simplicity* and *correctness* over
shortness. These concepts are related but not identical. In particular, while
correctness is relatively straightforward in Python, it’s less so in (classical)
C++. Modern C++ and Boost to the rescue. And as for shortness, Peter’s and
Anthony’s solutions take shortcuts at the cost of readability. We abstain from
this.

Guidelines
----------

In order to understand what we did here, consider the following principles:

 * Manual memory management is error-prone. Avoid it completely.
 * In fact, here are some simple, generally applicable rules:
    * `new` and `delete` are illegal. So are `malloc` and `free`.
    * C++ allocators are only permissible in low-level implementations of value
      holders.
    * Any other kind of memory management must be performed via exception safe
      constructs. Use `make_shared` and `make_unique` where smart pointers are
      indicated.
 * Pointers often add unnecessary indirection to the code. Only use them when
   actually required. C++ excels at value semantics – stick to that where
   possible.
 * As a corollary, avoid modelling everything via polymorphism class hierarchies
   with virtual functions – these often require the use of pointers. There are
   alternatives. In particular, discriminated unions can be modelled elegantly
   via `boost::variant`.
 * Favour early error checking. This implies relying on the compiler for static
   type checking, and codifying invariants into types. `boost::static_visitor`
   allows to do this for `boost::variant` usage.

These guidelines make the code simpler, less error-prone, and in particular the
increase the *confidence* in the code’s correctness because common pitfalls are
categorically avoided and the reduced complexity makes it easier verifiable.

None of these principles is new or surprising. But people often forget that they
are applicable to C++, too.

[Peter Norvig]: http://norvig.com/lispy.html
[Anthony Hay]: http://howtowriteaprogram.blogspot.de/2010/11/lisp-interpreter-in-90-lines-of-c.html
