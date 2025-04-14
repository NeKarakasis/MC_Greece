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
* File Name     : r_rom_diag.c
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : ROM Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_rom_diag.h"
#include "r_rom_diag_config.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static const uint32_t RRomDiagVer = ((uint32_t)ROM_DIAG_VERSION_MAJOR << 16u) | (uint16_t)ROM_DIAG_VERSION_MINOR;
static uint16_t RRomCheckSum;

extern uint16_t R_ROM_crc(uint32_t start, uint32_t end, uint8_t *crcdir, uint16_t *crcdor);

/*******************************************************************************
* Function Name: R_ROM_Diag_GetVersion
* Description  : Get version number of ROM diagnostic module.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_ROM_Diag_GetVersion(void)
{
    return RRomDiagVer;
} /* End of R_ROM_Diag_GetVersion() */


/*******************************************************************************
* Function Name: R_ROM_Diag_Init
* Description  : ROM diagnostic initialization.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
void R_ROM_Diag_Init(void)
{
    /* CRC module clock is enabled */
    REG_PRCR = (uint16_t)0x0A502;           /* Write enabled */
    REG_MSTPCRB &= (uint32_t)0xFF7FFFFFU;   /* MSTPCRB.MSTPB23 */
    REG_PRCR = (uint16_t)0x0A500;           /* Write disabled */

    /* Set initial value */
    RRomCheckSum = (uint16_t)CRC_INITIAL_VALUE;

    return;
} /* End of R_ROM_Diag_Init() */


/*******************************************************************************
* Function Name: R_ROM_Diag
* Description  : ROM diagnostic operation.
* Arguments    : start   - Specify start address for diagnostic ROM area
*                end     - Specify end address for diagnostic ROM area
*                mode    - Incremental mode (0: Disable, Other than 0: Enable)
* Return Value : CRC-16 checksum
* Note         : None
*******************************************************************************/
uint16_t R_ROM_Diag(uint32_t start, uint32_t end, uint32_t mode)
{
    /* Configure CRC Control Register */
    REG_CRCCR = (uint8_t)CRCCR_VALUE;

    if (ROM_INCR_OFF == mode)
    {
        /* Set initial value */
        REG_CRCDOR = (uint16_t)CRC_INITIAL_VALUE;
    }
    else
    {
        /* Set previous value */
        REG_CRCDOR = RRomCheckSum;
    }

    /* Generate CRC */
    RRomCheckSum = R_ROM_crc(start, end, (uint8_t *)&REG_CRCDIR, (uint16_t *)&REG_CRCDOR);

    return RRomCheckSum;
} /* End of function R_ROM_Diag() */


