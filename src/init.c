// -----------------------------------------------------------------------------
// INIT_C
//
// - Hardware Test Tool initialization screen
//
// -----------------------------------------------------------------------------
// Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta
//
// This source code is distributed under the terms of the MIT License. For more
// information, please see the LICENSE.txt file or refer to the following link:
//
//                                           https://opensource.org/license/mit/
// -----------------------------------------------------------------------------

#ifndef   WIN32
#error    "This program is targeted to systems running a Microsoft Windows OS!"
#else  // WIN32

// ------------------------ Private headers includes ------------------------ //

#include  "public.h"

// ---------------------- Private preprocessor macros ----------------------- //

#define   LSBY                                                               0xF
#define   MIN_MAJ_V                                                            6
#define   MIN_MEN_V                                                            1

#define   MIN_DISPLAY_X_RES                                                 1024
#define   MIN_DISPLAY_Y_RES                                                  768
#define   MAX_TASKBAR_Y_RES                                                   72

#define   N_THEADS                                                             2
#define   BACK_T_N                                                             0
#define   HWTT_T_N                                                             1

#define   DELAY_MS                                                            50

#define   CMD_HEIGHT                                                          43
#define   CMD_SCROLL                                                        2048

#define   SEL_MODE_Y_POS                                                      35

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

// -------------- Private global data holders initializations --------------- //

static const char hwtt_title[] = "Hardware Test Tool";

// --------------------- Private functions declarations --------------------- //

static void back_thread(void);
static void hwtt_thread(void);

static void check_env(void);
static void setup_cmd(void);
static void clear_cmd(void);
static void run_loop(void);
static void prompt_error(const char *top, const char *msg);

// ---------------------- Public functions definitions ---------------------- //

int main(void) {
    // Check if the system meets the minimum required operative system version
    // and display resolution.
    check_env();

    // Create the threads: a background thread to detect if the program is
    // running in full screen, killing it because it is not allowed; and the
    // main thread, which performs the program's stuff.
    HANDLE hdl[N_THEADS] = {0};
    hdl[BACK_T_N] = CreateThread(NULL, 0, (void *)back_thread, NULL, 0, NULL);
    hdl[HWTT_T_N] = CreateThread(NULL, 0, (void *)hwtt_thread, NULL, 0, NULL);

    // Wait until the main thread finishes (the background thread never
    // finishes during the program's execution).
    WaitForMultipleObjects(N_THEADS, hdl, FALSE, INFINITE);

    return 0;
}

// --------------------- Private functions definitions ---------------------- //

static void back_thread(void) {
    // Run this thread infinitely.
    for (;;) {
        // Check if the dimensions of the console are equals of the ones of the
        // display resolution. If they are equal, it is because the program is
        // running in full screen, which is not allowed, so the program is
        // killed after prompting an error.
        HWND cons_hdle = GetConsoleWindow();
        HWND desk_hdle = GetDesktopWindow();
        RECT cons_rect = {0};
        RECT desk_rect = {0};
        GetWindowRect(cons_hdle, &cons_rect);
        GetWindowRect(desk_hdle, &desk_rect);
        if (    cons_rect.left   == desk_rect.left   &&
                cons_rect.top    == desk_rect.top    &&
                cons_rect.right  == desk_rect.right  &&
                cons_rect.bottom == desk_rect.bottom) {
            prompt_error(
                    "Unsupported window mode",
                    "Running this program in full screen is not allowed!"
            );
        }

        // Delay 50 ms to avoid overloading the CPU.
        Sleep(DELAY_MS);
    }
}

static void hwtt_thread(void) {
    // Perform the console's window setup.
    setup_cmd();

    // Print the header and the logo at the top.
    write_header(stdout, NULL, hwtt_title);
    output(stdout, NULL, logo);

    // Print the build information.
    write_header(stdout, NULL, "Build Information");
    show_version(stdout, NULL);

    // Print the mode selection options.
    write_header(stdout, NULL, "Mode Selection");
    output(stdout, NULL, modes);
    output(stdout, NULL, "\n");
    output(stdout, NULL, "\n");
    output(stdout, NULL, "\n");

    // Print the copyright information at the bottom.
    output(stdout, NULL, copyright);
    output(stdout, NULL, "\n");

    // Print the MIT License (hidden, scroll needed).
    output(stdout, NULL, license);

    // Request the mode number to be used and perform its stuff.
    run_loop();

    // Indicate that the program has ended and request an ENTER keystroke to
    // exit.
    write_header(stdout, NULL, "Program Finished");
    input(stdout, NULL, " <- Press ENTER to exit ... ", NULL, ORIGIN, ORIGIN,
            ORIGIN, "\r");
}

static void check_env(void) {
    // Prevent this program from running on systems that do not have at least
    // Windows 7 (x64). This is done because some Windows API functions (in the
    // Ethernet module) have been used that may not be compatible with earlier
    // versions of the operating system.
    DWORD dwVersion = GetVersion();
    DWORD dwMajorVersion = (dwVersion >>                0) & LSBY;
    DWORD dwMinorVersion = (dwVersion >> BITS_IN_ONE_BYTE) & LSBY;
    ret =    dwMajorVersion <  MIN_MAJ_V ||
            (dwMajorVersion == MIN_MAJ_V && dwMinorVersion < MIN_MEN_V);
    if (ret != 0) {
        prompt_error(
                "Unsupported operative system",
                "Windows 7 (x64) or higher is required to run this software."
        );
    }

    // Prevent this program from running if the display resolution is too small.
    // This is done to avoid a failure in the automatic display size adjustment.
    int x = GetSystemMetrics(SM_CXFULLSCREEN);
    int y = GetSystemMetrics(SM_CYFULLSCREEN);
    if (x < MIN_DISPLAY_X_RES || y < MIN_DISPLAY_Y_RES - MAX_TASKBAR_Y_RES) {
        prompt_error(
                "Unsupported resolution",
                "The minimum required resolution is 1024x768 (if the task bar i"
                "s at the bottom with a height of 72 pixels)."
        );
    }
}

static void setup_cmd(void) {
    // Disable the window manual resizing.
    HWND hWnd = GetConsoleWindow();
    int nIndex = GWL_STYLE;
    LONG dwNewLong = GetWindowLongA(hWnd, nIndex) & ~WS_MAXIMIZEBOX &
            ~WS_SIZEBOX;
    SetWindowLongA(hWnd, nIndex, dwNewLong);

    // Disable the mouse interaction.
    HANDLE hConsoleHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwMode = ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT;
    SetConsoleMode(hConsoleHandle, dwMode);

    // Adjust the console size and buffer. Note that a function is used twice to
    // avoid a graphic error when scrolling (disappearing lines).
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    WINBOOL bAbsolute = TRUE;
    CONST SMALL_RECT stConsoleWindow = {
        .Left   = 0,
        .Top    = 0,
        .Right  = CMD_WIDTH  - 1,
        .Bottom = CMD_HEIGHT - 1
    };
    COORD dwSize = {CMD_WIDTH, CMD_SCROLL};
    SetConsoleWindowInfo(hConsoleOutput, bAbsolute, &stConsoleWindow);
    SetConsoleScreenBufferSize(hConsoleOutput, dwSize);
    SetConsoleWindowInfo(hConsoleOutput, bAbsolute, &stConsoleWindow);

    // Insert the title of the window.
    SetConsoleTitleA(hwtt_title);

    // Clear the screen (just in case if the program is executed from an
    // already opened console).
    clear_cmd();
}

static void clear_cmd(void) {
    // Get information of the window buffer.
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO stConsoleScreenBufferInfo = {0};
    GetConsoleScreenBufferInfo(hConsoleOutput, &stConsoleScreenBufferInfo);

    // Perform a sweep to clear the window (using blank spaces).
    SMALL_RECT stScrollRectangle = {
        .Left   = 0,
        .Top    = 0,
        .Right  = stConsoleScreenBufferInfo.dwSize.X,
        .Bottom = stConsoleScreenBufferInfo.dwSize.Y
    };
    SMALL_RECT *lpClipRectangle = NULL;
    COORD dwDestinationOrigin = {
        .X = 0,
        .Y = 0 - stConsoleScreenBufferInfo.dwSize.Y
    };
    CHAR_INFO stFill = {
        .Char.AsciiChar = ' ',
        .Attributes = stConsoleScreenBufferInfo.wAttributes
    };
    ScrollConsoleScreenBufferA(hConsoleOutput, &stScrollRectangle,
            lpClipRectangle, dwDestinationOrigin, &stFill);

    // Set the cursor at the beginning of the window.
    stConsoleScreenBufferInfo.dwCursorPosition.X = 0;
    stConsoleScreenBufferInfo.dwCursorPosition.Y = 0;
    SetConsoleCursorPosition(hConsoleOutput,
            stConsoleScreenBufferInfo.dwCursorPosition);
}

static void run_loop(void) {
    // Rewind the screen (as an automatic scroll to the top) and then set the
    // cursor after the mode selection menu.
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwCursorPosition = {0};
    SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
    dwCursorPosition.Y = SEL_MODE_Y_POS;
    SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);

    // Ask for the mode to be used.
    const char mode_fie[] = " <- Select mode : ";
    const char mode_opt[] = "123";
    char       mode_dat[SINGLE_CHAR_SIZE + NULL_TERMIN_SIZE] = {0};
    size_t     mode_len = strlen(mode_dat);
    input(stdout, NULL, mode_fie, mode_dat, mode_len, SINGLE_CHAR_SIZE,
            SINGLE_CHAR_SIZE, mode_opt);

    // Perform the stuff, clearing the screen every time the selected mode is
    // executed. When finished, it is asked to the user if he wants to start
    // over (if not, the program finishes).
    int again = 0;
    do {
        clear_cmd();
               if (*mode_dat == '1') {
            run_full(TRUE);
        } else if (*mode_dat == '2') {
            run_full(FALSE);
        } else {
            run_single();
        }
        ask_yes_no(stdout, NULL, " <- Start over? [Y/N] : ", &again);
    } while (again == YES);
}

static void prompt_error(const char *top, const char *msg) {
    // Hide the console window.
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Prompt the error message.
    MessageBoxA(NULL, msg, top, MB_OK | MB_ICONERROR);

    // End the program with failure.
    exit(1);
}

// -----------------------------------------------------------------------------

#endif // WIN32
