#pragma once
// compare run time path and compile time path
#include <string.h>
#include <type_traits>
#include "typestring.hpp"
#include "range.hpp"


// default function helper for length
template<typename Iterator>
unsigned length(Iterator it){
    return it->length();
}

// default function helper for raw data
template<typename Iterator>
char const* data(Iterator it){
    return it->data();
}

// compare compile time string and iterator
template <typename Iterator, typename CtStr>
bool compare(Iterator it, CtStr&&) {
    using CtStrCleaned = typename std::remove_reference<CtStr>::type;
    if (length(it) != CtStrCleaned::length())
        return false;
    // rest url must be case insensitive
    return (strncasecmp(data(it), CtStrCleaned::data(), CtStrCleaned::length()) ==
            0);
}

// compare single one value 
template <typename Iterator, char... C>
inline bool compare(range<Iterator> rng, typestring<C...>&&) {
    return compare(rng.begin_, typestring<C...>());
}

// compare range sequence and compile time tuple sequence
template <typename Range, typename... Values>
inline bool compare_impl(Range& rng, std::tuple<Values...>&& vals) {
    //std::tuple_size<decltype(vals)>::
    bool result = true;
    // unfold tuple
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

// starts with operation
template <typename Range, typename... Values>
inline bool compare_partial(Range& rng, std::tuple<Values...>&& vals) {
    auto result = compare_impl(rng, std::move(vals));
    // end doesn't reached
    if (result)
        return rng.begin_ != rng.end_;
    return false;
}

