#pragma once
#include "ct_str.hpp"
#include "method.hpp"
#include "tuple.hpp"

/*
 * \brief some rest prefix for some paths
 * Path - path to this nodes
 * Methods - rest_method and rest_prefix types
 */
template<typename Path, typename ...Methods>
struct rest_prefix {
    using path_t    = Path;
    using methods_t = std::tuple<Methods...>;

    using class_tuple_t = typename tuple_join<typename Methods::class_tuple_t...>::type;


    rest_prefix(Methods&&... meths)
        : methods_(std::make_tuple(std::forward<Methods>(meths)...)) {
    }

    methods_t methods_;
};

template<typename Path, typename ...Methods>
auto make_prefix(Methods&& ...mets){
    return rest_prefix<Path, std::decay_t<Methods>...>(
        std::forward<Methods>(mets)...);
}

#define prefix(path, ...) make_prefix<decltype(ct_str_split(ct_str_s(path)()))>(__VA_ARGS__)


