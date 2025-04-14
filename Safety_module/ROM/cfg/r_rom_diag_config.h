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
* File Name     : r_rom_diag_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for ROM Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_ROM_DIAG_CONFIG_H
#define R_ROM_DIAG_CONFIG_H

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Define register address */
#define REG_PRCR            *(volatile uint16_t *) (0x000803FE)    /* Protect Register */
#define REG_MSTPCRB         *(volatile uint32_t *) (0x00080014)    /* Module Stop Control Register(Target module: CRC) */

#define REG_CRCCR           *(volatile uint8_t * ) (0x00088280)    /* CRC Control Register */
#if (RX_DEVICE_TYPE & 0x000C0000) == 0x00040000
    #define REG_CRCDIR      *(volatile uint8_t * ) (0x00088284)    /* CRC Data Input Register */
    #define REG_CRCDOR      *(volatile uint16_t *) (0x00088288)    /* CRC Data Output Register */
#else
    #define REG_CRCDIR      *(volatile uint8_t * ) (0x00088281)    /* CRC Data Input Register */
    #define REG_CRCDOR      *(volatile uint16_t *) (0x00088282)    /* CRC Data Output Register */
#endif

/* Define register settings */
#define CRCCR_VALUE         (0x02)      /* CRC-16, LSB First */
#define CRC_INITIAL_VALUE   (0x0000)    /* CRC initial value */

#endif /* R_ROM_DIAG_CONFIG_H */
