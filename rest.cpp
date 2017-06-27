#include "rest.hpp"
#include "meta/root.hpp"
#include "rest_handlers.hpp"
#include <utils/string/string.hpp>
//#include <iostream>
//#include <boost/type_index.hpp>



namespace rest{

auto handler = root(std::string,
                    "api",
                    prefix("datetime",
                           method("log", &time::extract_log),
                           method("setup", &performance::setup_datetime)),
                    prefix("compute", 
                           method("accumulate", &performance::extract ),
                           method("sha1", &hash::compute_sha1)),
                    prefix("persistent",
                           method("setup", &persistent::setup ),
                           method("extract", &persistent::extract)));

/*\brief request callback
 *
 * contains all information for request
 */
struct request_instance {
    request_instance(request& req, response& resp)
        : req_(req), resp_(resp), handle_(false) {
    }
    std::string const& request_source()  { return req_.body_;}
     void set_response(int code, std::string&& src) {
        resp_.code_ = code;
        resp_.body_ = std::move(src);
        handle_ =true;
     }
     void set_text_response(int code, std::string&& src) {
         resp_.code_ = code;
         resp_.body_ = std::move(src);
         resp_.mime_ = rest::mime_type::TEXT;
         handle_ =true;
     }
     bool handled() {
         return handle_;
     }

    request&  req_;
     response& resp_;
     bool      handle_;
};
 
//auto meth = method("log", &time::extract_log);
response execute(request&& req){
    response resp;
    resp.code_ = 404;
    resp.mime_ = req.possible_response_mime_;

    using iterator_t = typename std::vector<std::string>::iterator;
    auto vec = utils::split(req.uri_, '/');
    range<iterator_t> rng{vec.begin(), vec.end()};

    //    std::cerr <<
    //    boost::typeindex::type_id_with_cvr<decltype(handler)>().pretty_name() <<
    //    "\n";
    request_instance inst(req, resp);
    handler.walk(inst,rng);
    
    return resp;
}
}
