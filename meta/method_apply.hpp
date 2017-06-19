#pragma once

// dependency of templater
#include <utils/reflection/meta/templater.hpp>
#include <utils/reflection/json/rj_reading_visitor.hpp>
#include <utils/reflection/json/rj_writing_visitor.hpp>
#include "method.hpp"


// workaround about void type 
// void type cannot be instantiated
template<typename Type>
struct holder {
    Type value_;
};

template<>
struct holder<void>{};

// helper of helper
// C++ not allowed partial specialization for a functions
// using this workaround for handle any possible combination
// of InputType,MemberFunction and ReturnType
template <typename ClassType, typename ReturnType, typename InputType>
struct invoke {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&        tuple,
                    FunctionRef         func,
                    holder<ReturnType>& result,
                    holder<InputType>&  input) {
        auto& object  = std::get<ClassType>(tuple);
        result.value_ = (object.*func)(input.value_);
    }
};

template <typename ReturnType, typename InputType>
struct invoke<void, ReturnType, InputType> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&,
                    FunctionRef         func,
                    holder<ReturnType>& result,
                    holder<InputType>&  input) {
        result.value_ = (*func)(input.value_);
    }
};

template <typename ClassType, typename ReturnType>
struct invoke<ClassType, ReturnType, void> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&        tuple,
                    FunctionRef         func,
                    holder<ReturnType>& result,
                    holder<void>&) {
        auto& object  = std::get<ClassType>(tuple);
        result.value_ = (object.*func)();
    }
};

template <typename ClassType, typename InputType>
struct invoke<ClassType, void, InputType> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource& tuple,
                    FunctionRef  func,
                    holder<void>&,
                    holder<InputType>& input) {
        auto& object = std::get<ClassType>(tuple);
        (object.*func)(input.value_);
    }
};

template <typename ClassType>
struct invoke<ClassType, void, void> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource& tuple,
                    FunctionRef  func,
                    holder<void>&,
                    holder<void>&) {
        auto& object = std::get<ClassType>(tuple);
        (object.*func)();
    }
};

template <typename ReturnType>
struct invoke<void, ReturnType, void> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&,
                    FunctionRef         func,
                    holder<ReturnType>& result,
                    holder<void>&) {
        result.value_ = (*func)();
    }
};

template <typename InputType>
struct invoke<void, void, InputType> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&,
                    FunctionRef func,
                    holder<void>&,
                    holder<InputType>& input) {
        (*func)(input.value_);
    }
};

template <>
struct invoke<void, void, void> {
    template <typename FunctionRef, typename ClassSource>
    static void run(ClassSource&, FunctionRef func, holder<void>&, holder<void>&) {
        (*func)();
    }
};

// serialize any possible type
template <typename BufferType, typename Type>
struct serialization {

    static void deserialize(BufferType const& buffer, holder<Type>& holder) {
        rj::reading_visitor visitor;
        if (!visitor.parse(buffer)) {
            throw std::runtime_error(visitor.parse_error());
        }
        templater::deserialize(&visitor, holder.value_);
    }

    static void serialize(holder<Type> const& holder, BufferType& buffer) {

        rj::writing_visitor writer;
        templater::serialize(&writer, holder.value_);

        buffer = writer.serialize<BufferType>();
    }
};

//serialize void
template <typename BufferType>
struct serialization<BufferType, void> {
    static void deserialize(BufferType const&, holder<void>&) {
    }
    static void serialize(holder<void> const&, BufferType& buffer) {
        // if in buffer have something
        buffer = BufferType{};
    }
};

// \brief helper for an apply operation
template <typename BufferType,
          typename ClassSource,
          typename ClassType,
          typename ReturnType,
          typename InputType>
struct operations {

    using ref_ft = typename ref_ft_impl<ClassType, ReturnType, InputType>::type;
    using decaied_input_t  = std::decay_t<InputType>;
    using decaied_return_t = std::decay_t<ReturnType>;
 
    static void deserialize(BufferType const& src, holder<decaied_input_t>& input) {
        serialization<BufferType, decaied_input_t>::deserialize(src, input);
    }

    static void serialize(holder<decaied_return_t> const& result, BufferType& buffer) {
        serialization<BufferType, decaied_return_t>::serialize(result, buffer);
    }

    // here is 8 possible combinations
    static void execute(ClassSource&              tuple,
                        ref_ft&                   func,
                        holder<decaied_return_t>& result,
                        holder<decaied_input_t>&  input) {
        // this is no way to handle rvalue
        invoke<ClassType, decaied_return_t, decaied_input_t>::run(tuple, func, result, input);
    }
};

template <typename Path, typename ClassType, typename ReturnType, typename InputType>
template <typename Tuple, typename BufferType>
BufferType rest_method<Path, ClassType, ReturnType, InputType>::apply(
    Tuple&            tup,
    BufferType const& src) {

    using decaied_input_t  = std::decay_t<InputType>;
    using decaied_return_t = std::decay_t<ReturnType>;
    using operations_t =
        operations<BufferType, Tuple, ClassType, ReturnType, InputType>;

    BufferType result;

    holder<decaied_input_t>  input_value;
    holder<decaied_return_t> return_value;

    operations_t::deserialize(src, input_value);
    operations_t::execute(tup, function_, return_value, input_value);
    operations_t::serialize(return_value, result);

    return result;
}

