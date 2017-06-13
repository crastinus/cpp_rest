#include <utils/reflection/meta/templater_read_values.hpp>
#include <utils/reflection/meta/templater_write_values.hpp>
#include <utils/reflection/meta/templater_spec_vector.hpp>
//#include <utils/reflection/meta/templater_read_values.hpp>
//

#include "rest_types.hpp"
//#include <Meta.h>

#define MAKE_INSTANCE(TYPE)                                                         \
    namespace templater {                                                           \
    template void serialize<TYPE>(writing_visitor * visitor,                        \
                                  TYPE const& obj) noexcept;                        \
    template void deserialize<TYPE>(reading_visitor * visitor, TYPE& obj) noexcept; \
    }

namespace meta {

template <>
auto registerMembers<log>() {
    return members(member("body", &log::string_),
                   member("timestamp", &log::timestamp_));
}

template <>
auto registerMembers<http_error>() {
    return members(member("error", &http_error::error_));
}

template <>
auto registerMembers<dt>() {
    return members(member("datetime", &dt::datetime_));
}

template <>
auto registerMembers<perf_count_query>() {
    return members(member("counter_name", &perf_count_query::name_));
}


template <>
auto registerMembers<perf_count>() {
    return members(member("fan_speed", &perf_count::fan_speed_),
                   member("cup_temp", &perf_count::cpu_remp_));
};

template <>
auto registerMembers<some_string_data>() {
    return members(member("data", &some_string_data::data_));
}

template <>
auto registerMembers<sha1>() {
    return members(member("sha1", &sha1::sha1sum_));
}

template<>
auto registerMembers<int_value>(){
    return members(member("intValue", &int_value::value_));
}
}

MAKE_INSTANCE(log)
MAKE_INSTANCE(http_error)
MAKE_INSTANCE(dt)
MAKE_INSTANCE(perf_count)
MAKE_INSTANCE(perf_count_query)
MAKE_INSTANCE(some_string_data)
MAKE_INSTANCE(sha1)
MAKE_INSTANCE(int_value)

