#pragma once
#include <string>

// some test methods of the rest querie

struct log {
    std::string string_;
    int64_t     timestamp_;
};

struct http_error{
    std::string error_;
};

struct dt {
    int64_t datetime_;
};

struct perf_count {
    int fan_speed_;
    int cpu_remp_;
};

struct perf_count_query{
    std::string name_;
};

struct some_string_data{
    std::string data_;
};

struct sha1{
    std::string sha1sum_;
};

struct int_value {
    int value_;
};
