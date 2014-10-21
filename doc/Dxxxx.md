    Document number: Dxxxx=14-xxxx
    Date:            2014-10-21
    Project:         Programming Language C++, Library Evolution Working Group
    Reply-to:        Rein Halbersma <rhalbersma@gmail.com>

User-Defined Literals for `size_t` (and `ptrdiff_t`)
====================================================

Introduction
------------

We propose the user-defined suffix `z` for `size_t` literals. This allows writing code like

    for (auto i = 0z; i < a.size(); ++i) { /* use i and a[i] */ }

that will let the loop variable `i` be of type `size_t`. Similarly, we propose the suffix `t` for `ptrdiff_t` literals. 

Motivation and Scope
--------------------

The main motivations for this proposal are:

  - `int` is the default type deduced from integer literals;
  - `size_t` is almost unavoidable when using the Standard Library;
  - `ptrdiff_t` is significantly less ubiquitous, but still hard to avoid when doing iterator related manipulations;
  - comparisons and arithmetic with integer types of mixed signs or different conversion ranks can lead to surprises;
  - surprises range from (pedantic) compiler warnings, value conversions, or even undefined behavior;
  - eliminating these surprises by explicitly typing `size_t` and `ptrdiff_t` literals is rather verbose;
  - user-defined literals are a type-safe and succinct way to express coding intent;
  - the suffixes `z` and `t` are consisent with the length modifiers for formatted I/O in the C standard library.  

Impact On the Standard
----------------------

This proposal does not depend on other library components, and nothing depends on it. It is a pure extension, but does require additions (though no modifications) to the standard header `<cstddef>`, as outlined in the section **Proposed Wording** below. It can be implemented using C++11 compilers and libraries, and it does not require language or library features that are not part of C++11. 

Design Decisions
----------------

The chosen naming of the literal suffixes `z` and `t` was motivated by the corresponding length modifiers for formatted I/O in the C standard library header `<stdio.h>`. See 7.21.6.1/7 for `fprintf` and 7.21.6.2/11 `fscanf`, numbered relative to [WG14/N1539](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1539.pdf).

The consequences of adopting the proposed literal suffixes into the Standard are:

  - both novices and occasional programmers, as well as experienced library implementors, can use left-to-right `auto` variable initializations with `size_t` and `ptrdiff_t` literals, without having to define their own literal suffixes with leading underscores `_z` and `_t` in order to do so;
  - other existing or future Standard Library types are prevented from adopting the same literal suffixes, unless they use overloads of the corresponding `operator""` that take arguments other than `unsigned long long`. 

There are no decisions left up to implementers, because the suggested wording below fully specifies the proposed functionality. We are not aware of similar libraries in use. There is a [reference implementation](https://bitbucket.org/rhalbersma/xstd/src/42782b8056160340ae9710b993a407fdf6136cc2/include/xstd/cstddef.hpp?at=default) and small [test suite](https://bitbucket.org/rhalbersma/xstd/src/42782b8056160340ae9710b993a407fdf6136cc2/test/src/cstddef.cpp?at=default) available for inspection. Note that the reference implementation uses `namespace xstd` and underscored suffixes `_t` and `_z` because the tested compiler `clang` will enforce the restriction from `[lex.ext]/10` that a program containing a user-defined suffix without an underscore is ill-formed, no diagnostic required.   

Proposed Wording
----------------

Insert in subclause `[support.types]/1` in the synopsis of header `<cstddef>` at the appropriate place the namespace `std::literals::support_literals`: 

>     namespace std {
      inline namespace literals {
        inline namespace support_literals {
          constexpr size_t operator "" z(unsigned long long);       
          constexpr ptrdiff_t operator "" t(unsigned long long);        
        }
      }
    }

Insert a new subclause `[support.literals]` between `[support.types]` and `[support.limits]` as follows (numered relative to [WG21/N4140](https://github.com/cplusplus/draft/blob/master/papers/n4140.pdf)):

> **18.3 Suffixes for support types [support.literals]**
>
> 1 This section describes literal suffixes for constructing `size_t` and `ptrdiff_t` literals. The suffixes `z` and `t` create numbers of the types `size_t` and `ptrdiff_t`, respectively. 
>
>     constexpr size_t operator "" z(unsigned long long u);
> 2 Returns: `static_cast<size_t>(u)`.
>
>     constexpr ptrdiff_t operator "" t(unsigned long long u);        
> 3 Returns: `static_cast<ptrdiff_t>(u)`.

Acknowledgments
----------------

We gratefully acknowledge feedback from Jerry Coffin and Andy Prowl.

References
----------

N3642. Peter Sommerlad, *User-defined Literals for Standard Library Types (part 1 - version 4)* [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf)

GotW #94. Herb Sutter, *AAA Style (Almost Always Auto)* [http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/) 
