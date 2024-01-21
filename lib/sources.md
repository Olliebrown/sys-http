# Lib Sources
This folder contains 3rd party libraries that have been modified to work on the switch.

## From Stratosphere Library (part of Atmosphere):
To avoid including the entire libstratosphere, the following files are extracted from it:
- dmntcht.c/.h: High-level interface to dmnt:cht, a cheat engine for Atmosphere.
- service_guard.h: Tool to count references to a service for automatic garbage collection.

Original source is here: https://github.com/Atmosphere-NX/Atmosphere-libs/tree/master/libstratosphere

## yhirose/cpp-httplib:
This is a single-file header-only library for HTTP and HTTPS written in C++11. It has no required
dependencies on external libraries such as OpenSSL or zlib (but will utilize them if they are available).

It has been modified to work with the switch socket API and split into a .c and .h file (using the
provided python script).

- Original source is here: https://github.com/yhirose/cpp-httplib

## mkjson library:
mkjson is a simple and flexible single-function C library intended to make building JSON strings in C
easier. Making use of variable length argument lists, it allows you to create complex JSON objects in just
one line and lets you specify the appropriate JSON types instead of trying to infer them from c types.

- Original source is here: https://github.com/Jacajack/mkjson
- Modified by zaksabeast for sys-http: https://github.com/zaksabeast/sys-http
