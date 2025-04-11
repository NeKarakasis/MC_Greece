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
* File Name     : r_ram_diag.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : RAM Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_RAM_DIAG_H
#define R_RAM_DIAG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include "r_ram_diag_config.h"

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define RAM_DIAG_VERSION_MAJOR  (0x01)
#define RAM_DIAG_VERSION_MINOR  (0x00)

/* RAM diagnostic result */
#define RAM_FAIL                (0U)                         /* Test Fail */
#define RAM_PASS                (1U)                         /* Test Pass */

/* RAM destructive test type */
#define RAM_MEM_NDT             (0U)                         /* Non destructive test */
#define RAM_MEM_DT              (1U)                         /* Destructive test */

/*******************************************************************************
* Macro functions
*******************************************************************************/
#define R_RAM_BLK_SADR(area, index)  (RramAreaCfg[(area)].startAddress + ((RramAreaCfg[(area)].BUTSize * (index)) * 4U))
#define R_RAM_BLK_EADR(area, index)  (R_RAM_BLK_SADR((area), (index)+1U) - 4U)

/*******************************************************************************
* Structure definitions
*******************************************************************************/
typedef struct
{
    const uint32_t startAddress;
    const uint32_t BUTSize;
    const uint32_t numberOfBUT;
} ST_RAM_AREA_CFG;

/*******************************************************************************
* Exported global variables
*******************************************************************************/
extern uint32_t RramBuffer[RAM_BUFFER_SIZE];
extern uint32_t RramResult1;
extern uint32_t RramResult2;
extern const ST_RAM_AREA_CFG RramAreaCfg[NUMBER_OF_AREA];

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_RAM_Diag_GetVersion(void);
void R_RAM_Diag(uint32_t area, uint32_t index, uint32_t destructive);

#endif /* R_RAM_DIAG_H */
