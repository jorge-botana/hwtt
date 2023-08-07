// -----------------------------------------------------------------------------
// MISC_C
//
// - General purpose miscellaneous functions and global variables
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

#define   EN_US                                                           0x0409

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char    ok_msg[]= "OK";
const char error_msg[]= "ERROR";

FILE *report = NULL;
int all_ok = TRUE;
long ret = 0;

// -------------- Private global data holders initializations --------------- //

// --------------------- Private functions declarations --------------------- //

// ---------------------- Public functions definitions ---------------------- //

void shift_buf(char *buf, size_t len, char new) {
    // Shift one position to the left every character in a buffer, inserting a
    // new character in the rightmost position and discarding the character in
    // the leftmost position.
    for (int i = 1; i < len; i++) {
        buf[i - 1] = buf[i];
    }
    buf[len - 1] = new;
}

void print_error(const char *msg) {
    // Print a custom error message or, if NULL is passed as argument, a Windows
    // error message related with communications is gotten using GetLastError()
    // for (U)ART or WSAGetLastError() for (E)thernet.
    output(stdout, NULL, "\n");
    output(stdout, NULL, " -> ");
    if (msg != NULL) {
        output(stdout, NULL, msg);
        output(stdout, NULL, "\n");
    } else {
        DWORD dwMessageId = 0;
        if (*coms == 'U') {
            dwMessageId = GetLastError();
        }
        if (*coms == 'E') {
            dwMessageId = WSAGetLastError();
        }
        CHAR lpBuffer[DEF_BIG_BUF_SIZE] = {0};
        ret = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwMessageId,
                EN_US, lpBuffer, DEF_BIG_BUF_SIZE, NULL);
        if (ret != 0) {
            output(stdout, NULL, lpBuffer);
        } else {
            const char error_locale[] =
                    "Could not be retrieved the message string matching to the "
                    "following error code, probably because the english localiz"
                    "ation (United States) is not installed in this system.";
            output(stdout, NULL, error_locale);
            output(stdout, NULL, "\n");
            output(stdout, NULL, "\n");
            char error_code[DEF_SMA_BUF_SIZE] = {0};
            sprintf(error_code, " -> Windows error code = %lu (0x%lX)",
                    dwMessageId, dwMessageId);
            output(stdout, NULL, error_code);
            output(stdout, NULL, "\n");
        }
    }
    output(stdout, NULL, "\n");
}

// --------------------- Private functions definitions ---------------------- //

// -----------------------------------------------------------------------------

#endif // WIN32
