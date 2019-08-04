acatl
=====

A simple helper framework with a couple of utility classes. It also features a nearly complete filesystem implementation that follows the C++17 standard.

Features
========

acatl consists of three libraries: acatl, acatl_application and acatl_network.

acatl
-----

The acatl core library can be used withtout the other libraries. Among other usefull stuff the library features:

- C++17 filesystem implementation
- Threadpool
- String helper
- Time helper
- JSON parser
- Safe numeric casts
- Logging

acatl_application
-----------------

- Application template
- Command line options following the POSIX Utility Conventions
- Console for interactive shells with history and readline default key bindings

acatl_network
-------------

- HTTP parser

Platforms
=========

- Linux
- Mac OS X

Dependencies
============

All dependencies are embedded into the acatl source code.

acatl
-----

The acatl core library has no dependencies to thirdparty stuff.

acatl_application
-----------------

- [argtable3](https://github.com/argtable/argtable3) for command line option
- [linenoise-ng](https://github.com/arangodb/linenoise-ng) as readline replacement

acatl_network
-------------

- [asio](https://think-async.com/Asio/AsioStandalone.html) for networking
- [nodejs HTTP Parser](https://github.com/nodejs/http-parser) for parsing HTTP


