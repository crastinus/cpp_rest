#pragma once
#include <string>

namespace rest{

enum class method_type {
    REQ_GET,
    REQ_POST,
    REQ_HEAD,
    REQ_PUT,
    REQ_DELETE,
    REQ_OPTIONS,
    REQ_TRACE,
    REQ_CONNECT,
    REQ_PATCH,
    REQ_UNKNOWN
};

// none for a filter wrong data
enum class mime_type { JSON = 1, XML = 2, NONE = 3, TEXT = 4 };

struct request {
    method_type method_;
    std::string uri_;
    std::string body_;
    mime_type   mime_;
    mime_type   possible_response_mime_;
};

// response field must be known 
struct response {
    int         code_;
    std::string body_;
    mime_type   mime_;
};

response execute(request&& req);

}
