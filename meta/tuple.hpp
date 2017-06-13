#pragma once

#include <tuple>
#include <type_traits>

//template <typename T, typename... Types>
//auto tuple_has_type(T, std::tuple<T, Types...>) -> std::true_type;
//
////template <typename T, typename... Types>
////auto tuple_has_type(void, std::tuple<T, Types...>) -> std::true_type;
//
//template <typename T>
//auto tuple_has_type(T, std::tuple<>) -> std::false_type;
//
//template <typename T, typename Other, typename... Types>
//auto tuple_has_type(T, std::tuple<Other, Types...>)
//    -> decltype(tuple_has_type(T(), std::tuple<Types...>()));



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
 * Cat tuples operation
 */

template <typename... Typles>
struct tuple_cat;

//template <typename T>
//struct tuple_cat<T> {
//    using type = T;
//};
template <typename... Ts>
struct tuple_cat<std::tuple<Ts...>> {
    using type = std::tuple<Ts...>;
};

template <typename... Ts, typename... Ts1>
struct tuple_cat<std::tuple<Ts...>, std::tuple<Ts1...>> {
    using type = std::tuple<Ts..., Ts1...>;
};

template <typename... Ts, typename... Ts1, typename... Tuples>
struct tuple_cat<std::tuple<Ts...>, std::tuple<Ts1...>, Tuples...> {
    using type = typename tuple_cat<std::tuple<Ts..., Ts1...>, Tuples...>::type;
};

//template <typename T>
//struct tuple_cat {
//    using type = T;
//};

//template <typename A, typename B, typename... Tuples>
//struct tuple_cat {
//    using type = tuple_cat_impl<A,B 
//};

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

