#    CMakeLists.txt -- libbacktrace CMake build script
#    Contributed by Alexander Monakov, ISP RAS
#    Updated by Thomas C. Etavard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     (1) Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#     (2) Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#
#     (3) The name of the author may not be used to
#     endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

set (BACKTRACE_SUPPORTED 1)

include (CheckSymbolExists)
check_symbol_exists (_Unwind_Backtrace unwind.h HAVE_BACKTRACE)

if (HAVE_BACKTRACE)
    set (BACKTRACE_FILE src/backtrace.c src/simple.c)
else ()
    set (BACKTRACE_FILE src/nounwind.c)
    set (BACKTRACE_SUPPORTED 0)
    message (STATUS "_Unwind_Backtrace() not found. Disabling Backtrace support.")
endif ()

include (CheckCCompilerFlag)
check_c_compiler_flag ("-funwind-tables" FLAG_UNWIND_TABLES)
if (FLAG_UNWIND_TABLES)
    add_definitions ("-funwind-tables")
endif ()

# Adjust warnings
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions ("-Wno-switch -Wno-enum-compare")
endif ()

#As of October 2020, libbacktrace supports ELF, PE/COFF, Mach-O, and
#XCOFF executables with DWARF debugging information.
#In other words, it supports GNU/Linux, *BSD, macOS,

set (BACKTRACE_SUPPORTS_DATA 1)
if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    set (FORMAT_FILE src/elf.c src/dwarf.c)
    math (EXPR BACKTRACE_ELF_SIZE 8*${CMAKE_C_SIZEOF_DATA_PTR})
elseif (CMAKE_EXECUTABLE_FORMAT STREQUAL "COFF" OR WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set (BACKTRACE_SUPPORTS_DATA 0)
    set (FORMAT_FILE src/pecoff.c src/dwarf.c)
elseif (CMAKE_EXECUTABLE_FORMAT STREQUAL "MACHO")
    set (BACKTRACE_SUPPORTS_DATA 0)
    set (FORMAT_FILE src/macho.c src/dwarf.c)
elseif (CMAKE_EXECUTABLE_FORMAT STREQUAL "XCOFF")
    set (BACKTRACE_SUPPORTS_DATA 0)
    set (FORMAT_FILE src/xcoff.c src/dwarf.c)
    math (EXPR BACKTRACE_XCOFF_SIZE 8*${CMAKE_C_SIZEOF_DATA_PTR})
else ()
    set (FORMAT_FILE src/unknown.c)
    set (BACKTRACE_SUPPORTED 0)
    set (BACKTRACE_SUPPORTS_DATA 0)
    message (STATUS "Executable format is Unknown. Disabling Backtrace support.")
endif ()

check_symbol_exists (mmap sys/mman.h HAVE_MMAP)

if (HAVE_MMAP)
    set (VIEW_FILE src/mmapio.c)
    check_symbol_exists (MAP_ANONYMOUS sys/mman.h HAVE_MMAP_ANONYMOUS)
    check_symbol_exists (MAP_ANON sys/mman.h HAVE_MMAP_ANON)
    if (HAVE_MMAP_ANONYMOUS AND HAVE_MMAP_ANON)
	set (ALLOC_FILE src/mmap.c)
    else ()
	set (ALLOC_FILE src/alloc.c)
    endif ()
else ()
    set (VIEW_FILE src/read.c)
    set (ALLOC_FILE src/alloc.c)
endif ()

if (ALLOC_FILE STREQUAL "src/alloc.c")
    set (BACKTRACE_USES_MALLOC 1)
else ()
    set (BACKTRACE_USES_MALLOC 0)
endif ()

add_definitions (-D_GNU_SOURCE)
set (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_GNU_SOURCE)

if(NOT HAVE_ATOMIC_FUNCTIONS AND HAVE_SYNC_FUNCTIONS)
    set(ATOMIC_FILE src/atomic.c)
endif()

configure_file (src/backtrace-supported.h.in gen/backtrace-supported.h)
configure_file (src/config.h.in.cmake gen/config.h)

# Add target library
add_library(libbacktrace STATIC
    ${BACKTRACE_FILE}
    ${FORMAT_FILE}
    ${VIEW_FILE}
    ${ALLOC_FILE}
    ${ATOMIC_FILE}
    src/fileline.c
    src/posix.c
    src/print.c
    src/sort.c
    src/state.c
)
add_library(libbacktrace::libbacktrace ALIAS libbacktrace)

target_include_directories(libbacktrace PUBLIC BEFORE ${CMAKE_CURRENT_BINARY_DIR}/gen)
target_include_directories(libbacktrace PUBLIC src)
target_compile_definitions(libbacktrace PUBLIC
    BACKTRACE_ELF_SIZE=${BACKTRACE_ELF_SIZE}
    BACKTRACE_XCOFF_SIZE=${BACKTRACE_XCOFF_SIZE}
)
