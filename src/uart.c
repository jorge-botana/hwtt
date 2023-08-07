// -----------------------------------------------------------------------------
// UART_C
//
// - Serial port routines
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

#if       (UART == 1)

// ---------------------- Private preprocessor macros ----------------------- //

#define   MAX_COM_VALUE_NAME_LEN                                           32767
#define   MAX_COM_DATA_LEN                                                   255

#define   MIN_COM_PORT_LEN                                                     1
#define   MAX_COM_PORT_LEN                                                     6

#define   BAUDRATE                                                          9600
#define   PARITY                                                        NOPARITY
#define   DATA_BITS                                                            8
#define   STOP_BITS                                                   ONESTOPBIT

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char coms[] = "UART";

// -------------- Private global data holders initializations --------------- //

static HANDLE h = 0;

// --------------------- Private functions declarations --------------------- //

// ---------------------- Public functions definitions ---------------------- //

int init_coms(void) {
    // Setup the communications.
    write_header(stdout, report, "Serial Communications Setup");

    // Open the registry key that contains the subkey with the available
    // communications ports and print them. As it is a predefined key, it is not
    // necessary to close it afterwards.
    HKEY com = 0;
    ret = RegOpenKeyA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM",
            &com);
    if (ret != ERROR_SUCCESS) {
        output(stdout, report, " -> No COM port list was found!");
        output(stdout, report, "\n");
    } else {
        DWORD dwIndex = 0;
        CHAR lpValueName[MAX_COM_VALUE_NAME_LEN] = {0};
        BYTE lpData[MAX_COM_DATA_LEN] = {0};
        while (ret != ERROR_NO_MORE_ITEMS) {
            DWORD dwcchValueName = MAX_COM_VALUE_NAME_LEN;
            DWORD dwcbData       = MAX_COM_DATA_LEN;
            ret = RegEnumValueA(com, dwIndex, lpValueName, &dwcchValueName,
                    NULL, NULL, lpData, &dwcbData);
            if (ret == ERROR_SUCCESS) {
                if (dwIndex == 0) {
                    output(stdout, report, " -> COM ports were found:");
                    output(stdout, report, "\n");
                    output(stdout, report, "\n");
                }
                char item[DEF_BIG_BUF_SIZE] = {0};
                sprintf(item, " -> %s", lpData);
                output(stdout, report, item);
                output(stdout, report, "\n");
            }
            dwIndex++;
        }
        if (--dwIndex == 0) {
            output(stdout, report, " -> No COM ports were found!");
            output(stdout, report, "\n");
        }
    }
    output(stdout, report, "\n");

    // Request the communications port used to connect to the tested PCBA.
    const char  uart_fie[] = " <- Serial communication port : ";
    static char uart_dat[MAX_COM_PORT_LEN + NULL_TERMIN_SIZE] = {DEF_COM_PORT};
    size_t      uart_len = strlen(uart_dat);
    input(stdout, report, uart_fie, uart_dat, uart_len, MIN_COM_PORT_LEN,
            MAX_COM_PORT_LEN, alphnum);

    // Open the selected communications port.
    output(stdout, report, " -> Checking availability ..... ");
    DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    h = CreateFileA(uart_dat, dwDesiredAccess, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error("COM port not available.");

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");
    output(stdout, report, "\n");

    // Setup the communications port.
    output(stdout, report, " -> Performing UART setup ..... ");
    DCB stDCB = {
        .DCBlength = sizeof(DCB),
        .BaudRate  = BAUDRATE,
        .fBinary   = TRUE,
        .Parity    = PARITY,
        .ByteSize  = DATA_BITS,
        .StopBits  = STOP_BITS
    };
    ret = SetCommState(h, &stDCB);
    if (ret == 0) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error(NULL);
        CloseHandle(h);

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");
    output(stdout, report, "\n");

    // Discard the content that could exist prior to the program's execution in
    // the buffers of the serial port of the computer.
    output(stdout, report, " -> Clearing client buffers ... ");
    ret = PurgeComm(h, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT |
            PURGE_RXCLEAR);
    if (ret == 0) {
        output(stdout, report, error_msg);
        output(stdout, report, "\n");
        print_error(NULL);
        CloseHandle(h);

        return 1;
    }
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");

    // Discard the content that could exist prior to the program's execution in
    // the buffers of the serial port of the PCBA. To do this, an ENTER is sent
    // to it and the received response is ignored (but it must exist and end
    // with _HWTT_TEST_END).
    output(stdout, report, " -> Clearing server buffers ... ");
    const char _hwtt_test_end[] = "_HWTT_TEST_END";
    char buf[sizeof(_hwtt_test_end)] = {0};
    size_t len = sizeof(buf) - NULL_TERMIN_SIZE;
    ret = send_buf("\r", SINGLE_CHAR_SIZE);
    do {
        if (ret != 0) {
            output(stdout, report, error_msg);
            output(stdout, report, "\n");
            print_error(NULL);
            CloseHandle(h);

            return 1;
        }
        char new = 0;
        ret = recv_buf(&new, 1);
        shift_buf(buf, len, new);
    } while (strcmp(buf, _hwtt_test_end) != 0);
    output(stdout, report, ok_msg);
    output(stdout, report, "\n");

    return 0;
}

int shut_coms(void) {
    // End the connection.
    ret = CloseHandle(h);
    if (ret == 0) {
        return 1;
    }

    return 0;
}

int send_buf(const char *buf, size_t len) {
    // Send a buffer of len number of bytes.
    DWORD dwNumberOfBytesWritten = 0;
    ret = WriteFile(h, buf, len, &dwNumberOfBytesWritten, NULL);
    if (ret == FALSE) {
        return 1;
    }

    return 0;
}

int recv_buf(char *buf, size_t len) {
    // Receive a buffer of len number of bytes.
    DWORD lpNumberOfBytesRead = 0;
    ret = ReadFile(h, buf, len, &lpNumberOfBytesRead, NULL);
    if (ret == FALSE) {
        return 1;
    }

    return 0;
}

// --------------------- Private functions definitions ---------------------- //

// -----------------------------------------------------------------------------

#endif // (UART == 1)

#endif // WIN32
