#include "rest_types.hpp"


// this is random names
struct time {
    static log extract_log();
};

struct hash {
    sha1 compute_sha1(some_string_data const& v);
};

struct performance {
    perf_count  extract(perf_count_query const& name);
    void setup_datetime(dt t);
};

struct persistent {

    void setup(int_value val);
    int_value extract();

    int val_ = 0;
    
};

