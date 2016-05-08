    Document #: WG21 PXXXXRY
    Date:       2016-05-08
    Revises:    N4254
    Project:    JTC1.22.32 Programming Language C++
    Audience:   LEWG
    Reply-to:   Rein Halbersma <rhalbersma at gmail dot com>

User-Defined Literals for `size_t`
====================================================

1 Introduction
--------------

Following an earlier [discussion](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo) on `std-proposals`, we propose the user-defined suffix `zu` for `size_t` literals. This allows the convenient left-to-right `auto` variable initialization:
    
    auto s = 0zu; // local variable s has value 0 and type size_t

2 Motivation and Scope
----------------------

**2.1** The main motivations for this proposal are:

  - `int` is the default type deduced from integer literals without suffix;
  - `size_t` is almost unavoidable when using the standard containers element access or `size()` member functions;
  - comparisons and arithmetic with integer types of mixed signs or different conversion ranks can lead to surprises;
  - surprises range from (pedantic) compiler warnings, value conversions, or even undefined behavior;
  - eliminating these surprises by explicitly typing or casting `size_t` literals is rather verbose;
  - user-defined literals are a succinct and type-safe way to express coding intent;
  - the suffix `zu` is consistent with the `%zu` length modifier for `size_t` formatted I/O in the C standard library (see also section 6).
  
**2.2** The scope of this proposal is limited to a literal suffix `zu` for the support type `size_t` in the Standard Library header `<cstddef>`. 

**2.3** The previous version of this proposal ([WG21/N4254](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4254.html)) also proposed adding a user-defined suffix for `ptrdiff_t` literals. This part of the proposal has been dropped based on feedback from LEWG.

**2.4** Note that a technically similar proposal could be made for literal suffixes for the integer types in the Standard Library header `<cstdint>`, such as literal suffixes `uX` for the integer types `uintX_t`, with `X` running over `{ 8, 16, 32, 64 }`. However, this would require a more thorough analysis of a good naming scheme that is both brief, intuitive, and without name clashes with other user-defined literals in the Standard Library. Furthermore, these fixed-size integers do not arise naturally when using the standard containers or algorithms. We therefore do not propose to add literal suffixes for the integer types in `<cstdint>`.   
  
3 Extended Example
------------------

**3.1** As an illustrative example enabled by this proposal, consider looping over a `vector` and accessing both the loop index `i` as well as the vector elements `v[i]`

    #include <cstddef>
    #include <vector>
    using namespace std::support_literals;
    
    int main()
    {
      auto v = std::vector<int> { 98, 03, 11, 14, 17 };
      
      // loop counter of type size_t, initialized to 0
      for (auto i = 0zu, s = v.size(); i < s; ++i) { 
        /* use both i and v[i] */ 
      }
    }

This coding style succinctly and safely caches the vector's size, similar to  the `end()` iterator's caching in a range-based `for` statement. This also fits nicely with the earlier mention of a left-to-right `auto` variable initialization, as recommended in [GotW #94](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/) and [Effective Modern C++, Item 5](http://shop.oreilly.com/product/0636920033707.do).

**3.2** As an aside, note that the above code example is not meant to imply a definitive style for all index-based `for` loops. E.g., this particular example might be improved by a range-based `for` statement that emits a `size_t` index deduced from a hypothetical zero-based `integral_range` object initialized with `v.size()`

    // not actually proposed here, loop over [ 0, v.size() )
    for (auto i : integral_range(v.size()) { /* ... */ }  

However, for non-zero-based integer ranges (e.g. when skipping the first element), the same type deduction issues would reappear, and it would become convenient to write

     // not actually proposed here, loop over [ 1, v.size() )
     for (auto i : integral_range(1zu, v.size()) { /* ... */ }

Regardless of the benefits of such a range-based approach for indexed `for` loops, we therefore argue that a user-defined suffix for `size_t` literals has its own merits. 

**3.3** Back to the code example. In the (rare) event that the vector's `size_type` is not equal to `size_t` (e.g. because of an exotic user-defined allocator), compilation will simply fail, so that no code will break *silently*. Under these circumstances (as well as in fully generic code), one has to rely on the rather verbose

    // required if the container size_type is not equal to size_t
    for (decltype(v.size()) i = 0, s = v.size(); i < s; ++i) { /* ... */ }
    
**3.4** Note that an `auto` version without any literal suffix comes with a lot of thorny issues (except for non-standard containers such as `QVector` for which the `size()` member function returns `int`)

    // not guaranteed to compile warning-free
    for (auto i = 0; i < v.size(); ++i) {     // -Wsign-compare
      std::cout << i << ": " << v[i] << '\n'; // -Wsign-conversion
    }   

First, the above code deduces `i` to be of type `int`, which means we cannot cache the vector's size (which is guaranteed of unsigned integer type) inside the loop's init-statement. Second, the above code triggers compiler warnings (shown for Clang and g++). Admittedly, those warnings are rather stringent. But they are not, in general, harmless. Furthermore, in many companies, developers are not free to adjust project-wide mandatory warning levels.

**3.5** It is tempting to assume that an `unsigned` literal is a safe alternative 

    // might lead to an infinite loop
    for (auto i = 0u; i < v.size(); ++i) { /* ... */ } 
    
Here, the literal `0u` will silence any sign-related warnings. However, whenever `v.size()` is beyond the range of an `unsigned` (e.g. more than `2^32` elements on most 64-bit systems) the loop variable `i` will wrap-around, never actually reaching the bound, leading to an infinite loop. 

**3.6** It is tempting to assume that an `unsigned long long` literal is a convenient alternative 

    // might not compile
    for (auto i = 0ull, s = v.size(); i < s; ++i) { /* ... */ } 
    
Here, the literal `0ull` (on most 64-bit systems) not lead to the infinite loop problem of section 3.5. However, `size_t` is implementation-defined and not guaranteed to be equal to `unsigned long long`. Hence, we cannot portably cache the vector's size inside the loop's init-statement. 

**3.7** A close and viable alternative to this proposal is to explicitly type the loop index

    // more verbose and no left-to-right initialization
    for (std::size_t i = 0, s = v.size(); i < s; ++i) { /* ... */ }
    
This works under the same circumstances as this proposal (with a fallback to `decltype(v.size())` for exotic containers or fully generic code). Its drawback is that it is more verbose, and that it forms an exception to the convenient left-to-right `auto` variable initialization that is available for both signed and unsigned integers. Admittedly, this is a matter of coding style, but this proposal does not *enforce* the use of `size_t` literals, it merely *enables* (as well as encourages) them.

4 Impact On the Standard
------------------------

**4.1** This proposal does not depend on other library components, and nothing depends on it. It is a pure library extension, but does require additions (though no modifications) to the standard header `<cstddef>`, as outlined in section 7 below. It can be implemented using C++14 compilers and libraries, and it does not require language or library features that are not part of C++14. In fact, this proposal is entirely implementable using only C++11 language features.

**4.2** There are, however, three active CWG issues ([cwg#1266](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1266), [cwg#1620](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1620) and [cwg#1735](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1735)) that could impact this proposal. All three issues note that in implementations with extended integer types, the decimal-literal in a user-defined integer literal might be too large for an `unsigned long long` to represent. Suggestions (but no formal proposals) were made to either fall back to a raw literal operator or a literal operator template, or to allow a parameter of an extended integer type. The latter suggestion would be easiest to incorporate into this proposal.

5 Survey of Existing Literals
-----------------------------

There are no (anticipated) conflicts with other literal suffixes, either for builtin types, other (proposed) Standard Library types, or in popular open source libraries such as Boost. 

**5.1** The literal suffixes for builtin integer types are described in Table 5 of `[lex.icon]/2`. These suffixes (`u` or `U` optionally followed by either `l` or `L` or by either `ll` or `LL`) do not contain the letter `z` and do not conflict with our proposal.

**5.2** The literal suffixes for builtin floating types are described in `[lex.fcon]/1`. These suffixes (one of `f`, `l`, `F`, `L`) do not contain the letter `z` and do not conflict with our proposal. 

**5.3** The Standard Library header `<chrono>` contains user-defined literal suffixes for time duration values, specified in `[time.duration.literals]`. The suffixes currently in use (`h`, `min`, `s`, `ms`, `us`, `ns`) do not contain the letter `z` and do not conflict with our proposal.

**5.4** The Standard Library header `<complex>` contains user-defined literal suffixes for complex numbers, specified in `[complex.literals]`. The suffixes currently in use (`il`, `i`, `if`) do not contain the letter `z` and do not conflict with our proposal.

**5.5** The Standard Library header `<string>` contains user-defined literal suffixes for strings, specified in `[basic.string.literals]`. The suffix currently in use (`s`) does not contain the letter `z` and does not conflict with our proposal. 

**5.6** The [Meta](https://ericniebler.github.io/meta/group__integral.html#gaddea0d053893b5bec6ba3d75af70624e) library defines a `_z` suffix for `std::integral_constant<size_t, N>` literals using the `template <char...> operator ""` overload (with `N` computed at compile-time from the template parameter pack). Even though Meta is a support library used in the reference implementation of the proposed Ranges Technical Specification [WG21/N4569](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4569.pdf), the Ranges TS does not rely on Meta's user-defined suffix `_z`. In fact, the lack of internal use of `_z` in Meta itself leads us to conclude that there is no anticipated conflict with our proposal.

**5.7** [Boost.Hana](http://www.boost.org/doc/libs/develop/libs/hana/doc/html/namespaceboost_1_1hana_1_1literals.html) exposes user-defined suffixes (`_c`, `_s`) for integral constant and compiletime string literals, and also internally defines user-defined literals (`_st`, `_nd`, `_rd`, `_th`) for tuple indexing. The suffixes in use do not conflict with our proposal.

**5.8** [Boost.Multiprecision](http://www.boost.org/doc/libs/1_60_0/libs/multiprecision/doc/html/boost_multiprecision/tut/lits.html) exposes user-defined suffixes(`_cppi`, `_cppui`, `_cppiN`, `_cppuiN`, with `N` an integral power of two) for high precision number literals, but these would not interfere with our proposal. 

**5.9** To the best of our knowledge, there are no other active WG21 proposals to add user-defined literals to the Standard Library. We are also not aware of any popular open source libraries (other than the aforementioned Meta, Boost.Hana and Boost.Multiprecision) that have user-defined literals that could conflict with our proposal. 

**5.10** Finally, note that `[usrlit.suffix]/1` states that literal suffix identifiers that do not start with an underscore are reserved for future standardization. This means that even if there were a popular open source library with a user-defined suffix `_zu`, there would only be a possible conflict with our proposed `zu` suffix for `size_t` literals if that suffix from a third-party library would also be accepted for standardization (see also section 6.4).

6 Design Decisions
------------------

**6.1** The proposed naming of the literal suffix `zu` was motivated by the `%zu` length modifiers for `size_t` formatted I/O in the C standard library header `<stdio.h>`. See 7.21.6.1/7 for `fprintf` and 7.21.6.2/11 `fscanf`, numbered relative to [WG14/N1539](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1539.pdf).

**6.2** The previous version of this paper ([WG21/N4254](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4254.html)) proposed the shorter suffix `z`. Based on feedback from LEWG, this has been changed to `zu`. The main rationale for this change is that `z` alone, is not the entirety of what’s needed in C I/O formatting.  The rather strong consensus was not to use only the modifier but to use the complete form.

**6.3** For purposes of bikeshedding, we note that other suffixes than the proposed `zu` such as (in order of this author's preference) the previously proposed `z`, `sz` or `uz` would also not conflict with existing literals. 

**6.4** The consequences of adopting the proposed literal suffix `zu` into the Standard are:

  - both novices and occasional programmers, as well as experienced library implementors, can use left-to-right `auto` variable initializations with `size_t` literals, without having to define their own literal suffix with leading underscore `_zu` in order to do so;
  - other existing or future Standard Library types (e.g. `chrono::duration` or `complex`) are prevented from adopting the same literal suffix, unless they use overloads of the corresponding `operator ""` that take arguments other than `unsigned long long` (because `[lex.ext]/3` gives these overloads lower precedence during overload resolution). 

**6.5** This proposal follows the existing practice established in [WG21/N3642](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf) with respect to the `constexpr` (present) and `noexcept` (absent) specifiers, as well as the usage of an appropriately named `inline namespace std::literals::support_literals`.

**6.6** There are no decisions left up to implementers, because the proposed wording forms a full specification. 

**6.7** There is a [reference implementation](https://bitbucket.org/rhalbersma/xstd/src/41b35cd8db50a529e426bae8f2669283247c1b6d/include/xstd/cstddef.hpp?at=default) and small [test suite](https://bitbucket.org/rhalbersma/xstd/src/41b35cd8db50a529e426bae8f2669283247c1b6d/test/src/cstddef.cpp?at=default) available for inspection. Note that the reference implementation uses `namespace xstd` and underscored suffix `_zu` because of the restriction from `[lex.ext]/10` that a program containing a user-defined suffix without an underscore is ill-formed, no diagnostic required. 

**6.8** This proposal successfully compiles and runs on g++ >= 4.7.3, clang >= 3.1 and Visual C++ >= 2015 (possibly on earlier versions of Visual C++ if `constexpr` literals are not used).  

7 Proposed Wording
------------------

**7.1** Insert in subclause `[support.types]/1` in the synopsis of header `<cstddef>` at the appropriate place the namespace `std::literals::support_literals`: 
      
    namespace std {
      inline namespace literals {
        inline namespace support_literals {
          constexpr size_t operator "" zu(unsigned long long);                         
        }
      }
    }

**7.2** Insert a new subclause `[support.literals]` between `[support.types]` and `[support.limits]` as follows (numbered relative to [WG21/N4582](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4582.pdf)):

> **18.3 Suffixes for support types [support.literals]**
>
> 1 This section describes a literal suffix for constructing `size_t` literals. The suffix `zu` creates numbers of type `size_t`. 
>
>     constexpr size_t operator "" zu(unsigned long long u);
> 2 Returns: `static_cast<size_t>(u)`.

8 Acknowledgments
-----------------

We gratefully acknowledge Walter E. Brown for acting as our *locum* in committee meetings and for his valuable feedback. We also acknowledge feedback on the previous version of this proposal from Jerry Coffin and Andy Prowl on `<Lounge C++>`, guidance from Daniel Krügler, as well as input from various participants on `std-proposals`.

9 References
------------

`[std-proposals]`: Morwenn Edrahir, *User defined literal for size_t* [https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo) 

`[N3642]`: Peter Sommerlad, *User-defined Literals for Standard Library Types (part 1 - version 4)* [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf)

`[N4254]`; Rein Halbersma, *User-defined Literals for size_t* [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4254.html](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4254.html)

`[N4569]`: Eric Niebler, *Working Draft, C++ Extensions for Ranges* [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4569.pdf](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4569.pdf)

`[Meta]`: Eric Niebler, *A tiny metaprogramming library* [https://ericniebler.github.io/meta/group__integral.html#gaddea0d053893b5bec6ba3d75af70624e](https://ericniebler.github.io/meta/group__integral.html#gaddea0d053893b5bec6ba3d75af70624e)

`[Boost.Hana]`: Louis Dionne, *A modern C++ metaprogramming library* [http://www.boost.org/doc/libs/develop/libs/hana/doc/html/namespaceboost_1_1hana_1_1literals.html](http://www.boost.org/doc/libs/develop/libs/hana/doc/html/namespaceboost_1_1hana_1_1literals.html)

`[Boost.Multiprecision]`: John Maddock and Christopher Kormanyos, *Extended precision arithmetic types for floating point, integer and rational arithmetic* [http://www.boost.org/doc/libs/1_60_0/libs/multiprecision/doc/html/boost_multiprecision/tut/lits.html](http://www.boost.org/doc/libs/1_60_0/libs/multiprecision/doc/html/boost_multiprecision/tut/lits.html)

`[GotW #94]`: Herb Sutter, *AAA Style (Almost Always Auto)* [http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/)

`[Effective Modern C++]`: Scott Meyers, *42 Specific Ways to Improve Your Use of C++11 and C++14* (*Item 5: Prefer auto to explicit type declarations.*) [http://shop.oreilly.com/product/0636920033707.do](http://shop.oreilly.com/product/0636920033707.do)

10 Document History
-------------------

  1. 2014-11-21; Published as N4254.
  2. 2016-05-08; Summarized WG21's view re N4254; dropped the proposed suffix for `ptrdiff_t`; changed the proposed suffix for `size_t` to `zu`; added survey of existing literals.