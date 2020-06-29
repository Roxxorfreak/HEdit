// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_HEADERS_HPP_

    // Header included
    #define HEDIT_SRC_HEADERS_HPP_

    #if !(defined(_WIN32) || defined(__WIN32__))

        #include <pwd.h>
        #include <string.h>
        #include <unistd.h>
        #include <dirent.h>
        #include <ncurses.h>

        // Enable 64bit support for lange files
        #define _FILE_OFFSET_BITS 64

        // Define the path delimiter as string ("/" under Linux)
        constexpr const char* const HE_PATH_DELIMITER = "/";

        // Define the line break string for text files
        constexpr const char* const HE_LINE_BREAK_STRING = "\n";

        // Windows case insensitive string comparison is strcmpi, linux uses strcasecmp
        #define _stricmp strcasecmp
        #define _strnicmp strncasecmp

        // Re-define the secure crt functions
        #define strcpy_s(target, size, source) strcpy(target, source)
        #define strcat_s(target, size, source) strcat(target, source)

        // May be removed, if __STDC_LIB_EXT1__ is defined and __STDC_WANT_LIB_EXT1__ is set to 1
        // (see https://en.cppreference.com/w/c/string/byte/strncpy)
        #define strncpy_s(target, size, source, length) strncpy(target, source, length)

        // Some 64bit function names
        #define _ftelli64(a) ftell(a)
        #define _fseeki64(a, b, c) fseek(a, b, c)

        // By default, the scandir callback function parameter uses a "const" parameter
        #define HE_SCANDIR_CONST const

        // Ignore "array subscript is above array bounds" for GCC 4.9.2
        #if (__GNUC__ == 4 && __GNUC_MINOR__ == 9 && __GNUC_PATCHLEVEL__ == 2)
            #pragma GCC diagnostic ignored "-Warray-bounds"
        #endif

        // This fixes the incompatible parameters of scandir for MacOS X < 10.8
        // #if defined(__APPLE__)
        //   #undef HE_SCANDIR_CONST
        //   #define HE_SCANDIR_CONST
        // #endif

    #else

        #include <io.h>
        #include <conio.h>
        #include <shlobj.h>

        // Define the path delimiter as string ("\" under Windows)
        constexpr const char* const HE_PATH_DELIMITER = "\\";

        // Define the line break string for text files (used for writing the configuration file ".hedit")
        constexpr const char* const HE_LINE_BREAK_STRING = "\r\n";

        // Unused under windows
        #define HE_SCANDIR_CONST

        // Define functions aliases for C++ Builder
        #if defined(__BORLANDC__)
            #define _kbhit kbhit
            #define _stricmp stricmp
            #define _strnicmp strnicmp
        #endif

    #endif

    // C++ Core Guidelines C.128 (C++Builder interpretation vs. The World)
    #if defined(__BORLANDC__)
        #define CPPBUILDER_OVERRIDE override
    #else
        #define CPPBUILDER_OVERRIDE
    #endif

    // Standard header files
    #include <cstddef>
    #include <cstdlib>
    #include <exception>
    #include <stdexcept>
    #include <memory>
    #include <cstdint>
    #include <cinttypes>
    #include <vector>
    #include <map>
    #include <utility>

    // The maximum number of file editors in one HEdit window (also used by the comparator engine).
    constexpr int32_t HE_MAX_EDITORS = 5;

    // The min function
    #define hedit_min(a, b)  (((a) < (b)) ? (a) : (b))
    // The max function
    #define hedit_max(a, b)  (((a) > (b)) ? (a) : (b))

    // Include the HEdit header files
    #include "string.hpp"
    #include "file.hpp"
    #include "console.hpp"
    #include "window.hpp"
    #include "message_box.hpp"
    #include "asm_buffer.hpp"
    #include "asm_opcode_manager.hpp"
    #include "asm_instruction.hpp"
    #include "disassembler.hpp"
    #include "config_file.hpp"
    #include "settings.hpp"
    #include "script_variable.hpp"
    #include "script_command_manager.hpp"
    #include "script_interpreter.hpp"
    #include "marker.hpp"
    #include "menu.hpp"
    #include "undo_engine.hpp"
    #include "base_viewer.hpp"
    #include "text_viewer.hpp"
    #include "hex_viewer.hpp"
    #include "asm_viewer.hpp"
    #include "script_viewer.hpp"
    #include "editor.hpp"
    #include "comparator.hpp"
    #include "formula.hpp"
    #include "hedit.hpp"

#endif  // HEDIT_SRC_HEADERS_HPP_
