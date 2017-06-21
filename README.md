
Rest service for C++
===================

Trying to show easy way of creating rest services for a C++ language. 
Any function in your application might be easy transform to a rest method.

Motivation
----------

Suppose you have an application that uses any http engine (libevent used in current application).
And some function must be used as RESTful method to receive some object and return some other object. 

Suppose this functions in your application must be accessible as RESTful endpoint (path in comment)

    // api/measure
    temperature last_measure();
    // api/clock/set
    void set_clock(clock const& clk);
    // api/clock/get
    clock get_clock();

Function set\_clock doesn't return any object. Rest method for this function will return 201 code if exception isn't thrown.

Library allows you to define handler for this rest methods as

    handler = root(BufferType, "api",
                   prefix("clock",
                       method("set", &set_clock),
                       method("get", &get_clock)),
                   method("measure", &last_measure));

Handler walks through api comparing path and run any method in proper way.

BufferType is a source type of all objects. Every return type will be serialized in it and every input type will be deserialized from it.
For every input/return type (temperature,clock)  serialization/deserialization functions must be defined (see rest\_types.cpp).

Example above implements variant for a static functions. Rest support also member function.
Suppose a function from the example above is a member functions of the classes

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

Handler definition with member functions

    handler = root(BufferType, "api",
                   prefix("clock",
                       method("set", &time_point::set_clock),
                       method("get", &time_point::get_clock)),
                   method("measure", &meter::last_measure));

Handler for classes time\_point and meter create instances in tuple. Every instance is created staticaly. Every type object in handler must have constructor without arguments or have no constructor at all.
Every instance is created once for a type ( for methods set\_clock and get\_clock creates single one time\_point class).
    

How to use
----------

Here assume BufferType is a std::string. 

For api must be created object with description (see rest.cpp).

    auto handler = 
      root(std::string,
          "api",
          prefix(
             .....

Define callback for api. Callback returns request, setting up response and end flag.

    struct request_instance {
         std::string const& request_source();
         void set_response(int code, std::string&& src); 
         void set_text_response(int code, std::string&& src); 
         bool handled(); 
    };

For a path using any container of strings with forward iterator. 

    std::vector<std::string> path = {"api", "persistent", "setup"}; 

Combine it all together 

    void handle_request(RequestInstance& inst, std::vector<std::string> const& path) {
        using range_iterator_t = typename decltype(path)::iterator;
        range<range_iterator_t> rng(path.begin(),path.end());
        handler.walk(rng,inst);
    }

RequestInstance must be return proper source of the query. For path above it might be { "intValue": 8080 }. Test code just save this variable in instance of persistent class in the rest\_root and will be available for rest method "extract".



How it works
------------

root, prefix and method  macros generates classes that combines with each other. root stores all classes for the member functions.

Method walk of the root see all possible pathes for api. Root find out proper method and invoke it.

prefix macro just contains methods and pathes and uses in the middle.

method contains just method name and reference to the function.  method function apply choose proper variant to call function reference. 

Every path from constant literal transformed to string classes ("prefix/method" -> std::tuple<typestring("prefix"),typestring("method")>)


Building
--------

Project depends on libevent. Compiler must be c++14 compatible.

    git clone https://github.com/crastinus/cpp_rest
    cd cpp_rest
    git submodule init
    git submodule update
    mkdir cpp_rest
    cmake ..
    make

Using
-----

Default port for a server is 3333. For now just json POST supported.

Compute sha1sum on server

    url: http://ip:3333/api/compute/sha1
    payload:
    {
      "data" : "Some data for sha1"
    }
    answer: 200 OK
    {
      "sha1": "f07f29200a882adc357b460e8fd11a1025abb628"
    }

Persist some random integer

    url: http://ip:3333/api/persistent/setup
    payload:
    {
      "intValue" : 100
    }
    answer: 201 OK

Extract this integer. Empty payload not supported for now. Server doesn't parse payload for this method (persistent::extract has no arguments)

    url: http://ip:3333/api/persistent/extract
    payload:
    answer: 200 OK
    {
      "intValue" : 100
    }

TODO
----

- application/xml
- other methods
- binary data

