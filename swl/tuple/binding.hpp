
#ifndef SWL_CPP_LIBRARY_TUPLE_STRUCT_BINDING_HEADER
#define SWL_CPP_LIBRARY_TUPLE_STRUCT_BINDING_HEADER

#include <utility>

namespace swl {

namespace {
	template <class T> struct identity { using type = T; };
		
	template <std::size_t Idx, class Head, class... Ts>
	constexpr auto type_at_index(){
		if constexpr (Idx == 0) return identity<Head>{};
		else return type_at_index<Idx - 1, Ts...>();
	}
} 

template <class... Ts>
class tuple;

} // SWL

// =================================================================================

namespace std {
	
	template <class... Ts>
	class tuple_size<::swl::tuple<Ts...>> {
		public : static constexpr std::size_t value = sizeof...(Ts);
	};
	
	template <std::size_t Idx, class... Ts>
	class tuple_element<Idx, ::swl::tuple<Ts...>> { 
		public : using type = typename decltype( ::swl::type_at_index<Idx, Ts...>() )::type;
	};

} // STD

#endif