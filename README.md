# HWTT

The HWTT (Hardware Test Tool) is a command line console software for Microsoft
Windows that can be used to perform custom hardware tests on manufactured PCBAs
(Printed Circuit Board Assembly), allowing two communication interfaces: serial
port or Ethernet.

## Introduction

Every manufactured PCBA must pass some quality control checks to make sure that
it works correctly without failures. In order to do that, many tests are
performed checking different hardware parts of the PCBA, and here there is a
computer program that can be used to easily implement and execute these tests,
taking into account that in the PCBA to be tested must run a firmware that
replies to all the tests requests following a specific protocol.

Apart from executing tests, this program offers a way to keep a traceability of
the PCBAs, generating a full TXT report with the debug information and the
results of all the tests, along with other data like the build information of
this program, the communications identifiers, the user who has tested the PCBA,
his company, the batch and serial number of the PCBA, the time and date and the
whole result of the tests. Furthermore, the relevant data is added to a common
traceability CSV.

## Features

This program has the following features:

- **Free** : allows to be developed, compiled and run with no cost.

- **Simple** : generates a single TXT report per PCBA tested and a global
  traceability CSV for all of them.

- **Flexible** : offers an easy way to adapt the program among different PCBA
  versions, making minimum changes.

- **Robust** : has a good error handling and the data input is very restricted
  to avoid errors.

- **Efficient** : can autocomplete some fields with predefined data. 

- **Standalone** : is portable, so no external dependencies are required
  (neither to compile nor to execute).

- **Fast** : works fine in low end computers.

- **Compatible** : runs in Microsoft Windows 7 (x64) and later.

- **Small** : compiles with less than 1 MiB of footprint (in the release build
  with optimizations).

These features make the program an easy solution to test PCBAs.

## Tests

The tests are defined the following way, being optional the implementation of
every step:

- **Prompt** : a message can be prompted asking the user to do something and
  then to press the ``[ENTER]`` key.

- **Request** : this program sends the test's request and the PCBA replies with
  the test's response.

- **Question** : at the end, a ``[Y/N]`` question can be done to the user to
  determine if the test was successful or not.

The following table describes how it is determined the result of a test:

    Prompt   | Request | Question || Result (only PASS or FAIL)
    ----------------------------------------------------------------------------
    No       | No      | No       || Inexistent test message is displayed, PASS
    Yes      | No      | No       || After the prompt, PASS
    X        | Yes     | No       || Determined by the response to the request
    X        | X       | Yes      || PASS if Yes, FAIL if No (question's answer)

Here is an example of a test where a prompt and a question are implemented, but
there is neither request nor response (so there is no communication for this
test): 

    <- Push the button 4. [ENTER] : 

    <- Did the LED 5 light up? [Y/N] :

This test is successful if the response of the user is affirmative.

## Protocol

The communication protocol established with the tested PCBAs has the following
structure:

    Request  >> T_<XX><TX><CR>
    Response << <RX><EC>_<XX>_HWTT_TEST_END

Where, in the request:

- ``<XX>`` is the number of the executed test, always with two digits (from 00
  to 99).

- ``<TX>`` is an optional payload in the request's message, for example a
  parameter.

- ``<CR>`` is a carriage return ``\r``, as an ENTER keystroke that marks the end
  of the request.

And, in the response:

- ``<RX>`` is an optional payload in the response's message, for example a
  parameter.

- ``<EC>`` is a single character that represents the error code of the test.
  ``P`` if PASS, ``F`` if FAIL, or ``Q`` if it is the case that a question to
  the user is required to determine if the test was successful or not (in this
  case, if the question is not implemented in the tests file, the result is
  interpreted as unsuccessful). If the received character is different from the
  previous ones, the test is also unsuccessful.

- ``<XX>`` is again the number of the executed test, always with two digits
  (from 00 to 99), and it must match with the number of the test sent in the
  request.

Here is an example of a test:

    Request  >> T_09 R_SHUNT=0.1_OHM\r
    Response << I_SHUNT=1.25_A P_09_HWTT_TEST_END

When the ``X_09_HWTT_TEST_END`` sequence is detected, the program interprets
that the response has finished.

It is important to take into account that, if the program sends to the PCBA
anything finished in ``\r`` that does not match to any test (for example, a
simple ``\r``), the PCBA must reply with anything finished with the sequence
``_XX_HWTT_TEST_END``.

## Operation

This program offers three operation modes:

- **[1] Production** : a full TXT report is generated with the results of all
  the tests of a PCBA, from the test #00 to a maximum of #99. Then it is saved
  in a folder called ``reports`` next to the executable, with a filename with
  the following structure:

  ``<BN>_<SN>_<WR>.txt``

  Where ``<BN>`` is the batch number of the PCBA, ``<SN>`` the serial number and
  ``<WR>`` the whole result of the tests. It is said, ``OK`` if all the tests
  were successful, or ``ERROR`` if at least one test was unsuccessful. For
  example:

  ``01234_56789_OK.txt``

  Additionally, the traceability CSV is updated (located next to the
  executable), which has a filename with the following structure:

  ``<HW>.csv``

  Where ``<HW>`` is the PCBA version defined in the configuration file. For
  example:

  ``MY_BOARD_REV_1_0_FW_1_00.csv``

- **[2] Testing** : similar as the previous operation mode, but the string
  ``_test_`` is appended at the beginning of the TXT report filename and the
  traceability CSV is not updated. For example:

  ``_test_01234_56789_ERROR.txt``

- **[3] Single** : neither TXT report nor traceability CSV are used (and no
  traceability information is requested). There are only executed the desired
  tests and the debug information is displayed on screen (this is not the case
  of the previous modes, where this information is saved in the TXT reports but
  not displayed on screen during the tests executions).

In case a TXT report is being generated, all the relevant information displayed
on screen is being saved, and also the tests payloads that are not being
displayed on screen.

## Flow

The flow of the program is the following:

- An introduction screen is displayed showing an ASCII art logo, the build
  version information, the option selection menu, the copyright information an
  the open source license (hidden).

- After the operation mode is selected, the screen is cleared and the build
  version information is displayed again.
 
 - Then there are created the necessary folder and files in the selected mode
  (reports folder, temporal file, traceability CSV).

- With the files created, it is asked for some traceability data: user, company,
  batch number and serial number. It is not the case in the single test mode,
  where it is not necessary as neither TXT report nor traceability CSV are being
  used.

- Later it is asked to open a serial port or to connect to an IPv4 address and
  port, initializing the communication with the PCBA.

- The tests are performed or, in case of the single test mode, it is asked for
  the test to be executed.

- When the tests end, the TXT report is saved with the results and a timestamp,
  and the traceability CSV is updated (if exists). In the case of the single
  test mode, it is asked if the user is about to continue doing tests.

When the chosen mode ends, it is asked to the user if he wants to start over the
same mode, and the beginning data fields are remembered in order to be able to
fill them faster.

## Configuration

There are two source files that are meant to be modified for every PCBA version:

- ``config.h`` : specifies the build information, the communications interface
  (serial port or Ethernet), the PCBA version to be tested, the number of tests
  and the predefined data fields.

- ``tests.c`` : defines the test requests commands sent to the tested PCBA, and
  also the prior prompts and posterior questions. If a field does not exist, it
  must me defined as an empty string ``""``.

After properly modifying these files, a custom build for a specific PCBA version
can be compiled.

## Build

The following steps can be followed to adapt the workspace and build this
program:

- Go to the following link to get a pre-built GCC toolchain of MinGW-w64:

  https://www.mingw-w64.org/downloads/#mingw-builds

- Download a release, for example, this one has been used:

  ``x86_64-13.1.0-release-posix-seh-ucrt-rt_v11-rev1.7z``

- Unzip it in the hard disk root and add the following path to the PATH
  environment variable:

  ``C:\mingw64\bin``

- Get and install the Eclipse IDE (used version: 2023-06):

  ``https://www.eclipse.org/downloads/``

  Select "Eclipse IDE for C/C++ Developers" during the installation, with the
  default options. After it is installed, open it and close the "Welcome"
  screen.

- Create a project:

  ``File -> New -> C/C++ Project -> All -> C Managed Build -> Project type: ->
  Executable -> Hello World ANSI C Project -> Toolchains: -> MinGW GCC``

  Click "Finish" after putting the name "hwtt" to the project, and then copy all
  the source files to the "src" folder, and delete the .c file automatically
  generated on project creation.

- Add the command to build the resources object:

  ``Project -> Properties -> C/C++ Build -> Settings -> Build Steps -> Pre-build
  steps -> Command:``

  ``windres ..\res\resources.rc ..\res\resources.o``

- Add the command to link the resources object:

  ``Project -> Properties -> C/C++ Build -> Settings -> Tool Settings -> MinGW C
  Linker -> Command:``

  ``gcc ..\res\resources.o``

- Ensure that the GCC command is the following:

  ``Project -> Properties -> C/C++ Build -> Settings -> Tool Settings -> (GCC
  Assembler / GCC C Compiler / MinGW C Linker) -> Command:``

  ``${COMMAND} ${FLAGS} ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} ${INPUTS}``

  Note that in the 2022-03 version of Eclipse CDT it is like this by default,
  but since the 2022-06 version an ``${EXTRA_FLAGS}`` token is added that causes
  some arguments to be passed twice in the linker, leading to problems, so that
  it has to be removed.

- Add the Winsock 2 library (necessary for the Ethernet build) in:

  ``Project -> Properties -> C/C++ Build -> Settings -> Tool Settings -> MinGW C
  Linker -> Libraries -> Add...``

  ``ws2_32``

At this point, it is possible to build the program (clicking to the icon with a
hammer). But, in order to continue developing this program, it is advisable to
perform some additional steps:

- Enable the Windows console debug (to avoid problems):

  ``Run -> Debug Configurations... -> C/C++ Application -> hwtt.exe -> Debugger
  -> Main -> Use external console for inferior (open a new console window for
  input/output)``

  To be able to do this, the program must have been debugged (as a "Local C/C++
  Application") at least once (clicking to the icon with a bug).

- Use UTF-8 encoding (not explicitly necessary, as only 7-bit ASCII is being
  used):

  ``Window -> Preferences -> General -> Workspace -> Text file encoding -> Other
  -> UTF-8``

- Use Unix delimiter (LF without CR):

  ``Window -> Preferences -> General -> Workspace -> New text file line
  delimiter -> Unix``

- Add a vertical line at the column 80:

  ``Window -> Preferences -> General -> Editors -> Text Editors -> Show print
  margin``

- Replace tabs with spaces (4) for text files without source code:

  ``Window -> Preferences -> General -> Editors -> Text Editors -> Insert spaces
  for tabs``

- Replace tabs with spaces (4) for C/C++ source code files:

  ``Window -> Preferences -> C/C++ -> Code Style -> Formatter -> New``

- Enter the new name "HWTT" for a new profile, initializing it with the settings
  of the built-in profile K&R:
  
  ``Indentation -> General settings -> Tab policy: -> Spaces only``

The Eclipse project files are included in this repository, so the project can be
directly imported, skipping some steps.

## Notes

Some extra information must be taken into account:

- All the files to be modified must be closed before the program's execution.

- During a TXT report generation, a temporal file is used next to the executable
  (``incomplete.hwtt``).

- After a TXT report is generated, if an old one exists with the same filename,
  it is replaced.

- In the input fields, the minimum and maximum lengths are determined by this
  program, and also the allowed charsets. For example, if it is asked for an
  IPv4 address, it cannot be inserted a letter (the program will ignore it and
  it is not going to be printed on screen).

- The error handling of this program has some predefined error messages, but
  other ones are taken from the system, and in this case the English (United
  States) localization must be installed in order to be able to see the error
  messages.

- In case of error, or at the end of the execution of an operation mode, all the
  connections and files are silently closed.

- It is forbidden to put the program on full screen with an ALT + ENTER
  keystroke. In that case, the program will close with an error prompt.

- The program can be killed at any moment with CTRL + C or ALT + F4.

Anyway, it is highly recommended to study the source code to understand in
detail how this program works. This program has been coded in C language and
using the WIN32 API.

## Requirements

As this is a command line console program, its requirements are very low:

- **Operative System** : Microsoft Windows 7 (x64) or higher

- **Graphics** : screen resolution of 1024x768 or higher

- **Communications** : serial port and/or Ethernet interface

Also it has minimal memory usage during its execution and it requires some
storage to save the TXT reports and the traceability CSV.

## About

This software was created by Jorge Botana Mtz. de Ibarreta and is distributed
under the terms of the MIT License.
