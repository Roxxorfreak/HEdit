// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

// The main function
int main(int argc, char* argv[])
{
    // Say hello
    printf("*** %s - Advanced HexEditor - %s ***\n\n", HE_PROGRAM_TITLE, HE_PROGRAM_COPYRIGHT);

    // If no parameters are specified, display the basic program usage
    const auto file_count = static_cast<int32_t>(argc - 1);
    if ((file_count < 1) || (file_count > HE_MAX_EDITORS))
    {
        printf("USAGE: hedit file [file] [file] [file] [file]\n\nNo wildcards allowed\nUse hedit --help for help about the editor\n");
        return EXIT_FAILURE;
    }

    // Check for help parameters (-h, --help, -?, ?, /?)
    if ((_stricmp(argv[1], "-h") == 0) || (_stricmp(argv[1], "--help") == 0) || (_stricmp(argv[1], "-?") == 0) ||
        (_stricmp(argv[1], "?") == 0) || (_stricmp(argv[1], "/?") == 0))
    {
        printf("Basic keys: Cursor Keys    - Navigate through the file\n");
        printf("            Pos1, End\n");
        printf("            PgUp, PgDown\n");
        printf("            TAB            - Switch to next editor window\n");
        printf("            ESC            - Exit HEdit\n");
        printf("            F1             - Detailed help\n");
        printf("            F5             - Jump to cursor\n");
        printf("            F6             - Goto address\n");
        printf("            F7             - Continue search\n");
        printf("            F8             - Start search\n");
        printf("            F9             - File operations\n");
        printf("            F10            - List available viewers\n");
        printf("            Shift-F1       - Viewer: Hex-Dec (default)\n");
        printf("            Shift-F2       - Viewer: Disassembler\n");
        printf("            Shift-F3       - Viewer: Text\n");
        printf("            Shift-F4       - Viewer: user-defined (via config file)\n");
        return EXIT_FAILURE;
    }

    // Start the hex editor
    try
    {
        // Create a new hex editor object
        std::unique_ptr<THEdit> hedit(new THEdit());

        // Start the editor
        hedit->Edit(file_count, &argv[1]);

        // Application successfully ended
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        // Display the error
        printf("Fatal Error: %s\n", e.what());
    }
    catch (...)
    {
        // Display the error
        printf("Fatal Error: undefined exception\n");
    }

    // Exit with error code
    return EXIT_FAILURE;
}
