// -----------------------------------------------------------------------------
// PROTOCOL_C
//
// - Communication protocol implementation to perform the tests
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

// -------------------- Private data types declarations --------------------- //

typedef enum test_res {
    TEST_RES_UNKNOWN  = 0,
    TEST_RES_QUESTION = 1,
    TEST_RES_FAIL     = 2,
    TEST_RES_PASS     = 3
} test_res_t;

// --------------- Public global data holders initializations --------------- //

// -------------- Private global data holders initializations --------------- //

static test_res_t result[N_TESTS] = {0};

// --------------------- Private functions declarations --------------------- //

static int tx_req(int num);
static int rx_res(int num);

// ---------------------- Public functions definitions ---------------------- //

int exe_test(int num) {
    // Print an initial header with the number of the test.
    char msg_test[DEF_SMA_BUF_SIZE] = {0};
    sprintf(msg_test, "Test %02i", num);
    write_header(stdout, report, msg_test);

    // If a prompt exists, show it.
    if (  *prompt[num] != 0) {
        input(stdout, report, prompt[num], NULL, 0, 0, 0, "\r");
        output(stdout, report, "\n");
    }

    // If a request exists, send it and receive the response.
    if ( *request[num] != 0) {
        ret = tx_req(num);
        if (ret != 0) {
            return 1;
        }
        ret = rx_res(num);
        if (ret != 0) {
            return 1;
        }
        output(stdout, report, "\n");
    }

    // If a question exists, ask it to the user.
    if (*question[num] != 0) {
        int answer = 0;
        ask_yes_no(stdout, report, question[num], &answer);
        if (answer == YES) {
            result[num] = TEST_RES_PASS;
        } else {
            result[num] = TEST_RES_FAIL;
        }
        output(stdout, report, "\n");
    }

    // If there is neither request nor question, set the result to PASS.
    if (*request[num] == 0 && *question[num] == 0) {
        if (*prompt[num] == 0) {
            output(stdout, report, " -> This test does not exist!");
            output(stdout, report, "\n");
            output(stdout, report, "\n");
        }
        result[num] = TEST_RES_PASS;
    }

    // Display the result of the test.
    dis_res(num);

    return 0;
}

void dis_res(int num) {
    // Display the result of a test. The "all_ok" global flag is set if the
    // first test [0] is successful, but if any test [0, N_TESTS - 1] is
    // unsuccessful, this flag is cleared.
    char buf[DEF_SMA_BUF_SIZE] = {0};
    sprintf(buf, " -> TEST %02i : ", num);
    output(stdout, report, buf);
    if (result[num] == TEST_RES_PASS) {
        output(stdout, report, "PASS");
        if (num == 0) {
            all_ok = TRUE;
        }
    } else {
        output(stdout, report, "FAIL");
        if (num < N_TESTS) {
            all_ok = FALSE;
        }
    }
    output(stdout, report, "\n");
}

// --------------------- Private functions definitions ---------------------- //

static int tx_req(int num) {
    // Print the initial message.
    if (report != NULL) {
        output(stdout, NULL, " -> Sending request ........... ");
        output(NULL, report, " >> ");
    } else {
        output(stdout, NULL, " >> ");
    }

    // Send the request.
    size_t len = strlen(request[num]);
    ret = send_buf(request[num], len);
    if (ret != 0) {
        if (report != NULL) {
            output(stdout, NULL, error_msg);
            output(stdout, NULL, "\n");
        }
        print_error(NULL);

        return 1;
    }

    // Print the request.
    if (report != NULL) {
        output(stdout, NULL, ok_msg);
        output(NULL, report, request[num]);
    } else {
        output(stdout, NULL, request[num]);
    }
    output(stdout, report, "\n");

    return 0;
}

static int rx_res(int num) {
    // Print the initial message.
    if (report != NULL) {
        output(stdout, NULL, " -> Receiving response ........ ");
        output(NULL, report, " << ");
    } else {
        output(stdout, NULL, " << ");
    }

    // Receive characters one by one until the sequence _XX_HWTT_TEST_END is
    // detected, where XX is the number of the current test (to do this, the
    // last received characters are stored in a shift buffer and every time is
    // searched in it the break condition end string).
    const char _hwtt_test_end[] =    "_HWTT_TEST_END";
    char    _xx_hwtt_test_end[] = "_XX_HWTT_TEST_END";
    sprintf(_xx_hwtt_test_end, "_%02u%s", num, _hwtt_test_end);
    char buf[SINGLE_CHAR_SIZE + sizeof(_xx_hwtt_test_end)] = {0};
    size_t len = sizeof(buf) - NULL_TERMIN_SIZE;
    do {
        // Receive the next character.
        char new = 0;
        ret = recv_buf(&new, SINGLE_CHAR_SIZE);
        if (ret != 0) {
            if (report != NULL) {
                output(stdout, NULL, error_msg);
                output(stdout, NULL, "\n");
            }
            print_error(NULL);

            return 1;
        }

        // Insert the received character in the shift buffer.
        shift_buf(buf, len, new);

        // Write the received character to the report (full report modes) or to
        // the screen (single test mode).
        char str[] = {new, 0};
        if (report != NULL) {
            output(NULL, report, str);
        } else {
            output(stdout, NULL, str);
        }
    } while (strcmp(&buf[1], _xx_hwtt_test_end) != 0);
    if (report != NULL) {
        output(stdout, NULL, ok_msg);
    }
    output(stdout, report, "\n");

    // Determine the result of the test by scanning the character before the
    // _XX_HWTT_TEST_END sequence. This result can be modified later in case
    // there is a question after the response.
           if (*buf == 'P') {
        result[num] = TEST_RES_PASS;
    } else if (*buf == 'F') {
        result[num] = TEST_RES_FAIL;
    } else if (*buf == 'Q') {
        result[num] = TEST_RES_QUESTION;
    } else                  {
        result[num] = TEST_RES_UNKNOWN;
    }

    return 0;
}

// -----------------------------------------------------------------------------

#endif // WIN32
