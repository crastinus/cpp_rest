#pragma once

#include "method.hpp"
#include "method_apply.hpp"
#include "comparation.hpp"
#include "prefix.hpp"
//#include "rest.hpp"

//?
//#include <vector>
#include <string>

//// \brief
//// type for an executing request
//// siplified walk throught function
//// i don't know for now how to unified this inteface
//struct request_instance {
//    rest::request  request_;
//    rest::response response_;
//};

/*\brief
 * Root of all rest methods
 * BufferType - type of the input and output data
 * Methods - types of rest_prefix and  rest_method
 */
template <typename BufferType, typename Path, typename... Methods>
struct rest_root {
    using path_t    = Path;
    using methods_t = std::tuple<Methods...>;

    using class_tuple_t =
        typename tuple_cat<typename Methods::class_tuple_t...>::type;
    using class_tuple_rm_void_t = typename tuple_clean_void<class_tuple_t>::type;
    using classes_t             = decltype(shrink_tuple(class_tuple_rm_void_t()));

    rest_root(Methods&&... meths)
        : methods_(std::make_tuple(std::forward<Methods>(meths)...)) {
    }

    // Request instance must have operations 
    // BufferType const& request_source();
    // void set_response(int code, BufferType&& src);
    // void set_text_response(int code, BufferType&&src);
    // bool handled();
    //
    // in this code handles just a POST methods.
    // non post methods must be filtered earlier
    template <typename RequestInstance,
              typename Range,
              typename MethodPath,
              typename ClassType,
              typename ReturnType,
              typename InputType>
    void walk_concrete(
        RequestInstance& inst,
        Range            rng,
        rest_method<MethodPath, ClassType, ReturnType, InputType>& method) {
        if (compare(rng, MethodPath())) {
            try {
                BufferType result = method.apply(instances_, inst.request_source());
                auto       code   = result.empty() ? 201 : 200;
                inst.set_response(code, std::move(result)); 
            } catch(std::exception& ex){
                inst.set_text_response(500, ex.what());
            } catch (...){
                inst.set_text_response(500, BufferType{});
            }
                  
        }
   }

   template <typename RequestInstance,
             typename Range,
             typename PrefixPath,
             typename... PrefixMethods>
   void walk_concrete(RequestInstance& inst,
                      Range            rng,
                      rest_prefix<PrefixPath, PrefixMethods...>& pref) {
       if (!compare_partial(rng, PrefixPath()))
           return;
       for_each(pref.methods_, [&, this](auto& method) {
           if (!inst.handled())
               walk_concrete(inst, rng, method);
       });
    }

    template <typename RequestInstance, typename Range>
    void walk(RequestInstance& inst, Range rng) {
        if (!compare_partial(rng, path_t()))
            return;
        for_each(methods_, [&, this](auto& method) {
            if (!inst.handled())
                walk_concrete(inst, rng, method);
        });
    }

    methods_t methods_;
    classes_t instances_;
};

template <typename BufType, typename Path, typename... Methods>
auto make_root(Methods&&... mets) {
    return rest_root<BufType, Path, std::decay_t<Methods>...>(std::forward<Methods>(mets)...);
}

#define root(buf_type, path, ...) make_root<buf_type, decltype(ct_str_split(ct_str_s(path)()))>(__VA_ARGS__)


