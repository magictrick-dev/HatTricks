# Magic Trick's Hat Tricks

A compendium of various programming ideas, theory testing, and concepts of mine, compiled into one place.
Each subsection of this repository is a standalone project or test suite of mine. Feel free to browse,
test, and use whatever you like.

Some of these projects may be WIPs; which means that they aren't complete. This
repository is meant to be a showcase, sandbox, and theory testing location for
things that I would otherwise not make a separate repository for.

### Development Environments

Unless expressly stated in the project directory, most of these projects are built
using [a build tool called CMake](https://cmake.org/). One design philosophy
(and development pet-peeve) is dependency hunting; having to fetch each little library,
include it, and then attempt to compile everything together is such a pain. As such,
any depencies that allow for free distribution will automatically be included in-source
so that you don't need to do anything except configure and build using CMake for the
targetted OS.

For the most part, if the code I am writing can be written for multiple platforms
without much trouble, then I will do so. I tend to write short-hand scripts to help
with building and running my applications. If you see PowerShell scripts `*.ps1`,
then the project probably builds for Windows. Likewise, `*.sh` scripts probably means
it builds for Unix as well. This isn't a hard and fast rule, though.

### Glossary

[DLL / Share Library Hotloading Concepts](./Hotloading) **WIP**

**About:** Hotloading is a very simple, yet powerful construct rarely utilized for C and C++ applications.
This demonstration highlights the capabilities of hotloading by allowing the user to define a set front-end API
which dynamically loads itself into memory every time it detects a change (a compilation, in this case).
