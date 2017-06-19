#pragma once

// algorithms for std::tuple
// 1. Check for type in tuple
// 2. Delete void type from tuple
// 3. Concatenate tuple
// 4. Shrink tuple (for example std::tuple<int,void,int,unsigned,char,unsigned> 
//                              -> std::tuple<int,void,unsigned,char>)
// 5. for_each element of tuple


#include <tuple>
#include <type_traits>


template <typename T, typename Tuple>
struct tuple_has_type;

template <typename T>
struct tuple_has_type<T, std::tuple<>> : std::false_type {};

template <typename ...Ts>
struct tuple_has_type<void, std::tuple<Ts...>> : std::false_type{};

template <typename T, typename U, typename... Ts>
struct tuple_has_type<T, std::tuple<U, Ts...>> : tuple_has_type<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct tuple_has_type<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename T, typename Tuple>
using tuple_has_type_t = typename tuple_has_type<T, Tuple>::type;


/*
 * Cleaning tuple from void's routine
 */
template<typename Tuple, typename Tuple2=void>
struct tuple_clean_void;

template<typename ...Ts>
struct tuple_clean_void<std::tuple<Ts...>,std::tuple<>> {
    using type = std::tuple<Ts...>;
};

template<typename ...Ts, typename ...Ts2>
struct tuple_clean_void<std::tuple<Ts...>, std::tuple<void,Ts2...>> {
    using type = typename tuple_clean_void<std::tuple<Ts...>, std::tuple<Ts2...>>::type;
};

template<typename ...Ts,typename T, typename ...Ts2>
struct tuple_clean_void<std::tuple<Ts...>, std::tuple<T,Ts2...>> {
    using type = typename tuple_clean_void<std::tuple<Ts..., T>, std::tuple<Ts2...>>::type;
};

template<typename ...Ts>
struct tuple_clean_void<std::tuple<Ts...>> {
    using type = typename tuple_clean_void<std::tuple<>, std::tuple<Ts...>>::type;
};


/*
 * Join operation operation
 */

template <typename... Typles>
struct tuple_join;

template <typename... Ts>
struct tuple_join<std::tuple<Ts...>> {
    using type = std::tuple<Ts...>;
};

template <typename... Ts, typename... Ts1>
struct tuple_join<std::tuple<Ts...>, std::tuple<Ts1...>> {
    using type = std::tuple<Ts..., Ts1...>;
};

template <typename... Ts, typename... Ts1, typename... Tuples>
struct tuple_join<std::tuple<Ts...>, std::tuple<Ts1...>, Tuples...> {
    using type = typename tuple_join<std::tuple<Ts..., Ts1...>, Tuples...>::type;
};

/*
 * Shrink tuples routine
 */
template<typename ...NewTypes>
auto shrink_tuple(std::tuple<NewTypes...>, std::tuple<>) -> std::tuple<NewTypes...>;

template <typename T, typename... NewTypes, typename... OldTypes>
auto shrink_tuple(std::tuple<NewTypes...>, std::tuple<T, OldTypes...>)
    -> decltype(shrink_tuple(std::tuple<NewTypes...>(),
                             tuple_has_type_t<T, std::tuple<NewTypes...>>(),
                             std::tuple<T, OldTypes...>()));

template <typename T, typename... NewTypes, typename... OldTypes>
auto shrink_tuple(std::tuple<NewTypes...>, std::false_type, std::tuple<T, OldTypes...>)
    -> decltype(shrink_tuple(std::tuple<NewTypes..., T>(), std::tuple<OldTypes...>()));

template <typename T, typename... NewTypes, typename... OldTypes>
auto shrink_tuple(std::tuple<NewTypes...>, std::true_type, std::tuple<T, OldTypes...>)
    -> decltype(shrink_tuple(std::tuple<NewTypes...>(), std::tuple<OldTypes...>()));

template <typename... Ts>
auto shrink_tuple(std::tuple<Ts...>) -> decltype(shrink_tuple(std::tuple<>(), std::tuple<Ts...>()));

template <typename Tuple>
constexpr std::make_index_sequence<std::tuple_size<Tuple>::value> getIxSeq(Tuple const& ) {
    return {};
}

template<size_t ...Is, typename Tuple, typename F>
void for_each(std::index_sequence<Is...>, Tuple&& tup, F&& f){
    using std::get;
    using ordered = int[];

    (void)ordered{(f(get<Is>(std::forward<Tuple>(tup))), 0)...};
}

template< typename Tuple, typename F>
void for_each( Tuple&& tup, F&& f){
    using std::get;
    using ordered = int[];
    for_each(getIxSeq(tup), std::forward<Tuple>(tup), std::forward<F>(f));
}

