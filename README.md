# swl-tuple
Proof of concept implementation of std::tuple, using a lambda as storage. The smallest (350 lines), fastest tuple implementation to compile in the Wild Web. 


## Requirements

This code is written in C++20. It's implementable in pre-C++20, if you replace all the `requires` with SFINAE.

## Disclaimer 

This implementation is not, as of yet, completely standard conforming. 