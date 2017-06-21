
#include <event2/http.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include <string.h>

#include <iostream>
#include "utils/string/string.hpp"

#include "rest.hpp"

template<typename... Args>
void message_impl(std::ostream& out, Args&&...args){
    using swallow = int[];
    (void)swallow{ (out << args ,0)...};
    out << "\n";
}

template <typename... Args>
void fatal(Args&&... args) {
    message_impl(std::cerr, std::forward<Args>(args)...);
    exit(-1);
}

template <typename... Args>
void message(Args&&... args) {
    message_impl(std::cout, std::forward<Args>(args)...);
}

rest::mime_type choose_return_format(char const* str);

void rest_callback(struct evhttp_request* req, void* /*arg*/){
	//const char *cmdtype;
	struct evkeyvalq *headers;
	//struct evkeyval *header;
	struct evbuffer *buf;

   
    rest::method_type methtype;

	switch (evhttp_request_get_command(req)) {
        case EVHTTP_REQ_GET:     methtype = rest::method_type::REQ_GET; break;
        case EVHTTP_REQ_POST:    methtype = rest::method_type::REQ_POST; break;
        case EVHTTP_REQ_HEAD:    methtype = rest::method_type::REQ_HEAD; break;
        case EVHTTP_REQ_PUT:     methtype = rest::method_type::REQ_PUT; break;
        case EVHTTP_REQ_DELETE:  methtype = rest::method_type::REQ_DELETE; break;
        case EVHTTP_REQ_OPTIONS: methtype = rest::method_type::REQ_OPTIONS; break;
        case EVHTTP_REQ_TRACE:   methtype = rest::method_type::REQ_TRACE; break;
        case EVHTTP_REQ_CONNECT: methtype = rest::method_type::REQ_CONNECT; break;
        case EVHTTP_REQ_PATCH:   methtype = rest::method_type::REQ_PATCH; break;
        default: methtype = rest::method_type::REQ_UNKNOWN; break;
	}

    // post implemented for now
    if (methtype != rest::method_type::REQ_POST && methtype != rest::method_type::REQ_GET){
        evhttp_send_reply(req, 501, "Just POST method implemented.", nullptr);
        return;
    }

    rest::request request;
    request.method_ = methtype;
    request.uri_    = evhttp_request_get_uri(req);

	buf = evhttp_request_get_input_buffer(req);
    request.body_.resize(evbuffer_get_length(buf));

    // extact data from buffer
    auto data = &*request.body_.begin();
    auto size = request.body_.size();
    evbuffer_remove(buf, data, size);

   headers = evhttp_request_get_input_headers(req);

    auto content_type_header = evhttp_find_header(headers, "Content-Type");
    if (content_type_header != nullptr) {
        if (strcmp(content_type_header, "application/json") != 0) {
            evhttp_send_reply(req, 415, "Just application/json type supported", nullptr);
            return;
        }
    }

    auto acc_header = evhttp_find_header(headers, "Accept");
    if (acc_header != nullptr) {
        if (choose_return_format(acc_header) != rest::mime_type::JSON) {
            evhttp_send_reply(req, 415, "Just application/json type supported", nullptr);
            return;
        }
    }

    // default content type is json
    // no one another content type doesn't supported for now
    request.mime_                   = rest::mime_type::JSON;
    request.possible_response_mime_ = rest::mime_type::JSON;

    auto result = rest::execute(std::move(request));
    if (result.code_ == 201) {
        evhttp_send_reply(req, 201, "OK", NULL);
        return;
    }

    char const* mime_str;
    switch(result.mime_){
        case rest::mime_type::XML : mime_str = "application/xml"; break; 
        case rest::mime_type::JSON : mime_str = "application/json"; break; 
        case rest::mime_type::NONE :
        case rest::mime_type::TEXT : mime_str = "text/html"; break; 
    }

    // error will be xml/json too
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type",
                      mime_str);

    auto evb = evbuffer_new();
    evbuffer_add(evb, &*result.body_.begin(), result.body_.size());

    //result response
    evhttp_send_reply(req, result.code_, (result.code_ == 200 ? "OK" : "Error"), evb);
}


int main() {
    struct event_base*          base;
    struct evhttp*              http;
    struct evhttp_bound_socket* socket;
    ev_uint16_t                 port = 3333;

    base = event_base_new();
    if (base == nullptr){
        fatal("cannot initialize event base");
    }

    http  = evhttp_new(base);
    if (http == nullptr){
        fatal("cannot initialize event http");
    }

	evhttp_set_gencb(http, rest_callback, NULL);

	socket = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
    if(socket == nullptr){
        fatal("Cannot open socket on random port (", port,")");
    }

    message("Listening on ", "0.0.0.0:", port, "\n");

    event_base_dispatch(base);
}

rest::mime_type choose_return_format(char const* str) {
   
    // this is not production server. Handle mime type in the easiest way
    std::string easy(str);
    bool json_found = (easy.find("application/json") != std::string::npos);
    bool xml_found = (easy.find("application/xml") != std::string::npos);
    bool any_found = (easy.find("*/*") != std::string::npos);

    // use xml in the one particular case when no other types available
    
    if (json_found || any_found)
        return rest::mime_type::JSON;
    if (xml_found)
        return rest::mime_type::XML;
    return rest::mime_type::NONE;
}
