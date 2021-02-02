# swl-tuple
Proof of concept implementation of std::tuple, using a lambda as storage. The smallest (350 lines), fastest tuple implementation to compile (about 20 times faster than the standard library, between 2 and 7 times faster than [tao::tuple](https://github.com/taocpp/tuple)) in the Wild Web. 

I've written an article about it [here](https://groundswellaudio.github.io/posts/cpp_lambda_tuple/).

## Requirements

This code is written in C++20. It's implementable in pre-C++20, if you replace all the `requires` with SFINAE.

## Disclaimer 

This implementation is not, as of yet, completely standard conforming. 