acatl
=====

A simple helper framework with a couple of utility classes.

Features
========

acatl consists of four libraries: acatl, acatl_application, acatl_mqtt, and acatl_network.

acatl
-----

The acatl core library can be used withtout the other libraries. Among other usefull stuff the library features:

- Partial C++17 filesystem implementation
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

acatl_mqtt
----------

- MQTT message parser

acatl_network
-------------

- HTTP parser

Platforms
=========

- Linux
- Mac OS X

Dependencies
============

All dependencies will be downloaded by cmake automatically.

acatl
-----

- [date](https://github.com/HowardHinnant/date) for date and timestamp handling

acatl_application
-----------------

- [argtable3](https://github.com/argtable/argtable3) for command line option
- [linenoise-ng](https://github.com/arangodb/linenoise-ng) as readline replacement

acatl_network
-------------

- [asio](https://think-async.com/Asio/AsioStandalone.html) for networking
- [nodejs HTTP Parser](https://github.com/nodejs/http-parser) for parsing HTTP

