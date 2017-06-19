#pragma once

// \brief helper for a path algorithms
// when path walk throught prefix part equivalent to prefix cut down
// for example 
// 
// path =  { "some", "prefix", "route", "api", "member" }
// prefix = "some/prefix/root"
//
// before prefix
// *begin_  = "some"
// 
// after prefix creates new range for comparision operations and invokes next
// *begin_ = "api"

template<typename Iterator>
struct range{
    Iterator begin_;
    Iterator end_;

    // for walking throught function
    range update(Iterator it) {
        return range{it, end_};
    }

    // this might be unsafe
    unsigned size() const {
        return end_ - begin_;
    }
};

template <typename Iterator>
inline range<Iterator> make_range(Iterator begin, Iterator end) {
    return range<Iterator>{begin, end};
}

