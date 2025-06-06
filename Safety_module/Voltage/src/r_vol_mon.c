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
* File Name     : r_vol_mon.c
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Voltage Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_vol_mon.h"
#include "r_vol_mon_config.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static const uint32_t RVolMonVer = ((uint32_t)R_VOL_MON_VERSION_MAJOR << 16u) | (uint16_t)R_VOL_MON_VERSION_MINOR;

/*******************************************************************************
* Function Name: R_VOL_Mon_GetVersion
* Description  : Get version number of voltage monitor S/W.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_VOL_Mon_GetVersion(void)
{
    return RVolMonVer;
} /* End of function R_VOL_MON_GetVersion() */

/*******************************************************************************
* Function Name: R_VOL_Mon_Init
* Description  : Procedures for Setting Bits Related to the Voltage
*                Monitoring 1 Interrupt
* Arguments    : evoltage      - Voltage Detection 1 Level Select
* Return Value : None
* Note         : None
*******************************************************************************/
uint8_t R_VOL_Mon_Init(uint8_t evoltage)
{
    uint8_t i = 0U;
    uint8_t result = 0U;
    uint32_t cnt = 0x00000000;

    const uint8_t tbl_lvd1lvl[] = {
#if   ((VOL_LVDA_VER == VOL_LVDAb) && (VOL_LVDA_LVL == (14)))
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
#elif ((VOL_LVDA_VER == VOL_LVDAb) && (VOL_LVDA_LVL == (9)))
            0, 1, 2, 3, 4, 5, 6, 7, 8
#elif ((VOL_LVDA_VER == VOL_LVDA) && (VOL_LVDA_LVL == (5)))
            4, 5, 6, 10, 11
#elif ((VOL_LVDA_VER == VOL_LVDA) && (VOL_LVDA_LVL == (3)))
            9, 10, 11
#endif
    };

    /* Argument setting check */
    for (i = 0U; i < sizeof(tbl_lvd1lvl); i++ )
    {
        if (tbl_lvd1lvl[i] == evoltage)
        {
            result = 1U;
            break;
        }
    }

    if (1U == result)
    {
        /* Disable register protection (PRC3) */
        SYSTEM_PRCR_VOL_MON.PRCR.WORD = 0xA508U;

        /* Select voltage level */
        SYSTEM_LVDLVLR_VOL_MON.LVDLVLR.BIT.LVD1LVL = evoltage;

#if (VOL_LVDA_VER == VOL_LVDAb)
        /* Voltage monitoring 1 interrupt occurs when the voltage passes Vdet1 */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1RI = 0U;
        /* When VCC < Vdet1 (drop) is detected */
        SYSTEM_LVD1CR1_VOL_MON.LVD1CR1.BIT.LVD1IDTSEL = 1U;
        /* Select NMI interrupt not maskable */
        SYSTEM_LVD1CR1_VOL_MON.LVD1CR1.BIT.LVD1IRQSEL = 0U;
#endif /* #if (VOL_LVDA_VER == VOL_LVDAb) */

        /* Enable voltage detection 1 circuit */
        SYSTEM_LVCMPCR_VOL_MON.LVCMPCR.BIT.LVD1E = 1U;

        /* Wait for at least td(E-A). */
        while (cnt != VOL_WAIT_CNT_TD)
        {
            cnt++;
        }

#if (VOL_LVDA_VER == VOL_LVDAb)
        /* Enable voltage monitoring 1 circuit comparison results output */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1CMPE = 1U;

        /* clear variable cnt */
        cnt = 0U;

        /* Wait for at least 2 us */
        while (cnt != VOL_WAIT_CNT_2US)
        {
            cnt++;
        }

#else /* #if (VOL_LVDA_VER == VOL_LVDAb) */
#if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF))
        /* Set Sampling Clock */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1FSAMP = SAMPLING_CLOCK;
        /* Enable digital filter */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1DFDIS = 0U;

        /* clear variable cnt */
        cnt = 0U;

        /* Wait for at least  2n + 3 cycles of the LOCO */
        while (cnt != VOL_WAIT_CNT_LOCO)
        {
            cnt++;
        }
#endif /* #if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF)) */

        /* Voltage monitoring 1 interrupt occurs when the voltage passes Vdet1 */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1RI = 0U;
        /* When VCC < Vdet1 (drop) is detected */
        SYSTEM_LVD1CR1_VOL_MON.LVD1CR1.BIT.LVD1IDTSEL = 1U;
        /* Select NMI interrupt not maskable */
        SYSTEM_LVD1CR1_VOL_MON.LVD1CR1.BIT.LVD1IRQSEL = 0U;
#endif /* #if (VOL_LVDA_VER == VOL_LVDAb) */

        /* Clear Detection Flag */
        SYSTEM_LVD1SR_VOL_MON.LVD1SR.BIT.LVD1DET = 0U;
        /* Enable Voltage Monitoring 1 Interrupt */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1RIE = 1U;

#if (VOL_LVDA_VER == VOL_LVDA)
        /* Enable voltage monitoring 1 circuit comparison results output */
        SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1CMPE = 1U;
#endif /* #if (VOL_LVDA_VER == VOL_LVDA) */

        /* Enable write protection of registers */
        SYSTEM_PRCR_VOL_MON.PRCR.WORD = 0xA500U;

        /* Enable Interrupt (NMI) on ICU */
        ICU_NMIER_VOL_MON.NMIER.BIT.LVD1EN = 1U;
    }
    else
    {
        /* Do nothing */
    }

    return result;
} /* End of function R_VOL_MON_Init() */

/*******************************************************************************
* Function Name: R_VOL_Mon_Stop
* Description  : Stop voltage monitoring.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
void R_VOL_Mon_Stop(void)
{
#if (VOL_LVDA_VER == VOL_LVDA)
#if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF))
    uint32_t cnt = 0x00000000;
#endif
#endif /* #if (VOL_LVDA_VER == VOL_LVDA) */

    /* Disable register protection (PRC3) */
    SYSTEM_PRCR_VOL_MON.PRCR.WORD = 0xA508U;

#if (VOL_LVDA_VER == VOL_LVDAb)
    /* Disable Voltage Monitoring 1 Interrupt */
    SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1RIE = 0U;
#endif /* #if (VOL_LVDA_VER == VOL_LVDAb) */
    /* Disable voltage monitoring 1 circuit comparison results output */
    SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1CMPE = 0U;

#if (VOL_LVDA_VER == VOL_LVDA)
#if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF))
    /* Wait for at least  2n + 3 cycles of the LOCO */
    while (cnt != VOL_WAIT_CNT_LOCO)
    {
        cnt++;
    }
#endif /* #if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF)) */

    /* Disable Voltage Monitoring 1 Interrupt */
    SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1RIE = 0U;

#if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF))
    /* Disable digital filter */
    SYSTEM_LVD1CR0_VOL_MON.LVD1CR0.BIT.LVD1DFDIS = 1U;
#endif /* #if (defined(DIGITAL_FILTER_ONOFF) && (1 == DIGITAL_FILTER_ONOFF)) */
#endif /* #if (VOL_LVDA_VER == VOL_LVDA) */

    /* Disable voltage detection 1 circuit */
    SYSTEM_LVCMPCR_VOL_MON.LVCMPCR.BIT.LVD1E = 0U;

    /* Enable write protection of registers */
    SYSTEM_PRCR_VOL_MON.PRCR.WORD = 0xA500U;
} /* End of function R_VOL_MON_Stop() */
