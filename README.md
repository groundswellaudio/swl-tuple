# swl-tuple
Proof of concept implementation of a tuple, using a lambda as storage. On concatenation heavy code, it compiles about 20 times faster than the std, and between 2 and 7 times faster than [tao::tuple](https://github.com/taocpp/tuple). However, using the lambda capture as memory means that the members cannot be constructed in place, they have to be copied/moved, so this technic cannot be used to implement a standard conforming tuple (and this tuple cannot be used to with non-movable, non-copyable objects).

I've written an article about it [here](https://groundswellaudio.github.io/posts/cpp_lambda_tuple/).

## Usage

In order to use structured binding, include "swl/tuple/binding.hpp". The reason for that is defining the traits necessary for tuple-like structured binding requires including \<utility>\, which is a comparatively big header. 

## Requirements

This code is written in C++20. It's implementable in pre-C++20, if you replace all the `requires` with SFINAE.

## Runtime performance

On informal benchmarks of concatenation code (using Clang) this implementation was : 
* 13x faster than std::tuple on -O0, 2x faster on -O3
* 2x faster than tao::tuple on -O0, but 10% slower on -O3
