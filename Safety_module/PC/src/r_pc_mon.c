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
* File Name     : r_pc_mon.c
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Program Counter Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_pc_mon.h"
#include "r_pc_mon_config.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static void pc_mon_IWDTCLK_set(void);

static const uint32_t RPcMonVer = ((uint32_t)R_PC_MON_VERSION_MAJOR << 16u) | (uint16_t)R_PC_MON_VERSION_MINOR;

/*******************************************************************************
* Function Name: R_PC_Mon_GetVersion
* Description  : Get version number of Program Counter Monitor S/W.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_PC_Mon_GetVersion(void)
{
    return RPcMonVer;
} /* End of function R_PC_Mon_GetVersion() */

/*******************************************************************************
* Function Name: R_PC_Mon_Init
* Description  : IWDT register setting and startup.
* Arguments    : clockdiv        - Clock division setting
*                startpos        - Window start position setting
*                endpos          - Window end position setting
*                rstirq          - Action settings for error
* Return Value : None
* Note         : None
*******************************************************************************/
void R_PC_Mon_Init(e_iwdt_cks_t clockdiv, e_iwdt_window_st_t startpos,
                   e_iwdt_window_end_t endpos, e_iwdt_action_t rstirq)
{
    st_iwdt_iwdtcr_pc_mon_t temp;
    uint16_t timeout_period;

    /* Operation setting of IWDTCLK */
    pc_mon_IWDTCLK_set();

    /* Check clock divide ratio */
    if (clockdiv == IWDT_CKS_DIV_256)
    {
        timeout_period = 0x1U;                  /* Timeout Period Select: 512 cycles@15kHz, 4096 cycles@120kHz */
    }
    else
    {
        timeout_period = 0x0U;                  /* Timeout Period Select: 128 cycles@15kHz, 1024 cycles@120kHz */
    }

    /* Set the IWDTCR register */
    temp.IWDTCR.WORD = 0x0000U;
    temp.IWDTCR.BIT.TOPS = timeout_period;      /* Timeout Period Select */
    temp.IWDTCR.BIT.RPSS = (uint16_t)startpos;  /* Window Start Position Select */
    temp.IWDTCR.BIT.RPES = (uint16_t)endpos;    /* Window End Position Select */
    temp.IWDTCR.BIT.CKS  = (uint16_t)clockdiv;  /* Clock Divide Ratio Select */
    IWDT_IWDTCR_PC_MON.IWDTCR = temp.IWDTCR;

    /* Set NMI or Reset action */
    if (rstirq == IWDT_ACTION_RST)
    {
        /* Reset */
        IWDT_IWDTRCR_PC_MON.IWDTRCR.BIT.RSTIRQS = 1U;
    }
    else
    {
        /* NMI */
        IWDT_IWDTRCR_PC_MON.IWDTRCR.BIT.RSTIRQS = 0U;

        /* Enable the NMI for IWDT */
        ICU_NMIER_PC_MON.NMIER.BIT.IWDTEN = 1U;
    }

    /* Start the count going by doing initial refresh (Register start mode) */
    R_PC_Mon_Refresh();
}/* End of function R_PC_Mon_Init() */


/*******************************************************************************
* Function Name: pc_mon_IWDTCLK_set
* Description  : IWDT clock (IWDTCLK) operation setting.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
static void pc_mon_IWDTCLK_set(void)
{
    uint32_t cnt = 0x00000000;

    if (1U == SYSTEM_ILOCOCR_PC_MON.ILOCOCR.BIT.ILCSTP)
    {
        /* Disable PRC0 write protection of register */
        SYSTEM_PRCR_PC_MON.PRCR.WORD = 0xA501U;

        /* Enable the IWDT dedicated clock (IWDTCLK) */
        SYSTEM_ILOCOCR_PC_MON.ILOCOCR.BIT.ILCSTP = 0U;

        /* Wait for IWDT oscillation stabilization */
        while (cnt != PC_MON_WAIT_CNT)
        {
            cnt++;
        }

        /* Enable PRCR write protection of register */
        SYSTEM_PRCR_PC_MON.PRCR.WORD = 0xA500U;
    }
    else
    {
        /* Do nothing */
    }
}/* End of function pc_mon_IWDTCLK_set() */

/*******************************************************************************
* Function Name: R_PC_Mon_Refresh
* Description  : Refreshing the IWDT counter.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
void R_PC_Mon_Refresh(void)
{
    /* Refreshing the IWDT counter */
    IWDT_IWDTRR_PC_MON.IWDTRR = 0x00U;
    IWDT_IWDTRR_PC_MON.IWDTRR = 0xFFU;
}/* End of function R_PC_Mon_Refresh() */

/*******************************************************************************
* Function Name: R_PC_Mon_Check
* Description  : Check the IWDT status register value.
* Arguments    : result          - Result of checking the status register value
* Return Value : None
* Note         : None
*******************************************************************************/
void R_PC_Mon_Check(int32_t *result)
{
    /* Initial value is an Error value */
    *result = 0x00000000;

    /* Read status register to see if underflow has occurred */
    if (1U == IWDT_IWDTSR_PC_MON.IWDTSR.BIT.UNDFF)
    {
        *result = 0x00000000;   /* Error */
    }
    /* Read status register to see if refresh error has occurred */
    else if (1U == IWDT_IWDTSR_PC_MON.IWDTSR.BIT.REFEF)
    {
        *result = 0x00000000;   /* Error */
    }
    /* No underflow or refresh errors occurred */
    else
    {
        *result = 0x00000001;   /* Pass */
    }
} /* End of function R_PC_Mon_Check() */
