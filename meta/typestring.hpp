#pragma  once

// Compile time string 
// In this file implements compile time split operation
// 
// get basics from this
// https://github.com/irrequietus/typestring/blob/master/typestring.hh
#include <tuple>



#define TYPESTRING1(n,x) grabch<0x##n##0>(x)

 /* 2^1 = 2 */
#define TYPESTRING2(n,x) grabch<0x##n##0>(x), grabch<0x##n##1>(x)

 /* 2^2 = 2 */
#define TYPESTRING4(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x)

 /* 2^3 = 8 */
#define TYPESTRING8(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x) \
       , grabch<0x##n##4>(x), grabch<0x##n##5>(x) \
       , grabch<0x##n##6>(x), grabch<0x##n##7>(x)

 /* 2^4 = 16 */
#define TYPESTRING16(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x) \
       , grabch<0x##n##4>(x), grabch<0x##n##5>(x) \
       , grabch<0x##n##6>(x), grabch<0x##n##7>(x) \
       , grabch<0x##n##8>(x), grabch<0x##n##9>(x) \
       , grabch<0x##n##A>(x), grabch<0x##n##B>(x) \
       , grabch<0x##n##C>(x), grabch<0x##n##D>(x) \
       , grabch<0x##n##E>(x), grabch<0x##n##F>(x)

 /* 2^5 = 32 */
#define TYPESTRING32(n,x) \
             TYPESTRING16(n##0,x),TYPESTRING16(n##1,x)

 /* 2^6 = 64 */
#define TYPESTRING64(n,x) \
             TYPESTRING16(n##0,x), TYPESTRING16(n##1,x), TYPESTRING16(n##2,x) \
       , TYPESTRING16(n##3,x)

 /* 2^7 = 128 */
#define TYPESTRING128(n,x) \
             TYPESTRING16(n##0,x), TYPESTRING16(n##1,x), TYPESTRING16(n##2,x) \
       , TYPESTRING16(n##3,x), TYPESTRING16(n##4,x), TYPESTRING16(n##5,x) \
       , TYPESTRING16(n##6,x), TYPESTRING16(n##7,x)

 /* 2^8 = 256 */
#define TYPESTRING256(n,x) \
             TYPESTRING16(n##0,x), TYPESTRING16(n##1,x), TYPESTRING16(n##2,x) \
       , TYPESTRING16(n##3,x), TYPESTRING16(n##4,x), TYPESTRING16(n##5,x) \
       , TYPESTRING16(n##6,x), TYPESTRING16(n##7,x), TYPESTRING16(n##8,x) \
       , TYPESTRING16(n##9,x), TYPESTRING16(n##A,x), TYPESTRING16(n##B,x) \
       , TYPESTRING16(n##C,x), TYPESTRING16(n##D,x), TYPESTRING16(n##E,x) \
       , TYPESTRING16(n##F,x)

 /* 2^9 = 512 */
#define TYPESTRING512(n,x) \
             TYPESTRING256(n##0,x), TYPESTRING256(n##1,x)

 /* 2^10 = 1024 */
#define TYPESTRING1024(n,x) \
             TYPESTRING256(n##0,x), TYPESTRING256(n##1,x), TYPESTRING256(n##2,x) \
       , TYPESTRING128(n##3,x), TYPESTRING16(n##38,x), TYPESTRING16(n##39,x) \
       , TYPESTRING16(n##3A,x), TYPESTRING16(n##3B,x), TYPESTRING16(n##3C,x) \
       , TYPESTRING16(n##3D,x), TYPESTRING16(n##3E,x), TYPESTRING16(n##3F,x)

template <int N, int M>
constexpr char grabch(char const (&c)[M]) noexcept {
    return c[N < M ? N : M - 1];
}

template <char... Chars>
struct typestring {
    static constexpr char const   literal[sizeof...(Chars) + 1] = {Chars..., '\0'};
    static constexpr unsigned int size                          = sizeof...(Chars);

  public:
    static constexpr char const* data() noexcept {
        return &literal[0];
    }

    static constexpr unsigned int length() noexcept {
        return size;
    };

    static constexpr char const* cbegin() noexcept {
        return &literal[0];
    }

    static constexpr char const* cend() noexcept {
        return &literal[size];
    }
};

template <char... Chars>
constexpr char typestring<Chars...>::literal[];

/*
 * cut zeroes operation
 *  /some/strin/\0\0\0.... -> /some/strin/
 */
template <char... X>
auto typestring_cut0(typestring<X...>) -> typestring<X...>;

template <char... X, char... Y>
auto typestring_cut0(typestring<X...>, typestring<'\0'>, typestring<Y>...) -> typestring<X...>;

template <char A, char... X, char... Y>
auto           typestring_cut0(typestring<X...>, typestring<A>, typestring<Y>...)
    -> decltype(typestring_cut0(typestring<X..., A>(), typestring<Y>()...));

template <char... C>
auto typestring_norm(typestring<C...>) -> decltype(typestring_cut0(typestring<C>()...));

/* Trim left operation
 *  ///abc/ -> abc/
 */
template <char... X>
auto typestring_trim_left_impl(typestring<X>...) -> typestring<X...>;

template <char... X>
auto typestring_trim_left_impl(typestring<'/'>, typestring<X>...)
    -> decltype(typestring_trim_left_impl(typestring<X>()...));

template <char... C>
auto typestring_trim_left(typestring<C...>)
    -> decltype(typestring_trim_left_impl(typestring<C>()...));

/*
 * split logic
 * abc/cde/q -> [abc,cde,q]
 */
template <typename... Strings>
auto typestring_split_impl(std::tuple<Strings...>) -> std::tuple<Strings...>;

template <typename... Strings, char... X>
auto typestring_split_impl(std::tuple<Strings...>, typestring<X...>)
    -> std::tuple<Strings..., typestring<X...>>;

// handle start and double slash if it might be
template <typename... Strings, char... Y, char... X>
auto typestring_split_impl(std::tuple<Strings...>,
                       typestring<Y...>,
                       typestring<'/'>,
                       typestring<X>...)
    -> decltype(typestring_split_impl(std::tuple<Strings..., typestring<Y...>>(),
                                  typestring<X>()...));

template <char A, typename... Strings, char... Y, char... X>
auto typestring_split_impl(std::tuple<Strings...>, typestring<Y...>, typestring<A>, typestring<X>...)
    -> decltype(typestring_split_impl(std::tuple<Strings...>(),
                                  typestring<Y..., A>(),
                                  typestring<X>()...));

template <char... C>
auto typestring_split(typestring<C...>)
     -> decltype(typestring_split_impl(std::tuple<>(), typestring<C>()...));

#define typestring_s(x) decltype(typestring_norm(typestring<TYPESTRING256(, x)>()))

