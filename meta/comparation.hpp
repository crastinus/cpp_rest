#pragma once
#include "ct_str.hpp"
#include <string.h>
#include <type_traits>


template<typename Iterator>
unsigned length(Iterator it){
    return it->length();
}

template<typename Iterator>
char const* data(Iterator it){
    return it->data();
}

template <typename Iterator, typename CtStr>
bool compare(Iterator it, CtStr&&) {
    using CtStrCleaned = typename std::remove_reference<CtStr>::type;
    if (length(it) != CtStrCleaned::length())
        return false;
    return (strncasecmp(data(it), CtStrCleaned::data(), CtStrCleaned::length()) ==
            0);
}

template<typename Iterator>
struct range{
    Iterator begin_;
    Iterator end_;

    // for walking throught function
    range update(Iterator it) {
        return range{it, end_};
    }

   // template <unsigned N>
   // bool compare(char const (&literal)[N]);
};

template <typename Iterator>
inline range<Iterator> make_range(Iterator begin, Iterator end) {
    return range<Iterator>{begin, end};
}

template <typename Iterator, char... C>
inline bool compare(range<Iterator> rng, ct_str<C...>&&) {
    return compare(rng.begin_, ct_str<C...>());
}

template <typename Range, typename... Values>
inline bool compare_impl(Range& rng, std::tuple<Values...>&& vals) {
    bool result = true;
    for_each(vals, [&](auto&& str) {
        if (rng.begin_ == rng.end_ || !result) {
            result = false;
            return;
        }
        result = compare(rng.begin_, str);
        ++rng.begin_;
    });
    return result;
}

template <typename Range, typename... Values>
inline bool compare(Range& rng, std::tuple<Values...>&& vals) {
    auto result = compare_impl(rng, std::forward<Values>(vals)...);
    if (result)
        // reached end of the sequence
        return rng.begin_ == rng.end_;
    return false;
}

template <typename Range, typename... Values>
inline bool compare_partial(Range& rng, std::tuple<Values...>&& vals) {
    auto result = compare_impl(rng, std::move(vals));
    // end doesn't reached
    if (result)
        return rng.begin_ != rng.end_;
    return false;
}

