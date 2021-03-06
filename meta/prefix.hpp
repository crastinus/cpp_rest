#pragma once
#include "typestring.hpp"
#include "method.hpp"
#include "tuple.hpp"

/*
 * \brief part of path to the method and the host
 *
 * \tparam Path     path to this nodes
 * \tparam Methods  rest_method and rest_prefix types
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

#define prefix(path, ...) make_prefix<decltype(typestring_split(typestring_s(path)()))>(__VA_ARGS__)


