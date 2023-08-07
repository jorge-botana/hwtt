// -----------------------------------------------------------------------------
// IO_C
//
// - Input / output functions and allowed charsets for input
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

#include  <conio.h>
#include  "public.h"

// ---------------------- Private preprocessor macros ----------------------- //

#define   LOWERCASE                                 "abcdefghijklmnopqrstuvwxyz"
#define   UPPERCASE                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define   NUMBERS                                                   "0123456789"
#define   SPACE                                                              "_"
#define   DOT                                                                "."

#define   N_HEADER_SPACES                                                      4
#define   N_HEADER_HALFS                                                       2

#define   CTRL_C_CHAR                                                       0x03
#define   ALT_F4_CHAR_ONE                                                   0x00
#define   ALT_F4_CHAR_TWO                                                   0x6B

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char lowercase[] = LOWERCASE                                             ;
const char uppercase[] =               UPPERCASE                               ;
const char   numbers[] =                             NUMBERS                   ;
const char     space[] =                                         SPACE         ;
const char       dot[] =                                                   DOT ;
const char   alphnum[] = LOWERCASE     UPPERCASE     NUMBERS                   ;
const char   num_dot[] =                             NUMBERS               DOT ;
const char       all[] = LOWERCASE     UPPERCASE     NUMBERS     SPACE     DOT ;

// -------------- Private global data holders initializations --------------- //

// --------------------- Private functions declarations --------------------- //

// ---------------------- Public functions definitions ---------------------- //

void write_header(FILE *cmd, FILE *txt, const char *title) {
    // Skip an initial empty line.
    output(stdout, report, "\n");

    // Determine the number of dashes to be printed.
    int len = strlen(title);
    int n_dashes = 0;
    if (len > 0) {
        n_dashes = CMD_WIDTH - N_HEADER_SPACES - len ;
    } else {
        n_dashes = CMD_WIDTH - N_HEADER_SPACES / N_HEADER_HALFS;
    }

    // Print an initial space.
    output(cmd, txt, " ");

    // Print the left dashes.
    for (int i = 0; i < n_dashes / N_HEADER_HALFS; i++) {
        output(cmd, txt, "-");
    }

    // Print the header title (if exists).
    if (len > 0) {
        // Print a space between the left dashes and the header title.
        output(cmd, txt, " ");

        // Print the header title.
        output(cmd, txt, title);

        // Print a space between the header title and the right dashes.
        output(cmd, txt, " ");
    }

    // Print the right dashes.
    for (int i = 0; i < n_dashes / N_HEADER_HALFS; i++) {
        output(cmd, txt, "-");
    }

    // Print an extra dash if the number of dashes is odd.
    if (n_dashes % N_HEADER_HALFS != 0) {
        output(cmd, txt, "-");
    }

    // End the header.
    output(cmd, txt, "\n");

    // Skip a final empty line.
    output(cmd, txt, "\n");
}

void show_version(FILE *cmd, FILE *txt) {
    // Print the HWTT version.
    char hwtt_version[DEF_SMA_BUF_SIZE] = {0};
    sprintf(hwtt_version, " -> HWTT core  : %-*s", MAX_BUILD_STR_SIZE,
            HWTT_VERSION);
    output(cmd, txt, hwtt_version);

    // Print the used communication interface.
    char        iface[DEF_SMA_BUF_SIZE] = {0};
    sprintf(iface,        " -> Coms. : %s", coms);
    output(cmd, txt, iface);
    output(cmd, txt, "\n");

    // Print the version of the PCBA.
    char pcba_version[DEF_SMA_BUF_SIZE] = {0};
    sprintf(pcba_version, " -> PCBA/HW/SW : %-*s", MAX_BUILD_STR_SIZE,
            PCBA_VERSION);
    output(cmd, txt, pcba_version);

    // Print the number of tests.
    char      n_tests[DEF_SMA_BUF_SIZE] = {0};
    sprintf(n_tests,      " -> Tests : %i", N_TESTS);
    output(cmd, txt, n_tests);
    output(cmd, txt, "\n");
}

void ask_yes_no(FILE *cmd, FILE *txt, const char *que, int *ans) {
    // Ask to the user a question that must be answered with Yes or No ([Y/N]).
    const char opt[] = "ynYN";
    char dat[SINGLE_CHAR_SIZE + NULL_TERMIN_SIZE] = {0};
    input(cmd, txt, que, dat, ORIGIN, SINGLE_CHAR_SIZE, SINGLE_CHAR_SIZE, opt);
    if (*dat == 'y' || *dat == 'Y') {
        *ans = YES;
    } else {
        *ans = NO;
    }
}

void input(FILE *cmd, FILE *txt, const char *field, char *data,
        size_t cur_len, size_t min_len, size_t max_len, const char *charset) {
    // Print the field to be completed with the data to be input.
    output(cmd, txt, field);

    // Print the default data, if any.
    if (max_len > 0) {
        output(cmd, NULL, data);
    }

    // Clear the input buffer to avoid from being processed keystrokes that were
    // made before calling the function.
    HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hConsoleInput);

    // Read characters entered on the keyboard until an ENTER keystroke is
    // detected or until the program is killed with Ctrl + C or Alt + F4.
    while (1) {
        // Get a character from stdin (keyboard).
        unsigned int x = _getch();

        // If the inputted character was a space, convert it in an underscore.
        if (x == ' ') {
            x = '_';
        }

        // Detect an introduced character and, if it is allowed by the charset,
        // print it and store it in a buffer.
        char *ptr = strchr(charset, x);
        if (x != 0 && ptr != NULL && cur_len < max_len) {
            char character[] = {x, 0};
            output(cmd, NULL, character);
            data[cur_len] = x;
            cur_len++;
        }

        // Detect a BACKSPACE keystroke and handle it, deleting the last
        // inputted character from the buffer and from the screen, and moving
        // the cursor one position back, going back one line if necessary.
        if (x == '\b' && cur_len > 0) {
            HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
            GetConsoleScreenBufferInfo(hConsoleOutput, &csbiInfo);
            COORD dwCursorPosition = {0};
            if (csbiInfo.dwCursorPosition.X == 0) {
                dwCursorPosition.X = csbiInfo.dwMaximumWindowSize.X - 1;
                dwCursorPosition.Y = csbiInfo.dwCursorPosition.Y    - 1;
            } else {
                dwCursorPosition.X = csbiInfo.dwCursorPosition.X    - 1;
                dwCursorPosition.Y = csbiInfo.dwCursorPosition.Y    - 0;
            }
            SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
            output(cmd, NULL, " ");
            SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
            cur_len--;
            data[cur_len] = 0;
        }

        // Detect an ENTER keystroke and handle it, returning from function if
        // at least one character was introduced or if no input data field
        // exists.
        if (x == '\r' && (cur_len >= min_len || max_len == 0)) {
            if (max_len > 0) {
                output(NULL, txt, data);
                data[cur_len] = 0;
            }
            output(cmd, txt, "\n");
            break;
        }

        // Detect CTRL + C keystrokes to kill the program.
        if (x == CTRL_C_CHAR) {
            exit(1);
        }

        // Detect ALT + F4 keystrokes to kill the program.
        if (x == ALT_F4_CHAR_ONE) {
            unsigned int x = _getch();
            if (x == ALT_F4_CHAR_TWO) {
                exit(1);
            }
        }
    }
}

void output(FILE *cmd, FILE *txt, const char *msg) {
    // Print a message to the screen.
    if (cmd != NULL) {
        fprintf(cmd, "%s", msg);
    }

    // Print a message to a text file.
    if (txt != NULL) {
        fprintf(txt, "%s", msg);
    }
}

// --------------------- Private functions definitions ---------------------- //

// -----------------------------------------------------------------------------

#endif // WIN32
