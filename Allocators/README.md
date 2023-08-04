# Stack Allocators

There are many ways applications manage memory, but one of the simplest
and most robust way of handling heap allocations is through stack allocators. Stack
allocators request a fixed region of memory and utilize a push/pop interface to receive
arbitrary regions of memory. This method is also extremely fast, particularly for
applications that regularly deallocate and allocate large regions of space very frequently.

### Implementation Details

Working with stack allocators is easy, and implementing them is even easier. However,
there are ways to make them *better* with a few extra steps!

We probably don't want to use the C/C++ standard library implementations of
`malloc()` or `new` to fetch blocks of memory we are using for our stack allocator.
We can bypass them by invoking an OS-call to fetch a region of virtually allocated memory.

What is virtually allocated memory? In simple terms, virtual memory is an intermediate
interface between raw memory (such as RAM) and the application. The operating system
may move where it stores its memory to other places in RAM or even to the disk.
Virtual memory creates a map from "virtual memory space" to "physical memory space"
to abstract this behavior away from the application.

An application programmer rarely cares where its memory is, only that the memory it
received is sized appropriately and is contiguous (when requested as some region of
*n*-bytes). So when we make calls to `new` or `malloc()`, we are not getting physical
addresses to RAM, but virtual addresses that map to *somewhere* where that memory
is being stored.

We can use the OS system calls to fetch regions of virtual memory and manage their
lifetimes ourselves. This is ideal because it removes responsibility from the standard
library which will create internal datastructures to maintain information about each
allocation.

Since the operating system keeps track of each region of virtually allocated memory
for book keeping purposes, it doesn't make sense to keep track of small allocations.
Instead, operating systems returns regions of memory called "pages" which correspond
to some minimum granularity size as defined by the OS. Pages are then given a set
number of flags that define how they're used; generally read and write and sometimes
even execute. Pages are marked with protection flags which determine what happens
when they accessed.

Pages that are not allocated to the process have read, write, and execute protection enabled.
When you attempt to access them, you will get the infamous **segmentation fault** error.
Buffer overruns, for example, trigger segfaults because they run into pages of memory
which the process does not have access to.

The reason this is important is because we are now taking responsibility of asking
the operating system for pages. We need to set our access flags, set protection,
and manage their lifetime ourselves. It's fairly straight forward; especially if
you just want a region of memory, but nonetheless we should know what all of this
means before we begin to play with it.

Here is what we need to do:

- We need a structure which contains the pointer to the region of virtually allocated
memory. Additionally, we should have store how large this region is. Finally, we
should also keep track of how large the stack is so we know where to allocate to.

- We need to write a couple helper functions that fetch regions of virtually allocated
memory for us. We aren't interested in the flags and protection behaviors; the standard
flags and protection is fine. We also want to procure that maximum granularity of a
page that we can. We are able to get sub-partitions as low as the minimum page granularity
(each sub-partition may have its own protection properties) but since the OS is
giving us the whole page regardless if we request a sub-partition of it, we might as
well use all of it.

- We finally need to write the push/pop functions and define a couple macros to
make fetching C-style structs and arrays easier. We will also need to implement
a way of constructing C++ objects as well. (Yes, OOP-programmers, rejoice! I did
not forget about you!)

**Helpful Notes:**

Stack allocators are *not general allocators*. They do not replace general allocators.
It is possible to create a general allocator using a stack allocator, but I would not
advise it. The C++ standard library and C's `malloc()` / `free()` are better suited
for this. General allocators best work in conditions heap allocations are not predictable.
Stack allocators work best in situations where your allocations are predictable and
follow a set execution flow.

Stack allocators are the backbone for "frame allocators"; a region of heap allocated
memory that you supply to a runtime loop that gets reset every "frame" or iteration
of the loop.

You will probably want to write robust memory set routines. It is possible to write
faster `memset()` and `memcpy()` routines for the stack allocator!

