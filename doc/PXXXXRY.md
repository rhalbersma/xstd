    Document number: PXXXXRY
    Date:            2016-05-05
    Project:         Programming Language C++, Library Evolution Working Group
    Reply-to:        Rein Halbersma <rhalbersma at gmail dot com>

User-Defined Literals for `size_t`
====================================================

Introduction
------------

Following an earlier [discussion](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo) on `std-proposals`, we propose the user-defined suffix `zu` for `size_t` literals. This allows the convenient left-to-right `auto` variable initialization:
    
    auto s = 0zu; // s has type size_t

Motivation and Scope
--------------------

The main motivations for this proposal are:

  - `int` is the default type deduced from integer literals without suffix;
  - `size_t` is almost unavoidable when using the standard containers element access or `size()` member functions;
  - comparisons and arithmetic with integer types of mixed signs or different conversion ranks can lead to surprises;
  - surprises range from (pedantic) compiler warnings, value conversions, or even undefined behavior;
  - eliminating these surprises by explicitly typing or casting `size_t` literals is rather verbose;
  - user-defined literals are a succinct and type-safe way to express coding intent;
  - the suffix `zu` is consistent with the `%zu` length modifiers for `size_t` formatted I/O in the C standard library (see also the section Design Decisions).
  
The scope of this proposal is limited to a literal suffix for the support type `size_t` in the Standard Library header `<cstddef>`.

Note that a technically similar proposal could be made for literal suffixes for the integer types in the Standard Library header `<cstdint>`, such as literal suffixes `uX` for the integer types `uintX_t`, with `X` running over `{ 8, 16, 32, 64 }`. However, this would require a more thorough analysis of a good naming scheme that is both brief, intuitive, and without name clashes with other user-defined literals in the Standard Library. Furthermore, these fixed-size integers do not arise naturally when using the standard containers or algorithms. We therefore do not propose to add literal suffixes for the integer types in `<cstdint>`.   
  
Extended Example
----------------

**a)** As an illustrative example, consider looping over a `vector` and accessing both the loop index `i` as well as the vector elements `v[i]`

    #include <cstddef>
    #include <vector>
    using namespace std::support_literals;
    
    int main()
    {
      auto v = std::vector<int> { 98, 03, 11, 14, 17 };
      for (auto i = 0zu, s = v.size(); i < s; ++i) { 
        /* use both i and v[i] */ 
      }
    }

This coding style succinctly and safely caches the vector's size, similar to  the `end()` iterator's caching in a range-based `for` statement. This also fits nicely with the earlier mention of a left-to-right `auto` variable initialization, as recommended in [GotW #94](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/) and [Effective Modern C++, Item 5](http://shop.oreilly.com/product/0636920033707.do).

**b)** As an aside, note that the above code example is not meant to imply a definitive style for all index-based `for` loops. E.g., this particular example might be improved by a range-based `for` statement that emits a `size_t` index deduced from a zero-based integer range object initialized from `v.size()`

    // not actually proposed here
    for (auto i : std::integral_range(v.size()) { /* ... */ }  

However, for non-zero-based integer ranges (e.g. when skipping the first element), the same type deduction issues would reappear, and it would become convenient to write

     // not actually proposed here
     for (auto i : std::integral_range(1zu, v.size()) { /* ... */ }

Regardless of the benefits of such a range-based approach for indexed `for` loops, we therefore argue that a user-defined literal suffix for `size_t` has its own merits. 

**c)** Back to the code example. In the event that the vector's `size_type` is not equal to `size_t` (e.g. because of an exotic user-defined allocator), compilation will simply fail, so that no code will break *silently*. Under these circumstances (as well as in fully generic code), one has to rely on the rather verbose

    for (decltype(v.size()) i = 0, s = v.size(); i < s; ++i) { /* ... */ }
    
**d)** Note that an `auto` version without any literal suffix comes with a lot of thorny issues (except for non-standard containers such as `QVector` for which the `size()` member function returns `int`)

    for (auto i = 0; i < v.size(); ++i) {     // -Wsign-compare
      std::cout << i << ": " << v[i] << '\n'; // -Wsign-conversion
    }   

First, the above code deduces `i` to be of type `int`, which means we cannot cache the vector's size (which is guaranteed of unsigned integer type) inside the loop's init-statement. Second, the above code triggers compiler warnings (shown for Clang and g++). Admittedly, those warnings are rather stringent. But they are not, in general, harmless. Furthermore, in many places, developers are not free to adjust project-wide mandatory warning levels.

**e)** It is tempting to assume that an `unsigned` literal is a safe alternative 

    for (auto i = 0u; i < v.size(); ++i) { /* ... */ } 
    
Here, the literal `0u` will silence any sign-related warnings. However, the above might entail undefined behavior (with no diagnostic required!) whenever `v.size()` is beyond the range of an `unsigned` (e.g. more than `2^32` elements on most 64-bit systems) since then the loop variable `i` will wrap-around, never actually reaching the bound. 

Preliminary tests with Clang and g++ indicate that in practice no diagnostics will be given, unless the loop's bound comes from a `constexpr size()` member function of a `constexpr` container object. Note that this can only be satisfied by the stack-based `std::array`, which is not likely to have more than `2^32` elements in the first place.

**f)** A close and viable alternative to this proposal is to explicitly type the loop index

    for (std::size_t i = 0, s = v.size(); i < s; ++i) { /* ... */ }
    
This works under the same circumstances as this proposal (with a fallback to `decltype(v.size())` for exotic containers or fully generic code). Its drawback is that it is more verbose, and that it forms an exception to the convenient left-to-right `auto` variable initialization that is available for both signed and unsigned integers. Admittedly, this is a matter of coding style, but this proposal does not *enforce* the use of `size_t` literals, it merely *enables* (as well as encourages) them.

Impact On the Standard
----------------------

This proposal does not depend on other library components, and nothing depends on it. It is a pure extension, but does require additions (though no modifications) to the standard header `<cstddef>`, as outlined in the section Proposed Wording below. It can be implemented using C++14 compilers and libraries, and it does not require language or library features that are not part of C++14. In fact, this proposal is entirely implementable using only C++11 language features.

There are, however, three active CWG issues ([cwg#1266](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1266), [cwg#1620](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1620) and [cwg#1735](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1735)) that could impact this proposal. All three issues note that in implementations with extended integer types, the decimal-literal in a user-defined integer literal might be too large for an `unsigned long long` to represent. Suggestions (but no formal proposals) were made to either fall back to a raw literal operator or a literal operator template, or to allow a parameter of an extended integer type. The latter suggestion would be easiest to incorporate into this proposal.

Design Decisions
----------------

The chosen naming of the literal suffix `zu` was motivated by the `%zu` length modifiers for `size_t` formatted I/O in the C standard library header `<stdio.h>`. See 7.21.6.1/7 for `fprintf` and 7.21.6.2/11 `fscanf`, numbered relative to [WG14/N1539](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1539.pdf).

The consequences of adopting the proposed literal suffix into the Standard are:

  - both novices and occasional programmers, as well as experienced library implementors, can use left-to-right `auto` variable initializations with `size_t` literals, without having to define their own literal suffix with leading underscore `_zu` in order to do so;
  - other existing or future Standard Library types are prevented from adopting the same literal suffix, unless they use overloads of the corresponding `operator ""` that take arguments other than `unsigned long long`. 

This proposal follows the existing practice established in [WG21/N3642](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf) with respect to the `constexpr` (present) and `noexcept` (absent) specifiers, as well as the usage of an appropriately named `inline namespace std::literals::support_literals`.

There are no decisions left up to implementers, because the proposed wording forms a full specification. We are not aware of similar libraries in use. There is a [reference implementation](https://bitbucket.org/rhalbersma/xstd/src/3ddfa8e9d24a0349b875709e7b609568d7684d9d/include/xstd/cstddef.hpp?at=default) and small [test suite](https://bitbucket.org/rhalbersma/xstd/src/3ddfa8e9d24a0349b875709e7b609568d7684d9d/test/src/cstddef.cpp?at=default) available for inspection. Note that the reference implementation uses `namespace xstd` and underscored suffix `_zu` because the tested compiler `Clang` will enforce the restriction from `[lex.ext]/10` that a program containing a user-defined suffix without an underscore is ill-formed, no diagnostic required.   

Proposed Wording
----------------

Insert in subclause `[support.types]/1` in the synopsis of header `<cstddef>` at the appropriate place the namespace `std::literals::support_literals`: 
      
    namespace std {
      inline namespace literals {
        inline namespace support_literals {
          constexpr size_t operator "" zu(unsigned long long);                         
        }
      }
    }

Insert a new subclause `[support.literals]` between `[support.types]` and `[support.limits]` as follows (numbered relative to [WG21/N4582](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4582.pdf)):

> **18.3 Suffixes for support types [support.literals]**
>
> 1 This section describes a literal suffix for constructing `size_t` literals. The suffix `zu` creates numbers of type `size_t`. 
>
>     constexpr size_t operator "" zu(unsigned long long u);
> 2 Returns: `static_cast<size_t>(u)`.

Acknowledgments
----------------

We gratefully acknowledge feedback from Jerry Coffin and Andy Prowl on `<Lounge C++>`, guidance from Daniel Krügler, as well as input from various participants on `std-proposals`.

References
----------

`[std-proposals]`: Morwenn Edrahir, *User defined literal for size_t* [https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/tGoPjUeHlKo) 

`[N3642]`: Peter Sommerlad, *User-defined Literals for Standard Library Types (part 1 - version 4)* [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf)

`[GotW #94]`: Herb Sutter, *AAA Style (Almost Always Auto)* [http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/)

`[Effective Modern C++]`: Scott Meyers, *42 Specific Ways to Improve Your Use of C++11 and C++14* (*Item 5: Prefer auto to explicit type declarations.*) [http://shop.oreilly.com/product/0636920033707.do](http://shop.oreilly.com/product/0636920033707.do)