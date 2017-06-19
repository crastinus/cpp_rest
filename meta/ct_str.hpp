#pragma  once

// Compile time string 
// In this file implements compile time split operation
#include <tuple>

#define CHARSEQ1(n,x) grabch<0x##n##0>(x)

 /* 2^1 = 2 */
#define CHARSEQ2(n,x) grabch<0x##n##0>(x), grabch<0x##n##1>(x)

 /* 2^2 = 2 */
#define CHARSEQ4(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x)

 /* 2^3 = 8 */
#define CHARSEQ8(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x) \
       , grabch<0x##n##4>(x), grabch<0x##n##5>(x) \
       , grabch<0x##n##6>(x), grabch<0x##n##7>(x)

 /* 2^4 = 16 */
#define CHARSEQ16(n,x) \
             grabch<0x##n##0>(x), grabch<0x##n##1>(x) \
       , grabch<0x##n##2>(x), grabch<0x##n##3>(x) \
       , grabch<0x##n##4>(x), grabch<0x##n##5>(x) \
       , grabch<0x##n##6>(x), grabch<0x##n##7>(x) \
       , grabch<0x##n##8>(x), grabch<0x##n##9>(x) \
       , grabch<0x##n##A>(x), grabch<0x##n##B>(x) \
       , grabch<0x##n##C>(x), grabch<0x##n##D>(x) \
       , grabch<0x##n##E>(x), grabch<0x##n##F>(x)

 /* 2^5 = 32 */
#define CHARSEQ32(n,x) \
             CHARSEQ16(n##0,x),CHARSEQ16(n##1,x)

 /* 2^6 = 64 */
#define CHARSEQ64(n,x) \
             CHARSEQ16(n##0,x), CHARSEQ16(n##1,x), CHARSEQ16(n##2,x) \
       , CHARSEQ16(n##3,x)

 /* 2^7 = 128 */
#define CHARSEQ128(n,x) \
             CHARSEQ16(n##0,x), CHARSEQ16(n##1,x), CHARSEQ16(n##2,x) \
       , CHARSEQ16(n##3,x), CHARSEQ16(n##4,x), CHARSEQ16(n##5,x) \
       , CHARSEQ16(n##6,x), CHARSEQ16(n##7,x)

 /* 2^8 = 256 */
#define CHARSEQ256(n,x) \
             CHARSEQ16(n##0,x), CHARSEQ16(n##1,x), CHARSEQ16(n##2,x) \
       , CHARSEQ16(n##3,x), CHARSEQ16(n##4,x), CHARSEQ16(n##5,x) \
       , CHARSEQ16(n##6,x), CHARSEQ16(n##7,x), CHARSEQ16(n##8,x) \
       , CHARSEQ16(n##9,x), CHARSEQ16(n##A,x), CHARSEQ16(n##B,x) \
       , CHARSEQ16(n##C,x), CHARSEQ16(n##D,x), CHARSEQ16(n##E,x) \
       , CHARSEQ16(n##F,x)

 /* 2^9 = 512 */
#define CHARSEQ512(n,x) \
             CHARSEQ256(n##0,x), CHARSEQ256(n##1,x)

 /* 2^10 = 1024 */
#define CHARSEQ1024(n,x) \
             CHARSEQ256(n##0,x), CHARSEQ256(n##1,x), CHARSEQ256(n##2,x) \
       , CHARSEQ128(n##3,x), CHARSEQ16(n##38,x), CHARSEQ16(n##39,x) \
       , CHARSEQ16(n##3A,x), CHARSEQ16(n##3B,x), CHARSEQ16(n##3C,x) \
       , CHARSEQ16(n##3D,x), CHARSEQ16(n##3E,x), CHARSEQ16(n##3F,x)

template <int N, int M>
constexpr char grabch(char const (&c)[M]) noexcept {
    return c[N < M ? N : M - 1];
}

template <char... Chars>
struct ct_str {
    static constexpr char const   literal[sizeof...(Chars) + 1] = {Chars..., '\0'};
    static constexpr unsigned int size                          = sizeof...(Chars);

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
constexpr char ct_str<Chars...>::literal[];

/*
 * cut zeroes operation
 *  /some/strin/\0\0\0.... -> /some/strin/
 */
template <char... X>
auto ct_str_cut0(ct_str<X...>) -> ct_str<X...>;

template <char... X, char... Y>
auto ct_str_cut0(ct_str<X...>, ct_str<'\0'>, ct_str<Y>...) -> ct_str<X...>;

template <char A, char... X, char... Y>
auto           ct_str_cut0(ct_str<X...>, ct_str<A>, ct_str<Y>...)
    -> decltype(ct_str_cut0(ct_str<X..., A>(), ct_str<Y>()...));

template <char... C>
auto ct_str_norm(ct_str<C...>) -> decltype(ct_str_cut0(ct_str<C>()...));

/* Trim left operation
 *  ///abc/ -> abc/
 */
template <char... X>
auto ct_str_trim_left_impl(ct_str<X>...) -> ct_str<X...>;

template <char... X>
auto ct_str_trim_left_impl(ct_str<'/'>, ct_str<X>...)
    -> decltype(ct_str_trim_left_impl(ct_str<X>()...));

template <char... C>
auto ct_str_trim_left(ct_str<C...>)
    -> decltype(ct_str_trim_left_impl(ct_str<C>()...));

/*
 * split logic
 * abc/cde/q -> [abc,cde,q]
 */
template <typename... Strings>
auto ct_str_split_impl(std::tuple<Strings...>) -> std::tuple<Strings...>;

template <typename... Strings, char... X>
auto ct_str_split_impl(std::tuple<Strings...>, ct_str<X...>)
    -> std::tuple<Strings..., ct_str<X...>>;

// handle start and double slash if it might be
template <typename... Strings, char... Y, char... X>
auto ct_str_split_impl(std::tuple<Strings...>,
                       ct_str<Y...>,
                       ct_str<'/'>,
                       ct_str<X>...)
    -> decltype(ct_str_split_impl(std::tuple<Strings..., ct_str<Y...>>(),
                                  ct_str<X>()...));

template <char A, typename... Strings, char... Y, char... X>
auto ct_str_split_impl(std::tuple<Strings...>, ct_str<Y...>, ct_str<A>, ct_str<X>...)
    -> decltype(ct_str_split_impl(std::tuple<Strings...>(),
                                  ct_str<Y..., A>(),
                                  ct_str<X>()...));

template <char... C>
auto ct_str_split(ct_str<C...>)
     -> decltype(ct_str_split_impl(std::tuple<>(), ct_str<C>()...));

#define ct_str_s(x) decltype(ct_str_norm(ct_str<CHARSEQ256(, x)>()))

