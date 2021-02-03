
#include "../swl/tuple/tuple.hpp"
#include "../swl/tuple/binding.hpp"

#include <cassert>


#include <string>
#include <vector>

template <class T> struct identity { using type = T; };

template <class... Args>
constexpr void lvalue_ref_check( identity<swl::tuple<Args...>> ){
	static_assert( (std::is_lvalue_reference_v<Args> && ...) );
}

int main(){

	
	// ============================= relations 
	
	static_assert( swl::make_tuple(1, 2, 4) > swl::make_tuple(1, 2, 3) );
	
	static_assert( swl::make_tuple(3.4, 2.4, -1) > swl::make_tuple(1, 0, 5.6) );
	
	static_assert( swl::make_tuple(1, 2, 4) >= swl::make_tuple(1, 2, 3) ); 
	
	static_assert( swl::make_tuple(1, 1, 1) <= swl::make_tuple(1, 1, 1.1f) );
	
	static_assert( swl::make_tuple(1, 2, 2, 5) == swl::make_tuple(1, 2, 2, 5) );
	static_assert( swl::forward_as_tuple(1, 2, 2) == swl::forward_as_tuple(1, 2, 2) );
	static_assert( swl::make_tuple(888) != swl::make_tuple(-1.22) );
	
	static_assert( swl::make_tuple(-55, nullptr, 100) != swl::make_tuple(-55, nullptr, 99) );
	
	swl::tuple<std::string, float, char**> tpl;
	
	// ============================== value category 
	
	static_assert( std::is_rvalue_reference_v< decltype( (get<0>(std::move(tpl))) ) > );
	static_assert( std::is_lvalue_reference_v< decltype( (get<0>(tpl)) ) > );
	
	int a, b;
	
	auto ref_tpl = swl::tie(a, b);
	
	lvalue_ref_check( identity<decltype(ref_tpl)>{} );
	
	// ============================== runtime 
	get<0>(tpl) = "testing...";
	
	assert( get<0>(tpl)  == "testing..." );
	
	auto concat = swl::tuple_cat( tpl, tpl, std::move(tpl), swl::make_tuple(1, 2, 3) );
	
	static_assert( std::tuple_size_v< decltype(concat) > == 12 );
	
	assert( get<0>(concat) == "testing..." && 
			get<0>(concat) == get<3>(concat) 
			&& get<0>(concat) == get<6>(concat) );
			
	std::vector<int> vec = {1, 2, 3, 4, 5};
	auto vec_copy = vec;
	
	int x = 1000;
	float* z = nullptr;
	
	swl::tuple<std::vector<int>, int&, float*&> tpl2 { std::move(vec), x, z };
	
	assert( vec.size() == 0 ); // vector should have moved
	assert( (get<0>(tpl2) == vec_copy) );
	
	float ptr;
	
	get<1>(tpl2) = 313;
	get<2>(tpl2) = &ptr;
	
	assert( x == 313 );
	assert( z == &ptr );
}