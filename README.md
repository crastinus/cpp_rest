
Rest service for C++
===================

Trying to show easy way creating rest services for a language. 

How to use
----------

For api must be created object with description.

   auto handler = 
     root(std::string,
         "api",
         prefix("datetime",
                method("log", &time::extract_log),
                method("setup", &performance::setup_datetime)),
         prefix("compute", 
                method("accumulate", &performance::extract ),
                method("sha1", &time::compute_sha1)),
         prefix("persistent",
                method("setup", &persistent::setup ),
                method("extract", &persistent::extract)));

root, prefix and method macros. Macros is nesseary for creation compile time strings.
root macro takes type of input/output buffer value witch contains rest payload, root path and list of prefix/method macro.
prefix macro takes prefix path to it and list of methods.
method macro takes method name and function pointer.

As callback for api uses structure like this.  

    struct RequestInstance {
        std::string const& request_source()  { }
         void set_response(int code, std::string&& src) { }
         void set_text_response(int code, std::string&& src) { }
         bool handled() { }
    };

Static typification allow invoke as callback any class with this signature. Just buffer type(std::string) must be the same as in root.

As path to method uses any array with elements compatible with strncasecmp function. 

    std::vector<std::string> path = {"api", "persistent", "setup"}; 

Finally code

    void handle_request(RequestInstance& inst) {
        using range_iterator_t = typename decltype(path)::iterator;
        range<range_iterator_t> rng(path.begin(),path.end());
        handler.walk(rng,inst);
    }

RequestInstance must be return proper source of the query. For path above it will be { "intValue": 8080 }. Test code just save this variable in instance of persistent class and will be available for rest method "extraxt".


How it works
------------
From every path creates compile time path by template metaprogramming mechanism ("prefix/method" -> tuple(StrType("prefix"), StrType("method"))). 
For every function/method creates wrapper-holder class(rest\_method). 
From function signatures extract types and creates serialization/deserialization code (deserialize input, invoke function, serialize output).  
If function signature is method then instance of this method creates into root class as part of tuple. For every unique class creates just a single one instance.
