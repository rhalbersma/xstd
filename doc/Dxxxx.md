    Document number: Dxxxx=14-xxxx
    Date:            2014-10-20
    Project:         Programming Language C++, Library Working Group
    Reply-to:        Rein Halbersma <rhalbersma@gmail.com>

User-Defined Literals for `ptrdiff_t` and `size_t`
==================================================

Introduction
------------

We propose the user-defined suffixes `t` and `z` for `ptrdiff_t` and `size_t` literals, respectively.

Motivation and Scope
--------------------

**Rationale** The main motivations for this proposal are:

  - `int` is the default type deduced from integer literals;
  - `ptrdiff_t` and especially `size_t` are almost unavoidable when using the Standard Library;
  - mixed comparisons and arithmetic of these support types with `int` can lead to compiler warnings or undiagnosed undefined behavior;
  - explicitly typing `ptrdiff_t` and `size_t` literals is currently overly verbose;
  - suffixes `t` and `z` are consisent with the length modifiers from the formatted input/output in the C standard library.  

**Example 1** Consider a simple program that loops over a `std::array<int, 2>` and prints both the index and element's content to the standard output. 

    #include <array>
    #include <iostream>

    int main()
    {
      auto const a = std::array<int, 2> {{ 1, 2 }}; 
      for (auto i = 0; i < a.size(); ++i)               // -Wsign-compare 
        std::cout << i << ": " << a[i] << "\n";         // -Wsign-conversion
    }

The program uses an [Almost Always Auto style](http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/) with a `for (auto i = 0; ...)` loop initialization. From its initializer `0`, the type of the variable `i` is deduced to be `int`. However, the subsequent comparison against `a.size()` of type `size_t` and the accessing of `std::array::operator[]` that takes a `size_t` argument, produces two warnings (using either the `gcc` or `clang` compiler at the highest warning levels) against mixed sign comparisons and sign conversions. One might consider these warning overly pedantic, but many codebases have mandatory warning levels.

It is straightforward to rewrite the above code to eliminate the sign comparison and conversion warnings. One can explicitly type either the variable `i` or its initializer `0`: 

      for (std::size_t i = 0; i < a.size(); ++i)        // OK, explicitly typed variable
    
      for (auto i = std::size_t{0}; i < a.size(); ++i)  // OK, explicitly typed initializer

The former approach forces users to abandon the convenient left-to-right declaration of the `auto` initialization. The latter approach is rather verbose. Using a `z` literal would preserve the left-to-right declaration of the `auto` initialization with minimal verbosity:

      for (auto i = 0z; i < a.size(); ++i)              // OK, literal suffix explicitly types the initializer

One might argue that the above code does not apply to the Standard Containers because the type of `c.size()` is `c::size_type` which is not guaranteed to be equal to `size_t`.  Fully generic code would indeed need a `for (decltype(c.size()) i = 0; ...)` loop initialization. In non-generic code, however, `c.size()` can usually be relied on to be of type `size_t`, unless one is using a container with exotic iterators, pointers or a user-defined allocator. 

**Example 2** Another example comes from the fact that the return type of `std::accumulate` is given by the type of its initializer argument:

    #include <algorithm>
    #include <array>
    #include <cstddef>
    #include <iostream>
    #include <numeric>
    
    int main()
    {
      auto const a = std::array<std::size_t, 2> {{ 1, 2 }}; 
      auto const sum = std::accumulate(a.begin(), a.end(), 0);
      auto const check = std::all_of(a.begin(), a.end(), [&](auto x) { 
        return x <= sum; // -Wsign-compare
      });
      std::cout << check;
    }

From its initializer `0`, the type of the variable `sum` is deduced to be `int`. The subsequent comparison of this sum against the array elements of type `size_t` will produce a sign comparison warning. This proposal would silence this warning by allowing the compact 
 
    std::accumulate(a.begin(), a.end(), 0z)
    
instead of requiring one of the more verbose alternatives
 
    std::accumulate(a.begin(), a.end(), std::size_t{0})

    std::accumulate(a.begin(), a.end(), decltype(*a.begin()){0})
  
**Example 3** An example involving `ptrdiff_t` can also be given by once more abusing the deduced return type of `std::accumulate`
        
    #include <array>
    #include <cstddef>
    #include <iostream>
    #include <limits>
    #include <numeric>

    int main()
    {
      auto const m = std::numeric_limits<ptrdiff_t>::max(); 
      auto const a = std::array<std::ptrdiff_t, 2> {{ m - 1, 1 }};  
      auto const sum = std::accumulate(a.begin(), a.end(), 0); // possibly undefined behavior
      auto const check = sum == m;
      std::cout << check;
    }
   
From its initializer `0`, the type of the variable `sum` is deduced to be `int`. Because `ptrdiff_t` is a signed integral type, the `sum ==  m` expression will not lead to a sign comparison warning. However, if `int` is a smaller type than `ptrdiff_t`, the sum of the particular array elements will cause signed integer overflow, and undefined behavior ensues. This proposal would eliminate the possible undefined behavior by allowing the compact 
 
    std::accumulate(a.begin(), a.end(), 0t)
    
instead of requiering one of the more verbose alternatives
 
    std::accumulate(a.begin(), a.end(), std::ptrdiff_t{0})
            
    std::accumulate(a.begin(), a.end(), decltype(*a.begin()){0})

**Existing practice** We are not aware of similar functionality being in use. There is a [reference implementation](https://bitbucket.org/rhalbersma/xstd/src/78bbe4276f98c404575f35e82e8e9532fd985eb2/include/xstd/cstddef.hpp?at=default#cl-8) and small [test suite](https://bitbucket.org/rhalbersma/xstd/src/78bbe4276f98c404575f35e82e8e9532fd985eb2/test/src/cstddef.cpp?at=default) available for inspection. Note that the reference implementation uses `namespace xstd` and underscored suffixes `_t` and `_z` because the tested compiler `clang` will enforce the restriction from `[lex.ext]/10` that a program containing a user-defined suffix without an underscore is ill-formed, no diagnostic required.   

Impact On the Standard
----------------------

This proposal does not depend on other library components, and nothing depends on it. It is a pure extension, but does require additions (though no modifications) to standard header `<cstddef>`, as outlined in the section **Proposed Wording** below. It can be implemented using C++11 compilers and libraries, and it does not require language or library features that are not part of C++11. 

Design Decisions
----------------

The chosen naming of the literal suffixes `t` and `z` was motivated by the corresponding length modifiers `t` and `z` for the formatted input/output functions from the C standard library header `<stdio.h>`. See 7.21.6.1/7 for `fprintf` and 7.21.6.2/11 `fscanf`, numbered relative to [WG14/N1539](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1539.pdf).

    #include <cstddef>
    #include <cstdio>
    #include <limits>

    int main()
    {
      auto const p = std::numeric_limits<std::ptrdiff_t>::min();
      auto const s = std::numeric_limits<std::size_t>::max(); 
      printf("%td %zu", p, s);
    }

Alternative literal suffixes could be `td` and `zu`, combining the length modifiers `t` and `z` with the corresponding conversion specifiers `d` and `u`. This has the advantage of not monopolizing single-letter suffixes for future use, while still keeping the correspondence with formatted input/output in the C standard library, at the cost of slightly longer typing.

Yet another set of literal suffixes would be `pd` and `sz`, or even `pdt` and `szt`. The tradeoffs are that although such names are perhaps more recognizable to novices and occasional programmers, they are slightly longer to type and, more importantly, inconsistent with existing naming conventions for formatted input/output in the C standard library. 

This proposal follows the existing practice established in [WG21/N3642](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf) with respect to the `constexpr` (present) and `noexcept` (absent) specifiers, as well as the usage of an appropriately named `inline namespace std::literals::support_literals`. 

The consequences of adopting the proposed literal suffixes into the Standard are:

  - both novices and occasional programmers, as well as expericence library implementors, can use left-to-right `auto` variable initializations with `ptrdiff_t` and `size_t` literals, without having to define their own literal suffixes with leading underscores `_t` and `_z` (or any of the mentioned alternatives) in order to do so;
  - other existing or future Standard Library types are prevented from adopting the same literal suffixes, unless they use overloads of the corresponding `operator""` that take arguments other than `unsigned long long`. 

There are no decisions left up to implementers, because the suggested wording below fully specifies the proposed functionality. We are not aware of similar libraries in use.

Proposed Wording
----------------

Insert in subclause `[support.types]/1` in the synopsis of header `<cstddef>` at the appropriate place the namespace `std::literals::support_literals`: 

>     namespace std {
      inline namespace literals {
        inline namespace support_literals {
          constexpr ptrdiff_t operator "" t(unsigned long long);        
          constexpr size_t operator "" z(unsigned long long);       
        }
      }
    }

Insert a new subclause `[support.literals]` between `[support.types]` and `[support.limits]` as follows (numered relative to [WG21/N4140](https://github.com/cplusplus/draft/blob/master/papers/n4140.pdf)):

> **18.3 Suffixes for support types [support.literals]**
>
> 1 This section describes literal suffixes for constructing `ptrdiff_t` and `size_t` literals. The suffixes `t` and `z` create numbers of the types `ptrdiff_t` and `size_t`, respectively. 
>
>     constexpr ptrdiff_t operator "" t(unsigned long long u);        
>2 Returns: `static_cast<ptrdiff_t>(u)`.
>
>     constexpr size_t operator "" z(unsigned long long u);
>3 Returns: `static_cast<size_t>(u)`.

Acknowledgments
----------------

This section intentionally left blank.

References
----------

This section intentionally left blank.

