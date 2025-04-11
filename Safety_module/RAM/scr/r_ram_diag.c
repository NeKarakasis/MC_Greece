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
* File Name     : r_ram_diag.c
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

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_ram_diag.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
/* Reserved areas */
#pragma address RramBuffer = RAM_BUFFER_ADDRESS
uint32_t RramBuffer[RAM_BUFFER_SIZE];
#pragma address RramResult1 = RAM_RESULT1_ADDRESS
uint32_t RramResult1;
#pragma address RramResult2 = RAM_RESULT2_ADDRESS
uint32_t RramResult2;


/* RAM area configuration */
const ST_RAM_AREA_CFG RramAreaCfg[NUMBER_OF_AREA] =
{
#if (NUMBER_OF_AREA > 0)
    {startAddress0, BUTSize0, numberOfBUT0},
#endif
#if (NUMBER_OF_AREA > 1)
    {startAddress1, BUTSize1, numberOfBUT1},
#endif
#if (NUMBER_OF_AREA > 2)
    {startAddress2, BUTSize2, numberOfBUT2},
#endif
#if (NUMBER_OF_AREA > 3)
    {startAddress3, BUTSize3, numberOfBUT3},
#endif
#if (NUMBER_OF_AREA > 4)
    {startAddress4, BUTSize4, numberOfBUT4},
#endif
#if (NUMBER_OF_AREA > 5)
    {startAddress5, BUTSize5, numberOfBUT5},
#endif
#if (NUMBER_OF_AREA > 6)
    {startAddress6, BUTSize6, numberOfBUT6},
#endif
#if (NUMBER_OF_AREA > 7)
    {startAddress7, BUTSize7, numberOfBUT7},
#endif
#if (NUMBER_OF_AREA > 8)
#error Set directive NUMBER_OF_AREA to 8 or less
#endif
};

static const uint32_t r_ram_diag_version = ((uint32_t)RAM_DIAG_VERSION_MAJOR << 16u) | (uint16_t)RAM_DIAG_VERSION_MINOR;

extern void R_RAM_Diag_MarchC(uint32_t sAdr, uint32_t eAdr, uint32_t destructive);

/*******************************************************************************
* Function Name: R_RAM_Diag_GetVersion
* Description  : Get version number of RAM diagnostic module.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_RAM_Diag_GetVersion(void)
{
    return r_ram_diag_version;
} /* End of R_RAM_Diag_GetVersion() */

/*******************************************************************************
* Function Name: R_RAM_Diag
* Description  : RAM diagnostic operation.
* Arguments    : area           - Specify RAM area No.
*                index          - Specify RAM block under test
*                destructive    - Specify the kind of test
*                                 (1:destructive test, !0:non-destructive test)
* Return Value : None
* Note         : None
*******************************************************************************/
void R_RAM_Diag(uint32_t area, uint32_t index, uint32_t destructive)
{
    /* Default value is fail. */
    RramResult1 = (uint32_t)RAM_FAIL;
    RramResult2 = (uint32_t)RAM_FAIL;

    /* Check parameters */
    if ( (area < (uint32_t)NUMBER_OF_AREA) && (index < RramAreaCfg[area].numberOfBUT) )
    {
        uint32_t sAdr = R_RAM_BLK_SADR(area, index);
        uint32_t eAdr = R_RAM_BLK_EADR(area, index);

        R_RAM_Diag_MarchC(sAdr, eAdr, destructive);
    }
    else
    {
        /* Do nothing */
    }

    return;
} /* End of R_RAM_Diag() */
