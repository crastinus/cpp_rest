#pragma once

#include <tuple>
#include "typestring.hpp"

// This function invokes as arguments
template <typename Class, typename ReturnType, typename InputType>
using ref_member_ft = ReturnType (Class::*)(InputType);

template <typename ReturnType, typename InputType>
using ref_static_ft = ReturnType (*)(InputType);

template <typename Class, typename ReturnType>
using ref_member_void_ft = ReturnType (Class::*)();

template < typename ReturnType>
using ref_static_void_ft = ReturnType (*)();

// Metaprogramming for a class
template <typename ClassType, typename ReturnType, typename InputType>
struct ref_ft_impl {
    using type = ref_member_ft<ClassType, ReturnType, InputType>;
};

template <typename ReturnType, typename InputType>
struct ref_ft_impl<void, ReturnType, InputType> {
    using type = ref_static_ft<ReturnType, InputType>;
};

template <typename ClassType, typename ReturnType>
struct ref_ft_impl<ClassType, ReturnType, void> {
    using type = ref_member_void_ft<ClassType, ReturnType>;
};

template <typename ReturnType>
struct ref_ft_impl<void, ReturnType, void> {
    using type = ref_static_void_ft<ReturnType>;
};

// \brief method itself
// stores, apply method
template <typename Path, typename ClassType, typename ReturnType, typename InputType>
struct rest_method {
    using path_t   = Path;
    using returh_t = ReturnType;
    using input_t  = InputType;
    using typename_t  = ClassType;
    using ref_ft   =  typename ref_ft_impl<ClassType,ReturnType,InputType>::type;

    // for metaprgramming purposses
    using class_tuple_t = std::tuple<typename_t>;

    rest_method(ref_ft func) : function_(func) {
    }

    // noreturn is an attribute
    constexpr bool is_no_return() {
        return std::is_same<ReturnType, void>::value;
    }

    constexpr bool is_no_invoke(){
        return std::is_same<InputType, void>::value;
    }

    // apply current method 
    // 1. Deserialize src if input type is not void
    // 2. Invoke as argument in function if this needed
    // 3. If function_ is a member of class then use tup for invoke
    // 4. Serialize result if ResultType is not void
    // \param tup - tuple of possible objects for member functions
    // \param src - source code of query
    template <typename Tuple, typename ResultType>
    ResultType apply(Tuple& tup, ResultType const& src);

    ref_ft function_;
};

template <typename Path, typename ClassType, typename ReturnType, typename InputType>
auto make_method(ref_member_ft<ClassType, ReturnType, InputType> function) {
    return rest_method<Path, ClassType, ReturnType, InputType>(function);
}

template <typename Path, typename ReturnType, typename InputType>
auto make_method(ref_static_ft<ReturnType, InputType> function) {
    return rest_method<Path, void, ReturnType, InputType>(function);
}
//template <typename Class, typename ReturnType>
//using ref_member_void_ft = ReturnType (Class::*)();
//
//template <typename Class, typename ReturnType>
//using ref_static_void_ft = ReturnType (*)();

template <typename Path, typename ClassType, typename ReturnType>
auto make_method(ref_member_void_ft<ClassType, ReturnType> function) {
    return rest_method<Path, ClassType, ReturnType, void>(function);
}

template <typename Path, typename ReturnType>
auto make_method(ref_static_void_ft<ReturnType> function) {
    return rest_method<Path, void, ReturnType, void>(function);
}


#define method(path, func) make_method<typestring_s(path)>(func)


