/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************/
/* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.   */
/*******************************************************************************
* File Name     : r_ram_diag_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for RAM Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_RAM_DIAG_CONFIG_H
#define R_RAM_DIAG_CONFIG_H

#include "safety_functions_api.h"
/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* N is the number of RAM areas to diagnose */
/* NUMBER_OF_AREA : Number of RAM areas to diagnose. NUMBER_OF_AREA = N + 1 */
#define NUMBER_OF_AREA      (1)

/* startAddressN  : Start address of RAM areaN */
/* MUTSizeN       : Overall size of RAM area N (Expressed in double words) */
/* BUTSizeN       : RAM block size of RAM area N (Expressed in double words) */
/* numberOfBUTN   : Number of RAM blocks in RAM area N */

#define startAddress0       RAM_TEST_START_ADDRESS
#define MUTSize0            (RAM_TEST_TOTAL_SIZE*4/4)
#define numberOfBUT0        (RAM_TEST_TOTAL_SIZE)
#define BUTSize0            (MUTSize0/numberOfBUT0) /* Block size = 1K byte */

#if NUMBER_OF_AREA > 1
#define startAddress1       (0x00002000)
#define MUTSize1            (2*1024/4)
#define numberOfBUT1        (4)
#define BUTSize1            (MUTSize1/numberOfBUT1) /* Block size = 512 byte */
#endif/* #if NUMBER_OF_AREA > 1 */

/* RAM test buffer size (Expressed in double words) */
/* Note: Set the maximum RAM block size of all RAM areas */
#define RAM_BUFFER_SIZE     (BUTSize0)

/* Address assignment of reserved area */
/* Note: Assign 3 reserved areas to different RAM blocks separately */
#define RAM_BUFFER_ADDRESS  RAM_MAIN_BUFFER_ADDRESS
#define RAM_RESULT1_ADDRESS RAM_RESULT_ADDRESS_1
#define RAM_RESULT2_ADDRESS RAM_RESULT2_ADDRESS_2

#endif /* R_RAM_DIAG_CONFIG_H */
