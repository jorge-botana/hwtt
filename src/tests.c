// -----------------------------------------------------------------------------
// TESTS_C
//
// - Tests definitions (prompts, requests and questions)
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

#include  <assert.h>
#include  "public.h"

// ---------------------- Private preprocessor macros ----------------------- //

#define   N_PROMPT_FIELDS                   sizeof(  prompt) / sizeof(  *prompt)
#define   N_REQUEST_FIELDS                  sizeof( request) / sizeof( *request)
#define   N_QUESTION_FIELDS                 sizeof(question) / sizeof(*question)

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char   *prompt[] = {
    /* 00 */    "",
    /* 01 */    " <- Activate the switch. [ENTER] : ",
    /* 02 */    "",
    /* 03 */    " <- Unplug the USB wire. [ENTER] : ",
    /* 04 */    " <- This test does nothing. [ENTER] : ",
    /* 05 */    ""
};

const char  *request[] = {
    /* 00 */    "T_00_lorem_ipsum\r",
    /* 01 */    "T_01\r",
    /* 02 */    "",
    /* 03 */    "T_03_jaja_xd\r",
    /* 04 */    "",
    /* 05 */    ""
};

const char *question[] = {
    /* 00 */    " <- Did the LED 3 light up? [Y/N] : ",
    /* 01 */    "",
    /* 02 */    "",
    /* 03 */    " <- Is the current between 0.25 and 0.50 amps? [Y/N] : ",
    /* 04 */    "",
    /* 05 */    " <- Is the voltage higher than 1,75 volts? [Y/N] : "
};

// -------------- Private global data holders initializations --------------- //

// --------------------- Private functions declarations --------------------- //

static void __attribute__ ((unused)) cfg_check(void);

// ---------------------- Public functions definitions ---------------------- //

// --------------------- Private functions definitions ---------------------- //

static void __attribute__ ((unused)) cfg_check(void) {
    static_assert(sizeof(HWTT_VERSION) <= MAX_BUILD_STR_SIZE,
            "Too long build HWTT core string!");
#if       !((UART == 1) ^ (ETH == 1))
#error    "The communication data link layer (UART/ETH) must be defined!"
#endif // !((UART == 1) ^ (ETH == 1))
    static_assert(sizeof(PCBA_VERSION) <= MAX_BUILD_STR_SIZE,
            "Too long build PCBA/HW/SW string!");
#if       N_TESTS < 1
#error    "The number of tests must be positive!"
#endif // N_TESTS > 1
#if       N_TESTS > 100
#error    "The number of tests cannot exceed 100!"
#endif // N_TESTS > 100
    static_assert(N_PROMPT_FIELDS   >= N_TESTS,
            "Misconfiguration in prompts array!");
    static_assert(N_REQUEST_FIELDS  >= N_TESTS,
            "Misconfiguration in requests array!");
    static_assert(N_QUESTION_FIELDS >= N_TESTS,
            "Misconfiguration in questions array!");
}

// -----------------------------------------------------------------------------

#endif // WIN32
