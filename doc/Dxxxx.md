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

**[Example 1]** Consider the following simple program that prints both the index and an array element's content: 

    #include <array>
    #include <iostream>

    int main()
    {
      auto a = std::array<int, 2> {{ 1, 2 }}; 
      for (auto i = 0; i < a.size(); ++i)       // -Wsign-compare 
        std::cout << i << ": " << a[i] << "\n"; // -Wsign-conversion
    }

The program uses an [Almost Always Auto style](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/) with a `for (auto i = 0; ...)` loop initialization. From its initializer `0`, the type of the variable `i` is deduced to be `int`. However, the subsequent comparison against `a.size()` of type `size_t` and the accessing of `std::array::operator[]` that takes a `size_t` argument, produces two warnings (using either the `gcc` or `clang` compiler at the highest warning levels) against mixed sign comparisons and sign conversions. One might consider these warning overly pedantic, but many codebases have mandatory warning levels that cannot be overriden by users.

It is straightforward to rewrite the above code to eliminate the sign comparison and conversion warnings. One can explicitly type either the variable `i` or its initializer `0`: 

    for (std::size_t i = 0; i < a.size(); ++i)              // OK, explicitly typed variable
    
    for (decltype(a.size()) i = 0; i < a.size(); ++i)       // OK, for fully generic code

    for (auto i = std::size_t{0}; i < a.size(); ++i)        // OK, explicitly typed initializer

    for (auto i = decltype(a.size()){0}; i < a.size(); ++i) // OK, for fully generic code

These alternatives either force users to abandon the convenient left-to-right declaration of `auto` variable initialization, or significantly increase code verbosity. Using a `z` literal would preserve the left-to-right declaration of the `auto` initialization with minimal verbosity:

    for (auto i = 0z; i < a.size(); ++i)                    // OK, literal suffix explicitly types the initializer

**[Example 2]** Another example comes from the fact that the return type of `std::accumulate` is given by the type of its initializer argument:

    #include <algorithm>
    #include <array>
    #include <cstddef>
    #include <iostream>
    #include <numeric>
    
    int main()
    {
      auto a = std::array<std::size_t, 2> {{ 1, 2 }}; 
      auto sum = std::accumulate(a.begin(), a.end(), 0);
      auto check = std::all_of(a.begin(), a.end(), [&](auto x) { 
        return x <= sum; // -Wsign-compare
      });
      std::cout << check;
    }

From its initializer `0`, the type of the variable `sum` is deduced to be `int`. The subsequent comparison of this sum against the array elements of type `size_t` will produce a sign comparison warning. This proposal would silence this warning by allowing the compact 
 
    std::accumulate(a.begin(), a.end(), 0z)
    
instead of requiring one of the more verbose alternatives
 
    std::accumulate(a.begin(), a.end(), std::size_t{0})

    std::accumulate(a.begin(), a.end(), decltype(*a.begin()){0})
  
**[Example 3]** An example involving `ptrdiff_t` can also be given by once more abusing the deduced return type of `std::accumulate`
        
    #include <array>
    #include <cstddef>
    #include <iostream>
    #include <limits>
    #include <numeric>

    int main()
    {
      std::ptrdiff_t m = std::numeric_limits<int>::max(); 
      auto a = std::array<std::ptrdiff_t, 2> {{ m, 1 }};  
      auto sum = std::accumulate(a.begin(), a.end(), 0); // possibly undefined behavior
      auto check = sum == m + 1;
      std::cout << check;
    }
   
From its initializer `0`, the type of the variable `sum` is deduced to be `int`. Because `ptrdiff_t` is a signed integral type, the `sum == m + 1` expression will not lead to a sign comparison warning. However, if `int` has a smaller conversion rank than `ptrdiff_t`, the sum of the particular array elements will cause signed integer overflow, and undefined behavior ensues. This proposal would eliminate the possible undefined behavior by allowing the compact 
 
    std::accumulate(a.begin(), a.end(), 0t)
    
instead of requiering one of the more verbose alternatives
 
    std::accumulate(a.begin(), a.end(), std::ptrdiff_t{0})
            
    std::accumulate(a.begin(), a.end(), decltype(*a.begin()){0})

Impact On the Standard
----------------------

This proposal does not depend on other library components, and nothing depends on it. It is a pure extension, but does require additions (though no modifications) to the standard header `<cstddef>`, as outlined in the section **Proposed Wording** below. It can be implemented using C++11 compilers and libraries, and it does not require language or library features that are not part of C++11. 

Design Decisions
----------------

The chosen naming of the literal suffixes `z` and `t` was motivated by the corresponding length modifiers for formatted I/O in the C standard library header `<stdio.h>`. See 7.21.6.1/7 for `fprintf` and 7.21.6.2/11 `fscanf`, numbered relative to [WG14/N1539](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1539.pdf).

    #include <cstddef>
    #include <cstdio>
    #include <limits>

    int main()
    {
      auto const s = std::numeric_limits<std::size_t>::max(); 
      auto const p = std::numeric_limits<std::ptrdiff_t>::min();
      printf("%zu %td", s, p);
    }

Alternative literal suffixes could be `zu` and `td`, combining the length modifiers with the corresponding conversion specifiers. This has the advantage of not monopolizing single-letter suffixes for future use, while still keeping the correspondence with formatted input/output in the C standard library, at the cost of slightly longer typing.

Yet another set of literal suffixes would be `sz` and `pd`, or even `szt` and `pdt`. Such names are perhaps more recognizable to novices and occasional programmers, but they are inconsistent with existing naming conventions for formatted I/O in the C standard library. As a side note: people who are accustomed to Hungarian notation might even confuse `sz` with "string, zero-terminated". 

This proposal follows the existing practice established in [WG21/N3642](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf) with respect to the `constexpr` (present) and `noexcept` (absent) specifiers, as well as the usage of an appropriately named `inline namespace std::literals::support_literals`.

One might argue that the Example 1 does not apply to the Standard Containers because the type of `c.size()` is `c::size_type` which is not guaranteed to be equal to `size_t`.  Fully generic code would indeed need `decltype(c.size())` as a type specifier. In non-generic code, however, `c.size()` can usually be relied on to be of type `size_t`, unless one is either using a container with exotic iterators or pointers, or using a user-defined allocator. 

An alternative resolution to the indexed-based `for` statement discussed in Example 1 might be a range-based `for` statement with a library function `sequence` that performs type deduction on its upper-bound:

    for (auto i : sequence(0, a.size())) { ... } // i initialized with decltype(a.size()){0}

An alternative resolution for the deduced return type for `std::accumulate` discussed in Examples 2 and 3 might be an overloaded function template that has return type `std::iterator_traits<InputIterator>::value_type`, and that value-initializes its return value: 

    auto sum = accumulate(a.begin(), a.end());   // summation initialized with decltype(*a.begin()){}

Regardless of the merits of such alternative library proposals (and the latter only works with `0` as the initializer), we consider these outside the scope of the current proposal and deem the proposed user-defined literals useful in their own right.    

The consequences of adopting the proposed literal suffixes into the Standard are:

  - both novices and occasional programmers, as well as experienced library implementors, can use left-to-right `auto` variable initializations with `size_t` and `ptrdiff_t` literals, without having to define their own literal suffixes with leading underscores `_z` and `_t` (or any of the mentioned alternatives) in order to do so;
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



