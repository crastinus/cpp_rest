#include "rest_handlers.hpp"
#include <utils/string/sha1sum.hpp>
#include <utils/string/build.hpp>
#include <utils/iostream/datetime.hpp>
#include <chrono>
#include <iostream>

log time::extract_log() {
    log result;

    result.string_ = utils::build_string("Some random log ", __FILE__, ":", __LINE__,
                                         " at time ", utils::datetime_utc);
    result.timestamp_ = std::chrono::system_clock::now().time_since_epoch().count();
    return result;
}

sha1 hash::compute_sha1(some_string_data const& v) {
    sha1 result;
    result.sha1sum_ = utils::sha1(v.data_);
    return result;
}

perf_count performance::extract(perf_count_query const& name) {
    perf_count a;
    // it is totaly stupid, but this just for a rest
    a.fan_speed_ = 0;
    for (auto c : name.name_)
        a.fan_speed_ += c;
    a.cpu_remp_ = a.fan_speed_ * 10;

    return a;
}
void performance::setup_datetime(dt t) {
    // just shows datetime
    // pretty function doesn't present in msvc
    std::cerr << "Function fires " << __PRETTY_FUNCTION__ << t.datetime_ << "\n";
}

void persistent::setup(int_value val) {
    val_ = val.value_;
}
int_value persistent::extract() {
    return int_value{val_};
}

