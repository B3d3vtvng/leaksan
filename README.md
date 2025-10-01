# Leaksan - A simple Leak sanitizer that works on macOS

## Build Process

    - Clone the repository
    - To build and install, simply execute 'make install'
    - (optionally) clean up the cloned repository by running 'make clean'

## Usage

    - In the translation units to be sanitized, simply include lksan.h as the first include
    - Leaksan automatically reports leaks at program exit. If you want to report leaks earlier (for example before aborting), simple insert lksan_report_leaks(FILE* out, bool free_list). The first parameter specifies the output file / pipe, for example stderr or a log file. The second parameter should only be set to true if you do not plan on calling lksan_report_leaks again (for example when aborting after).

    - During the building stage, simply link liblksan with '-llksan'. Make sure /usr/local/lib is in your library path as this is where the library will be installed

    - Even using a custom allocator is no problem, simply see 'custom_alloc/' under examples