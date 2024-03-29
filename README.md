# Magic Trick's Hat Tricks

A compendium of various programming ideas, theory testing, and concepts of mine,
compiled into one place. Each subsection of this repository is a standalone
project or test suite. Feel free to browse, test, and use whatever you like for
your personal projects.

Some of these projects may be WIPs; which means that they aren't complete. This
repository is meant to be a showcase, sandbox, and theory testing location for
things that I would otherwise not make a separate repository for.

### Development Environments

Unless expressly stated in the project directory, most of these projects are built
using [a build tool called CMake](https://cmake.org/). A huge development pet-peeve
of mine is dependency hunting; having to fetch each little library, then
include it, and then attempt to compile everything together is such a pain. As such,
any depencies that allow for free distribution will automatically be included in-source
so that you don't need to do anything except configure and build using CMake for the
targetted OS. You're welcome!

For the most part, if the code I am writing can be written for multiple platforms
without much trouble, then I will do so. I tend to write short-hand scripts to help
with building and running my applications. If you see PowerShell scripts `*.ps1`,
then the project probably builds for Windows. Likewise, `*.sh` scripts probably means
it builds for Unix as well. This isn't a hard and fast rule, though.

# Glossary

[DLL / Share Library Hotloading Concepts](./Hotloading) **Example Complete**

**About:**
Hotloading is a very simple, yet powerful construct rarely utilized for C and C++ applications.
This demonstration highlights the capabilities of hotloading by allowing the user to define a set front-end API
which dynamically loads itself into memory every time it detects a change (a compilation, in this case).

[Memory Management & Custom Allocators](./Allocators) **Example Complete**

**About:**
There are many ways applications manage memory, but one of the simplest
and most robust way of handling heap allocations is through stack allocators. Stack
allocators request a fixed region of memory and utilize a push/pop interface to receive
arbitrary regions of memory. This method is also extremely fast, particularly for
applications that regularly deallocate and allocate large regions of space very frequently.

[In-Memory DLLs](./MemoryLibrary) **Example Complete**

**About:**
Loading DLLs into memory is not something Windows *wants* us to do, but I don't
care what Windows wants, so I found a library that allows this to work. It's a nice
little hack that we can use for in-dev stuff, but sorta useless on its own since
it doesn't serve much more of an advantage that traditionally loading the library.

[Library Data Packing](./LibraryPacking)

**About:**
Along with loading libraries into memory, I also want to explore packing additional
data into a DLL file and see what happens. If we can sneak some resources in there,
then we could potentially use the Memory Library & Library Packing techniques for
some cool data-packing stuff.

[Non-blocking Windows](./NonblockWindow) **Example Complete**

**About:**
Fixing something that should have never existed in the window procedure: loop-locked
messages. Fixing this requires the use of a seperate thread that creates windows for
us. When that thread creates a window, it maintains control over the window procedure
and allows a seperate thread to run concurrently so that it is no longer blocking.
