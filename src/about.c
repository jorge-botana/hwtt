// -----------------------------------------------------------------------------
// ABOUT_C
//
// - Strings related to the initialization screen
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

// ---------------------- Private preprocessor macros ----------------------- //

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

const char      logo[] =
        "               |   |   |   |   |   |   |   |               \n"
        "              _|___|___|___|___|___|___|___|_              \n"
        "          ___|                               |___          \n"
        "             |    _       _     _       _    |             \n"
        "          ___|   | |     | |   | |     | |   |___          \n"
        "             |   | |_____| |   | |  _  | |   |             \n"
        "          ___|   |  _____  |   | | | | | |   |___          \n"
        "             |   | |     | |   | |_| |_| |   |             \n"
        "          ___|   |_|     |_|   |_________|   |___          \n"
        "             |                               |             \n"
        "          ___|    _________     _________    |___          \n"
        "             |   |___   ___|   |___   ___|   |             \n"
        "          ___|       | |           | |       |___          \n"
        "             |       | |           | |       |             \n"
        "          ___|       | |           | |       |___          \n"
        "             |       |_|           |_|       |             \n"
        "          ___|                               |___          \n"
        "             |_______________________________|             \n"
        "               |   |   |   |   |   |   |   |               \n"
        "               |   |   |   |   |   |   |   |               \n"
;

const char     modes[] =
        " -> [1] Production : full report and CSV update            \n"
        " -> [2] Testing    : full report without CSV update        \n"
        " -> [3] Single     : single tests and debug info on screen \n"
;

const char copyright[] =
        " ----------------------------------------------------------\n"
        " |    Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta    |\n"
        " |                                                        |\n"
        " |     Licensed under the MIT License (scroll to see)     |\n"
        " ----------------------------------------------------------\n"
;

const char   license[] =
"\
Permission is hereby granted, free of charge, to any person obtaining a copy of\
 this software and associated documentation files (the \"Software\"), to deal i\
n the Software without restriction, including without limitation the rights to \
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies o\
f the Software, and to permit persons to whom the Software is furnished to do s\
o, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all \
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM\
PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS \
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR \
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER \
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECT\
ION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\
";

// -------------- Private global data holders initializations --------------- //

// --------------------- Private functions declarations --------------------- //

// ---------------------- Public functions definitions ---------------------- //

// --------------------- Private functions definitions ---------------------- //

// -----------------------------------------------------------------------------

#endif // WIN32
