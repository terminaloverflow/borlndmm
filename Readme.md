About
=====
A minimal replacement library for borlndmm.dll which wraps the native Windows heap allocator. It would be straight forward to adapt this library to use a different allocator back-end such as jemalloc, tcmalloc or mimalloc.

Debug builds can define POISON_HEAP_USER_DATA in order to memset the newly allocated or freed memory blocks with non NULL bytes which can help identify uninitialized variable or use after free issue. Tools like WinDbg can enable additional heap debugging features for native heaps, such as user stack traces, and the WinDbg !heap command can query allocations. This can be beneficial in chasing down memory allocation issues. To benefit from this you should convert your map files to dbg files via map2dbg so WinDbg will give you correct symbols.

Release builds benefit from the native Windows heap allocators performance as well as the built in heap security mitigations.

To avoid linking in the default runtime library (ie. CC32260MT.DLL or CC64260MT.DLL) we build an empty c0d32w.obj and c0d64w.a.

To ensure ilink64 will generate the expected export names, we use a DEF file to manually specify the exports.

Build
=====
Build borlndmm.cbproj with C++ Builder (Tested using version 10.3.2).

License
=======
Released under the 3-Clause BSD License.