
Rest service for C++
===================

Trying to show easy way creating rest services for a language. 
This is a test project. Don't use samples in production.

Motivation
----------

Suppose you have an application that uses any http engine in self (libevent used in current application).
And some function must be used as rest method receive object and return object. 

Suppose this functions in your application must be accessible as rest (path in comment)

    // api/measure
    temperature last_measure();
    // api/clock/set
    void set_clock(clock const& clk);
    // api/clock/get
    clock get_clock();

Function set\_clock doesn't return any object. Rest method for this function will be return 201 code if exception doesn't thrown.

Service allow you to define handler for this rest methods as

    handler = root(BufferType, "api",
                   prefix("clock",
                       method("set", &set_clock),
                       method("get", &get_clock)),
                   method("measure", &last_measure));

Handler walk throught api and run any method in proper way.

BufferType is a source type of all objects. Every return type will be serialize in it and every input type will be deserialize from it.
For every input/return types (temperature,clock) must be defined serialization/deserialization functions (see rest\_handlers.cpp).

Example above implements variant for a static functions. Rest support also member function.
Suppose function from example above is a member functions of the classes

    struct meter {
        // api/measure
        temperature last_measure();
    };
    
    struct time_point {
        // api/clock/set
        void set_clock(clock const& clk);
        // api/clock/get
        clock get_clock();
    };

Handler definition now

    handler = root(BufferType, "api",
                   prefix("clock",
                       method("set", &time_point::set_clock),
                       method("get", &time_point::get_clock)),
                   method("measure", &meter::last_measure));

Handler for classes time\_point and meter create instances in tuple. Every instance creates staticaly. Every type object in handler must have constructor without arguments.
Every instance creates once for a type ( for methods set\_clock and get\_clock creates single one time\_point class).
    

How to use
----------

For api must be created object with description.

   auto handler = 
     root(std::string,
         "api",
         prefix(
            .....

root, prefix and method macros. Macros is nesseary for creation compile time strings. In other case code will be ugly.
root macro takes type of input/output buffer value witch contains rest payload, root path and list of prefix/method macro.
prefix macro takes prefix path to it and list of methods.
method macro takes method name and function pointer.

As callback for api uses structure like this. This structure must be callback for a http engine. 

    struct RequestInstance {
         std::string const& request_source();
         void set_response(int code, std::string&& src); 
         void set_text_response(int code, std::string&& src); 
         bool handled(); 
    };

Static typification allow invoke as callback any class with this signature. Just buffer type(std::string) must be the same as in root.

As path to method uses any array with elements compatible with strncasecmp function. 

    std::vector<std::string> path = {"api", "persistent", "setup"}; 

And function that handles request 

    void handle_request(RequestInstance& inst, std::vector<std::string> const& path) {
        using range_iterator_t = typename decltype(path)::iterator;
        range<range_iterator_t> rng(path.begin(),path.end());
        handler.walk(rng,inst);
    }

RequestInstance must be return proper source of the query. For path above it might be { "intValue": 8080 }. Test code just save this variable in instance of persistent class in the rest\_root and will be available for rest method "extract".


How it works
------------
From every path creates compile time path by template metaprogramming mechanism ("prefix/method" -> tuple(StrType("prefix"), StrType("method"))). 
For every function/method creates wrapper-holder class(rest\_method). 
From function signatures extract types and creates serialization/deserialization code (deserialize input, invoke function, serialize output).  
If function signature is method then instance of this method creates into root class as part of tuple. For every unique class creates just a single one instance.
