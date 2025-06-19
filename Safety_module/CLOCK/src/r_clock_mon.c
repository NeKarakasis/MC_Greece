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
* File Name     : r_clock_mon.c
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Clock Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_clock_mon.h"
#include "r_clock_mon_config.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static void clock_mon_IWDTCLK_set(void);
static void INTERRUPT_Reg_Set(void);

static const uint32_t RClockMonVer = ((uint32_t)R_CLOCK_MON_VERSION_MAJOR << 16u) | (uint16_t)R_CLOCK_MON_VERSION_MINOR;

/*******************************************************************************
* Function Name: R_CLOCK_Mon_GetVersion
* Description  : Get version number of Clock Monitor S/W.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_CLOCK_Mon_GetVersion(void)
{
    return RClockMonVer;
} /* End of function R_CLOCK_Mon_GetVersion() */

/*******************************************************************************
* Function Name: R_CLOCK_Mon_Init
* Description  : CAC register setting and startup.
* Arguments    : target_clk      - Target clock setting
*                target_clk_freq - Target clock frequency
*                    (Input value for calculating pulse count value)
*                tolerance_pct   - Tolerance (Parameters for calculating
*                    the upper and lower limits of the pulse count)
* Return Value : Argument check result value
* Note         : None
*******************************************************************************/
uint32_t R_CLOCK_Mon_Init(uint32_t target_clk, uint32_t target_clk_freq,
        uint32_t tolerance_pct)
{
    uint32_t setting_chk_result;
    uint32_t pulse_cnt, tolerance_calc_val;
    uint16_t pulse_cnt_upper, pulse_cnt_lower;
    uint8_t cacr1_set_val, div_num;
    const uint8_t target_clk_div[4] = {1, 4, 8, 32};

    /* Argument setting check */
    if (3U <= target_clk)
    {
        /* Clock selecting error */
        setting_chk_result = 1U;
    }
    else if ((target_clk_freq < 100U) || (100000U < target_clk_freq) )
    {
        /* Frequency setting error */
        setting_chk_result = 2U;
    }
    else if (100U < tolerance_pct)
    {
        /* tolerance_pct setting error */
        setting_chk_result = 3U;
    }
    else
    {
        /* Argument setting OK */
        setting_chk_result = 0U;

        /* Operation setting of IWDTCLK */
        clock_mon_IWDTCLK_set();

        /* Calculate the division ratio of the target clock */
        for (div_num = 0U; div_num <= 3U; div_num++)
        {
            pulse_cnt = (uint32_t)(((float)target_clk_freq / (float)target_clk_div[div_num]) / ((float)CAC_IWDTCLK / 128U));

            tolerance_calc_val = (pulse_cnt + ((pulse_cnt * tolerance_pct) / 100U));
            if (0xFFFFU >= tolerance_calc_val)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }
        }

        /* Calculate the upper and lower settings from the tolerance_pct */
        pulse_cnt_upper = (uint16_t)((pulse_cnt + ((pulse_cnt * tolerance_pct) / 100U)) - 1U);
        pulse_cnt_lower = (uint16_t)(pulse_cnt - ((pulse_cnt * tolerance_pct) / 100U));

        if (1U <= pulse_cnt_lower)
        {
            pulse_cnt_lower = pulse_cnt_lower - 1U;
        }
        else
        {
            /* Do nothing */
        }

        /* CAC module stop release setting */
        SYSTEM_PRCR_CLOCK_MON.PRCR.WORD = 0xA502U;          /* Disable PRC1 write protection of register */
        SYSTEM_MSTPCRC_CLOCK_MON.MSTPCRC.BIT.MSTPC19 = 0U;  /* Enable CAC module */
        SYSTEM_PRCR_CLOCK_MON.PRCR.WORD = 0xA500U;          /* Enable PRCR write protection of register */

        /* CACR1 register setting */
        if (0U == target_clk)
        {
            cacr1_set_val = 0x00U;  /* Measurement Target Clock: Main clock */
        }
        else if (1U == target_clk)
        {
            cacr1_set_val = 0x04U;  /* Measurement Target Clock: HOCO clock */
        }
        else
        {
            /* Target clock is set to LOCO clock */
            cacr1_set_val = 0x06U;  /* Measurement Target Clock: LOCO clock */
        }
        cacr1_set_val |= div_num << 0x4U;   /* Timer Count Clock Source Select */
        CAC_CACR1_CLOCK_MON.CACR1.BYTE = cacr1_set_val;

        /* CACR2 register setting */
        CAC_CACR2_CLOCK_MON.CACR2.BYTE = 0x19U;         /* Measurement Reference Clock: IWDTCLK, Frequency Division Ration: 1/128 clock */

        /* CAICR register setting */
        CAC_CAICR_CLOCK_MON.CAICR.BYTE = 0x75U;         /* Clear all status flags, Enable overflow and frequency error interrupts */

        /* CAULVR, CALLVR register setting */
        CAC_CAULVR_CLOCK_MON.CAULVR = pulse_cnt_upper;  /* Count limit upper */
        CAC_CALLVR_CLOCK_MON.CALLVR = pulse_cnt_lower;  /* Count limit lower */

        /* Interrupt related register setting */
        INTERRUPT_Reg_Set();

        /* CACR0 register setting */
        CAC_CACR0_CLOCK_MON.CACR0.BIT.CFME = 1U;        /* Enable clock frequency measurement */
    }

    return setting_chk_result;
} /* End of function R_CLOCK_Mon_Init() */

/*******************************************************************************
* Function Name: R_CLOCK_Mon_Stop
* Description  : CAC stopped operating
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
void R_CLOCK_Mon_Stop(void)
{
    /* Disable clock frequency measurement */
    CAC_CACR0_CLOCK_MON.CACR0.BIT.CFME = 0U;
} /* End of function R_CLOCK_Mon_Stop() */

/*******************************************************************************
* Function Name: clock_mon_IWDTCLK_set
* Description  : IWDT clock (IWDTCLK) operation setting.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
static void clock_mon_IWDTCLK_set(void)
{
    uint32_t cnt = 0x00000000;

    if (1U == SYSTEM_ILOCOCR_CLOCK_MON.ILOCOCR.BIT.ILCSTP)
    {
        /* Disable PRC0 write protection of register */
        SYSTEM_PRCR_CLOCK_MON.PRCR.WORD = 0xA501U;

        /* Enable the IWDT dedicated clock (IWDTCLK) */
        SYSTEM_ILOCOCR_CLOCK_MON.ILOCOCR.BIT.ILCSTP = 0U;

        /* Wait for IWDT oscillation stabilization */
        while (cnt != CLOCK_MON_WAIT_CNT)
        {
            cnt++;
        }

        /* Enable PRCR write protection of register */
        SYSTEM_PRCR_CLOCK_MON.PRCR.WORD = 0xA500U;
    }
    else
    {
        /* Do nothing */
    }
} /* End of function clock_mon_IWDTCLK_set() */

/*******************************************************************************
* Function Name: INTERRUPT_Reg_Set
* Description  : CAC interrupt enable setting.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
static void INTERRUPT_Reg_Set(void)
{
#if (1 == CAC_GROUP_INT)
    /* GROUPBL0(Vect:110, CAC-FERRI/CAC-OVFI) Interrupt priority level setting */
    ICU_IPR110_CLOCK_MON.IPR110.BIT.IPR = CLOCK_MON_INTERRUPT_PRIORITY;

    /* GROUPBL0 interrupt enable setting */
    ICU_IER0D_CLOCK_MON.IER0D.BIT.IEN6 = 1U;

    /* GROUPBL0-CAC(FERRI/OVFI) interrupt enable setting */
    ICU_GENBL0_CLOCK_MON.GENBL0.BIT.EN26 = 1U;  /* FERRI */
    ICU_GENBL0_CLOCK_MON.GENBL0.BIT.EN28 = 1U;  /* OVFI */
#else /* #if (1 == CAC_GROUP_INT) */
    /* CAC-FERRF(Vect:32), CAC-OVFF(Vect:34) Interrupt priority level setting */
    ICU_IPR32_CLOCK_MON.IPR32.BIT.IPR = CLOCK_MON_INTERRUPT_PRIORITY;   /* FERRF */
    ICU_IPR34_CLOCK_MON.IPR34.BIT.IPR = CLOCK_MON_INTERRUPT_PRIORITY;   /* OVFF */

    /* CAC(FERRF,OVFF) interrupt enable setting */
    ICU_IER04_CLOCK_MON.IER04.BIT.IEN0 = 1U;     /* FERRF */
    ICU_IER04_CLOCK_MON.IER04.BIT.IEN2 = 1U;     /* OVFF */
#endif /* #if (1 == CAC_GROUP_INT) */
} /* End of function INTERRUPT_Reg_Set() */
