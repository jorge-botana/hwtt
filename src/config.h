// -----------------------------------------------------------------------------
// CONFIG_H
//
// - Configuration file; in the communications protocols defines, the assigned
//   values mean the following:
//
//   0 -> Disabled
//   1 -> Enabled
//
// -----------------------------------------------------------------------------
// Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta
//
// This source code is distributed under the terms of the MIT License. For more
// information, please see the LICENSE.txt file or refer to the following link:
//
//                                           https://opensource.org/license/mit/
// -----------------------------------------------------------------------------

#ifndef   CONFIG_H
#define   CONFIG_H

// ------------------------ Public headers includes ------------------------- //

// ----------------------- Public preprocessor macros ----------------------- //

// -----------------------------------------------------------------------------
// Build specific defines
// -----------------------------------------------------------------------------
#define   HWTT_VERSION                                                 "1.0.0.0"
#define   UART                                                                 0
#define   ETH                                                                  1
#define   PCBA_VERSION                                "MY_BOARD_REV_1_0_FW_1_00"
#define   N_TESTS                                                              6

// -----------------------------------------------------------------------------
// Default data fields
// -----------------------------------------------------------------------------
#define   DEF_USER                                                            ""
#define   DEF_COMP                                                            ""
#define   DEF_BN                                                              ""
#define   DEF_SN                                                              ""
#define   DEF_IPV4_ADDR                                                       ""
#define   DEF_TCP_PORT                                                        ""
#define   DEF_COM_PORT                                                     "COM"

// --------------------- Public data types declarations --------------------- //

// ---------------- Public global data holders declarations ----------------- //

// --------------------- Public functions declarations ---------------------- //

// -----------------------------------------------------------------------------

#endif // CONFIG_H
