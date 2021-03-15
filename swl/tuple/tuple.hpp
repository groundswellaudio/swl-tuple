
#ifndef SWL_CPP_LIBRARY_TUPLE_MAIN_HEADER
#define SWL_CPP_LIBRARY_TUPLE_MAIN_HEADER

#include <type_traits>

namespace swl {

template <class... Ts>
class tuple;

namespace impl {
	
	
	/* 
	namespace debug {
		template <class... Args>
		inline constexpr bool always_false = false;

		template <class... Args>
		struct show_type {
			static_assert( always_false<Args...> );
		};
	} // DEBUG   */ 
	
	namespace meta {
		
		template <class... Args>
		struct list_cat;

		template <template <class...> class W, class... Ts>
		struct list_cat<W<Ts...>>{
			using type = W<Ts...>;
		};

		template <template <class...> class W,
				class... A, class... B, class... Rest>
		struct list_cat<W<A...>, W<B...>, Rest...> {
			using type = typename list_cat<W<A..., B...>, Rest...>::type;
		};
		
		template <template <class...> class W, class... A, class... B, 
				  class... C, class... D, class... E, class... Rest>
		struct list_cat<W<A...>, W<B...>, W<C...>, W<D...>, W<E...>, Rest...> {
			using type = typename list_cat<W<A..., B..., C..., D..., E...>, Rest...>::type;
		}; 
		
		template <template <class...> class W, class... A, class... B,
				  class... C, class... D, class... E, class... F, class... G, 
				  class... H, class... I, class... J, class... Tail>
		struct list_cat<W<A...>, W<B...>, W<C...>, W<D...>, W<E...>, W<F...>, W<G...>,
						W<H...>, W<I...>, W<J...>, Tail...> {
			using type = typename list_cat<W<A..., B..., C..., D..., E..., 
									F..., G..., H..., I..., J...>, Tail...>::type;			
		};

		template <class... Args>
		using list_cat_t = typename list_cat<Args...>::type;
		
		// ========================================================
		
		template <class T, class D = std::decay_t<T>>
		struct is_swl_tuple : std::false_type{};
		
		template <class T, class... Ts>
		struct is_swl_tuple<T, ::swl::tuple<Ts...>> : std::true_type{};
		
		template <class... Args>
		inline constexpr bool is_swl_tuple_v = (is_swl_tuple<Args>::value && ...);

	} // META

	template <std::size_t Clk, class Head, class... Args>
	constexpr auto&& tuple_getter(Head&& h, Args&&... args) noexcept {
		if constexpr (Clk == 0) return decltype(h)(h);
		else return tuple_getter<Clk - 1>(decltype(args)(args)...);
	}

	template <std::size_t Clk, class Wanted, class Head, class... Args>
	constexpr std::size_t get_index_of_type() {
		if constexpr ( std::is_same_v<Wanted, Head> ) return Clk;
		else return get_index_of_type<Clk + 1, Wanted, Args...>();
	}

	template <class T>
	struct element_wrapper {
		T elem;
	};

	template <class... Args>
	constexpr auto make_tuple_impl(Args&&... args) {
		return [...args = element_wrapper<Args>{static_cast<Args&&>(args)}] (auto&& fn) mutable -> decltype(auto) {
			return decltype(fn)(fn)( args.elem... );
		};
	}
	
} // IMPL

// ====================== tuple definition ===============================================

template <class... Ts>
class tuple{

	public : 
	
	static constexpr std::size_t size = sizeof...(Ts);
	
	// default ctor
	constexpr tuple() 
	noexcept ( (std::is_nothrow_default_constructible_v<Ts> && ...) ) 
	requires ( (std::is_default_constructible_v<Ts> && ...) )
	: memfn( impl::make_tuple_impl<Ts...>(Ts{}...)) 
	{
	}
	
	// generic ctor
	template <class... Args> requires ( (std::is_constructible_v<Ts, Args&&> && ...) )
	constexpr tuple(Args&&... args) 
	noexcept ( (std::is_nothrow_constructible_v<Ts, Args&&> && ...) )
	: memfn( impl::make_tuple_impl<Ts...>( Ts{static_cast<Args&&>(args)}... ) )
	{
	}
	
	// move ctor
	constexpr tuple(tuple&& o) 
	noexcept ( (std::is_nothrow_move_constructible_v<Ts> && ...) )
	requires ( (std::is_move_constructible_v<Ts> && ...) ) 
	: memfn( decltype(o.memfn)(o.memfn) )
	{
	}
	
	// move ctor
	template <class... Types>
	constexpr tuple(tuple<Types...>&& o)
	noexcept ( (std::is_nothrow_constructible_v<Ts, Types&&> && ...) )
	requires ( (std::is_constructible_v<Ts, Types&&> && ...) )
	: memfn( apply(decltype(o)(o), [] (auto&&... args) 
	{
		return impl::make_tuple_impl<Ts...>( Ts{static_cast<Types&&>(args)}... );
	}))
	{
	}
	
	// copy ctor
	constexpr tuple(const tuple<Ts...>& o)
	noexcept ( (std::is_nothrow_copy_constructible_v<Ts> && ...) )
	requires ( (std::is_copy_constructible_v<Ts> && ...) )
	: memfn(o.memfn)
	{
	}
	
	// copy ctor
	template <class... Types>
	constexpr tuple(const tuple<Types...>& other)
	noexcept ( (std::is_nothrow_constructible_v<Ts, const Types&> && ... ) )
	requires ( (std::is_constructible_v<Ts, const Types&> && ...) )
	: memfn( apply(other, [] (const auto&... elems) 
	{
		return impl::make_tuple_impl<Ts...>( Ts{elems}... );
	}))
	{
	}
	
	// copy-assign
	template <class... Types>
	constexpr tuple& operator=(const tuple<Types...>& other)
	noexcept ( (std::is_nothrow_assignable_v<Ts, const Types&> && ...) )
	requires ( (std::is_assignable_v<Ts, const Types&> && ...) )
	{
		apply(*this, [&other] (auto&... this_elems)
		{
			apply(other, [&this_elems...] (const auto&... other_elems)
			{
				((this_elems = other_elems), ...);
			});
		});
		return *this;
	}
	
	// move-assign
	template <class... Types>
	constexpr tuple& operator=(tuple<Types...>&& other)
	noexcept ( (std::is_nothrow_assignable_v<Ts, Types&&> && ...) )
	requires ( (std::is_assignable_v<Ts, Types&&> && ...) )
	{
		apply(*this, [&other] (Ts&... this_elems)
		{
			apply(decltype(other)(other), [&this_elems...] (auto&&... other_elems)
			{
				((this_elems = decltype(other_elems)(other_elems)), ...);
			});
		});
		return *this;
	}
	
	template <class Self, class Fn>
	requires impl::meta::is_swl_tuple_v<Self>
	friend constexpr decltype(auto) apply(Self&&, Fn&&);
	
	private :
	
	template <class T>
	static constexpr std::size_t index_of_type(){
		static_assert( (std::is_same_v<T, Ts> || ...), "Requested type is not contained in tuple." );
		return impl::get_index_of_type<0, T, Ts...>();
	}
	
	template <class Fn>
	constexpr decltype(auto) m_apply(Fn&& fn) & {
		return memfn( decltype(fn)(fn) );
	}
	
	template <class Fn>
	constexpr decltype(auto) m_apply(Fn&& fn) && {
		return memfn( [&fn] (auto&... elems) -> decltype(auto)
		{
			return static_cast<Fn&&>(fn)( static_cast<Ts&&>(elems)... );
		});
	}
	
	template <class Fn>
	constexpr decltype(auto) m_apply(Fn&& fn) const & {
		return const_cast<decltype(memfn)&>(memfn)( [&fn] (const auto&... elems) -> decltype(auto)
		{
			return static_cast<Fn&&>(fn)( elems... );
		});
	}
	
	template <class Fn>
	constexpr decltype(auto) m_apply(Fn&& fn) const && {
		return const_cast<decltype(memfn)&>(memfn)( [&fn] (auto&... elems) -> decltype(auto)
		{
			return static_cast<Fn&&>(fn)( static_cast<const Ts&&>(elems)... );
		});
	}
	
	decltype( impl::make_tuple_impl<Ts...>(std::declval<Ts>()...) ) memfn;
};

// ====================== tuple interface ===============================================

template <class Self, class Fn> 
requires impl::meta::is_swl_tuple_v<Self>
constexpr decltype(auto) apply(Self&& self, Fn&& fn){
	return static_cast<Self&&>(self).m_apply( static_cast<Fn&&>(fn) );
}

template <std::size_t N, class Self> 
requires impl::meta::is_swl_tuple_v<Self> && (N < std::decay_t<Self>::size) 
constexpr auto&& get(Self&& self) noexcept {
	return apply(decltype(self)(self), [] (auto&&... elems) -> auto&&
	{ 	
		return impl::tuple_getter<N>(decltype(elems)(elems)...); 
	});
}

template <class T, class Self> 
requires impl::meta::is_swl_tuple_v<Self>
constexpr auto&& get(Self&& self) noexcept {
	constexpr auto idx = std::decay_t<Self>::template index_of_type<T>();
	return get<idx>(decltype(self)(self));
}

template <class... Us, class... Vs>
constexpr bool operator == (const tuple<Us...>& u, const tuple<Vs...>& t){
	return apply(u, [&t] (auto&... u_mem) {
		return apply(t, [&u_mem...] (auto&... t_mem) {
			return ((u_mem == t_mem) && ...);
		});
	});
}

template <class... Us, class... Vs>
constexpr bool operator != (const tuple<Us...>& u, const tuple<Vs...>& v){
	return not (u == v);
}

template <class... Us, class... Vs>
constexpr bool operator < (const tuple<Us...>& a, const tuple<Vs...>& b){
	auto impl = [&a, &b] (auto& self, auto I) -> bool {
		if constexpr ( I == sizeof...(Us) ) return false;
		else {
			const auto& ae = get<I>(a);
			const auto& be = get<I>(b);
			if (ae < be) return true;
			else return not(be > ae) && self(self, std::integral_constant<std::size_t, I + 1>{});
		}
	};
	return impl(impl, std::integral_constant<std::size_t, 0>{});
}

template <class... Us, class... Vs>
constexpr bool operator > (const tuple<Us...>& a, const tuple<Vs...>& b){
	return b < a;
}

template <class... Us, class... Vs>
constexpr bool operator >= (const tuple<Us...>& a, const tuple<Vs...>& b){
	return not (a < b);
}

template <class... Us, class... Vs>
constexpr bool operator <= (const tuple<Us...>& a, const tuple<Vs...>& b){
	return not (b < a);
}

namespace impl {
	template <class Fn>
	constexpr auto tuple_cat_tail(Fn&& fn) noexcept {
		return fn();
	}
	
	template <class Fn, class Head, class... Tail>
	constexpr auto tuple_cat_tail(Fn&& fn, Head&& head, Tail&&... tail) noexcept {
		return tuple_cat_tail( [&head, &fn] (auto&&... elems) 
		{
			return apply( decltype(head)(head), [&elems..., &fn] (auto&&... head_elems) 
			{	
				return fn(decltype(head_elems)(head_elems)..., decltype(elems)(elems)...);
			});
		},	decltype(tail)(tail)... );
	}
}

template <class A, class B, class... Tail>
requires impl::meta::is_swl_tuple_v<A, B, Tail...>
constexpr auto tuple_cat(A&& a, B&& b, Tail&&... tail) {
	using Result = impl::meta::list_cat_t<std::decay_t<A>, std::decay_t<B>, std::decay_t<Tail>...>;
	
	return tuple_cat_tail( [&a] (auto&&... pack) 
	{
		return apply( decltype(a)(a), [&pack...] (auto&&... a_mem) 
		{	
			// lambda capture by rvalue references when possible, 
			// so we don't need to do anything more than ref-capture + forward
			return Result { decltype(a_mem)(a_mem)..., decltype(pack)(pack)... };
		});
	},	decltype(b)(b), decltype(tail)(tail)... );
	
}

template <class... Args>
constexpr auto make_tuple(Args&&... args){
	return tuple<std::decay_t<Args>...>(decltype(args)(args)...);
}

template <class... Args>
constexpr tuple<Args&...> tie (Args&... args) noexcept {
	return tuple<Args&...>(args...);
}

template <class... Args>
constexpr tuple<Args&&...> forward_as_tuple(Args&&... args) noexcept {
	return tuple<Args&&...>( decltype(args)(args)... );
}

} // SWL

#endif