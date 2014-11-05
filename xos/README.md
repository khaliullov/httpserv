XOS
===

## What is this project?

The C++ 11 standard happened a few years back, and the direction the language
has taken profoundly changed quite a bit about programming for me. The
language has become much more expressive, efficient and a lot more fun to
program with.

As I have become more familiar with the library features present in C++ 11 and
C++ 14, I have started to see some areas where the cross-platform portability
of the standard library is deficient for systems-level programming, or where
the C++ library really needs some supplemental functionality.

This project is a collection of reusable and hopefully library-grade code
that supplements the C++ 11, 14 and latter standards.

## So what areas of the standard need to be supplemented?

There are several parts of the C++ standard library that I think could use
a complete overhaul. The most glaring one is the iostreams framework. I
personally am still very unhappy with the speed and interface C++ provides
for doing just basic terminal and file I/O.

Additionally, I think there's a lot of work that can be done to provide
useful abstractions on top of file descriptors, and provide a natural
Posix-like interface.

In coming to understand the above issues, a third area for improvement has
emerged as something that needs a reimagining, and that area is localization.

## Why don't you just use Boost::whatever?

Boost, by design, is a set of *cross-platform* libraries. XOS is not meant to
be portable in the same sense. Everything here is meant to be as thin of a
wrapper as possible around OS facilities. The rationale behind that is that
it allows for better abstraction of the underlying OS.

Perhaps some code here will end up in Boost, perhaps some code from Boost
will end up here (or get re-implemented for licensing compatibility reasons).
That isn't a primary goal, however.

## What is actually here now?

Not much, I'm afraid. Mostly what exists are utility classes that I've gathered
from various projects I have worked on. A good example of such a class is
PosixClock&lt;T&gt;, which implements a clocksource templated on the standard
integral constants that clock\_gettime uses for identifying clock sources.

There's also some useful error handling routines, including utility functions
that make throwing a std::system\_error less of a nuisance, and for getting
a stack trace when exceptions are thrown.

Additionally, some of the work to abstract file descriptors into objects has
been started, and in the process of tackling that I've found it necessary
to create an implementation of the n4100 std::filesystem library.

## Current Status and Roadmap

The current status is that very little work on the loftier goals of the project
has been started. As with any software project, you need to start out with a
firm foundation on which to build abstractions.

Because I have limited time to work on this, the primary platform I am
targeting is x86\_64 Linux. I am not interested in building or testing this
code on other platforms, though i do welcome contributions from people that
do have an interest in using this on other Hardware/OS combinations.

As time goes on, I believe that this project will become very Linux-centric.
As I start to tackle some of the file descriptor abstraction, I expect the
epoll interface to become very prominent in the code.

Right now, the focus is to offer a nearly complete implementation of the
filesystem draft proposal (n4100).

## What's with the name?

I chose XOS for the name as a 3-letter name that is short for
Linux/Unix/Posix-OS. 
