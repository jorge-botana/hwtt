// -----------------------------------------------------------------------------
// PUBLIC_H
//
// - Shared elements; in functions, the return values mean the following:
//
//   0 -> Success
//   1 -> Failure
//
// -----------------------------------------------------------------------------
// Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta
//
// This source code is distributed under the terms of the MIT License. For more
// information, please see the LICENSE.txt file or refer to the following link:
//
//                                           https://opensource.org/license/mit/
// -----------------------------------------------------------------------------

#ifndef   PUBLIC_H
#define   PUBLIC_H

// ------------------------ Public headers includes ------------------------- //

#include  <stdio.h>
#include  <windows.h>
#include  "config.h"

// ----------------------- Public preprocessor macros ----------------------- //

// -----------------------------------------------------------------------------
// Yes/No questions answers codes
// -----------------------------------------------------------------------------
#define   YES                                                                  1
#define   NO                                                                   0

// -----------------------------------------------------------------------------
// Default buffer sizes
// -----------------------------------------------------------------------------
#define   DEF_SMA_BUF_SIZE                                                   128
#define   DEF_MED_BUF_SIZE                                                   256
#define   DEF_BIG_BUF_SIZE                                                   512

// -----------------------------------------------------------------------------
// Miscellaneous definitions
// -----------------------------------------------------------------------------
#define   BITS_IN_ONE_BYTE                                                     8
#define   CMD_WIDTH                                                           60
#define   ORIGIN                                                               0
#define   SINGLE_CHAR_SIZE                                                     1
#define   NULL_TERMIN_SIZE                                                     1
#define   MAX_BUILD_STR_SIZE                                                  26

// --------------------- Public data types declarations --------------------- //

// ---------------- Public global data holders declarations ----------------- //

// -----------------------------------------------------------------------------
// Tests (prompts, requests and questions)
// -----------------------------------------------------------------------------
extern const char   *prompt[];
extern const char  *request[];
extern const char *question[];

// -----------------------------------------------------------------------------
// About information
// -----------------------------------------------------------------------------
extern const char      logo[];
extern const char     modes[];
extern const char copyright[];
extern const char   license[];

// -----------------------------------------------------------------------------
// Communication mode
// -----------------------------------------------------------------------------
extern const char      coms[];

// -----------------------------------------------------------------------------
// OK and ERROR messages
// -----------------------------------------------------------------------------
extern const char    ok_msg[];
extern const char error_msg[];

// -----------------------------------------------------------------------------
// Text input charsets
// -----------------------------------------------------------------------------
extern const char lowercase[];
extern const char uppercase[];
extern const char   numbers[];
extern const char     space[];
extern const char       dot[];
extern const char   alphnum[];
extern const char   num_dot[];
extern const char       all[];

// -----------------------------------------------------------------------------
// Report file handle
// -----------------------------------------------------------------------------
extern FILE *report;

// -----------------------------------------------------------------------------
// Flag used to determine, in a full report mode, if all the tests were
// successful (TRUE) or if at least one of them failed (FALSE)
// -----------------------------------------------------------------------------
extern int all_ok;

// -----------------------------------------------------------------------------
// General purpose return error code
// -----------------------------------------------------------------------------
extern long ret;

// --------------------- Public functions declarations ---------------------- //

// -----------------------------------------------------------------------------
// Write the header of a section, with its name centred with dashes at left and
// right.
// -----------------------------------------------------------------------------
void write_header(
        FILE *cmd,           // Handle to stdout or NULL
        FILE *txt,           // Handle to report or NULL
        const char *title    // Header title
);

// -----------------------------------------------------------------------------
// Show the build information of this software.
// -----------------------------------------------------------------------------
void show_version(
        FILE *cmd,           // Handle to stdout or NULL
        FILE *txt            // Handle to report or NULL
);

// -----------------------------------------------------------------------------
// Ask the user a question that must be answered with Yes or No ([Y/N]).
// -----------------------------------------------------------------------------
void ask_yes_no(
        FILE *cmd,           // Handle to stdout or NULL
        FILE *txt,           // Handle to report or NULL
        const char *que,     // Question to be answered
        int *ans             // Answer: 1 if Yes, 0 if No
);

// -----------------------------------------------------------------------------
// Ask the user to fill a field with data.
// -----------------------------------------------------------------------------
void input(
        FILE *cmd,           // Handle to stdout or NULL
        FILE *txt,           // Handle to report or NULL
        const char *field,   // Input field (e.g. " <- Insert your age : ")
        char *data,          // Input data, can be initialized (e.g. "18")
        size_t cur_len,      // Length of the default data (e.g. 2), 0 if none
        size_t min_len,      // Minimum length of the filled data (e.g. 1)
        size_t max_len,      // Maximum length of the filled data (e.g. 3)
        const char *charset  // Allowed charset (e.g. "0123456789")
);

// -----------------------------------------------------------------------------
// Print a string.
// -----------------------------------------------------------------------------
void output(
        FILE *cmd,           // Handle to stdout or NULL
        FILE *txt,           // Handle to report or NULL
        const char *msg      // String to be printed
);

// -----------------------------------------------------------------------------
// Perform all the tests, with all the debug information saved in a TXT report
// file, and determining with the function's argument if the traceability CSV is
// updated (TRUE) or not (FALSE).
// -----------------------------------------------------------------------------
void run_full(
        int prod             // Production/testing switch
);

// -----------------------------------------------------------------------------
// Perform a single test, with all the debug information displayed on screen.
// -----------------------------------------------------------------------------
void run_single(void);

// -----------------------------------------------------------------------------
// Start the communications with the PCBA via serial port or Ethernet.
// -----------------------------------------------------------------------------
int init_coms(void);

// -----------------------------------------------------------------------------
// Stop the communications with the PCBA via serial port or Ethernet.
// -----------------------------------------------------------------------------
int shut_coms(void);

// -----------------------------------------------------------------------------
// Send a buffer to the PCBA via serial port or Ethernet.
// -----------------------------------------------------------------------------
int send_buf(
        const char *buf,     // Buffer to be sent
        size_t len           // Length of the buffer in bytes
);

// -----------------------------------------------------------------------------
// Receive a buffer from the PCBA via serial port or Ethernet.
// -----------------------------------------------------------------------------
int recv_buf(
        char *buf,           // Buffer to store the received bytes
        size_t len           // Number of bytes to receive
);

// -----------------------------------------------------------------------------
// Execute a test.
//
// Prompt   | Request  | Question || Result (only PASS or FAIL)
// -----------------------------------------------------------------------------
// No       | No       | No       || Inexistent test message is displayed, PASS
// Yes      | No       | No       || After the prompt, PASS
// X        | Yes      | No       || Determined by the response to the request
// X        | X        | Yes      || PASS if Yes, FAIL if No (question's answer)
// -----------------------------------------------------------------------------
int exe_test(
        int num              // Number of the test
);

// -----------------------------------------------------------------------------
// Display the result of a test.
// -----------------------------------------------------------------------------
void dis_res(
        int num              // Number of the test
);

// -----------------------------------------------------------------------------
// Shift one position to the left every character in a buffer, inserting a new
// character in the rightmost position and discarding the character in the
// leftmost position.
// -----------------------------------------------------------------------------
void shift_buf(
        char *buf,           // Buffer to be shifted
        size_t len,          // Length of the buffer
        char new             // New character
);

// -----------------------------------------------------------------------------
// Print a custom error message or, if NULL is passed as argument, a Windows
// error message related with communications is gotten using GetLastError() for
// (U)ART or WSAGetLastError() for (E)thernet.
// -----------------------------------------------------------------------------
void print_error(
        const char *msg      // Custom error message
);

// -----------------------------------------------------------------------------

#endif // PUBLIC_H
