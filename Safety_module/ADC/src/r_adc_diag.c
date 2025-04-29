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
* File Name     : r_adc_diag.c
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : A/D Converter Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_adc_diag.h"
#include "r_adc_diag_config.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static const uint32_t RAdcDiagVer = ((uint32_t)R_ADC_DIAG_VERSION_MAJOR << 16u) | (uint16_t)R_ADC_DIAG_VERSION_MINOR;

/*******************************************************************************
* Function Name: R_ADC_Diag_GetVersion
* Description  : Get version number of A/D converter diagnostic S/W.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_ADC_Diag_GetVersion(void)
{
    return RAdcDiagVer;
} /* End of function R_ADC_Diag_GetVersion() */

/*******************************************************************************
* Function Name: R_ADC_Diag_Init
* Description  : Enable the S12AD.
* Arguments    : None
* Return Value : None
* Note         : None
*******************************************************************************/
void R_ADC_Diag_Init(void)
{
    /* Disable register protection (PRC1) */
    SYSTEM_PRCR_ADC_DIAG.PRCR.WORD = 0xA502U;
    /* Enable the S12AD */
#if ( 1 == UNIT0_DIAGNOSTIC )
    SYSTEM_MSTPCRA_ADC_DIAG.MSTPCRA.BIT.MSTPA17 = 0U;
#endif
#if ( 1 == UNIT1_DIAGNOSTIC )
    SYSTEM_MSTPCRA_ADC_DIAG.MSTPCRA.BIT.MSTPA16 = 0U;
#endif
#if ( 1 == UNIT2_DIAGNOSTIC )
    SYSTEM_MSTPCRA_ADC_DIAG.MSTPCRA.BIT.MSTPA23 = 0U;
#endif
    /* Re-Enable register protection */
    SYSTEM_PRCR_ADC_DIAG.PRCR.WORD = 0xA500U;
} /* End of function R_ADC_Diag_Init() */

/*******************************************************************************
* Function Name: R_ADC_Diag
* Description  : Perform S12AD diagnostics.
* Arguments    : unit      - Target unit number
*              : voltage   - Self-Diagnosis Conversion Voltage
* Return Value : A/D Self-Diagnosis Data(ADRD register data)
* Note         : None
*******************************************************************************/
uint16_t R_ADC_Diag(uint8_t unit, uint8_t voltage)
{
    volatile struct st_s12ad_adcsr_adc_diag     __evenaccess *ADCSR_ADC_DIAG;
    volatile struct st_s12ad_adcer_adc_diag     __evenaccess *ADCER_ADC_DIAG;
    volatile struct st_s12ad_adrd_adc_diag      __evenaccess *ADRD_ADC_DIAG;
    uint8_t error_flag = 0U;
    uint16_t Diag_Value;

    if (0U == unit)
    {
#if (1 == UNIT0_DIAGNOSTIC)
        /* Settings for unit 0 */
        ADCSR_ADC_DIAG = &S12AD0_ADCSR_ADC_DIAG;
        ADCER_ADC_DIAG = &S12AD0_ADCER_ADC_DIAG;
        ADRD_ADC_DIAG = &S12AD0_ADRD_ADC_DIAG;
#endif /* #if (1 == UNIT0_DIAGNOSTIC) */
    }
#if (1 == UNIT1_DIAGNOSTIC)
    else if (1U == unit)
    {
        /* Settings for unit 1 */
        ADCSR_ADC_DIAG = &S12AD1_ADCSR_ADC_DIAG;
        ADCER_ADC_DIAG = &S12AD1_ADCER_ADC_DIAG;
        ADRD_ADC_DIAG = &S12AD1_ADRD_ADC_DIAG;
    }
#endif /* #if (1 == UNIT1_DIAGNOSTIC) */
#if (1 == UNIT2_DIAGNOSTIC)
    else if (2U == unit)
    {
        /* Settings for unit 2 */
        ADCSR_ADC_DIAG = &S12AD2_ADCSR_ADC_DIAG;
        ADCER_ADC_DIAG = &S12AD2_ADCER_ADC_DIAG;
        ADRD_ADC_DIAG = &S12AD2_ADRD_ADC_DIAG;
    }
#endif /* #if (1 == UNIT2_DIAGNOSTIC) */
    else
    {
        /* argument error */
        error_flag = 1U;
    }

    if ((voltage < 1U) || (3U < voltage))
    {
        /* argument error */
        error_flag = 1U;
    }
    else
    {
        /* Do nothing */
    }

    if (0U == error_flag)
    {
        /* Set A/D Control Register (ADCSR) */
        ADCSR_ADC_DIAG->ADCSR.BIT.ADCS = 0U;            /* Set single mode */
        ADCSR_ADC_DIAG->ADCSR.BIT.DBLE = 0U;            /* Deselects double trigger mode */
        ADCSR_ADC_DIAG->ADCSR.BIT.ADIE = 0U;            /* Scan end interrupt is disabled */

        /* Set A/D Control Extended Register (ADCER) */
        ADCER_ADC_DIAG->ADCER.BIT.DIAGVAL = voltage;    /* Set Self-Diagnosis Conversion Voltage */
        ADCER_ADC_DIAG->ADCER.BIT.DIAGLD = 1U;          /* Set fixed mode for self-diagnosis voltage */
        ADCER_ADC_DIAG->ADCER.BIT.DIAGM = 1U;           /* Enables self-diagnosis of 12-bit A/D converter */

        /* Start conversion */
        ADCSR_ADC_DIAG->ADCSR.BIT.ADST = 1U;

        /* Wait for conversion to finish */
        while (1U == ADCSR_ADC_DIAG->ADCSR.BIT.ADST)
        {
            /* Do nothing */
        }

        /* Get A/D conversion results */
        Diag_Value = ADRD_ADC_DIAG->ADRD.WORD;
        /* Disables self-diagnosis of 12-bit A/D converter */
        ADCER_ADC_DIAG->ADCER.BIT.DIAGM = 0U;
    }
    else
    {
        /* Notify that there was an error in the argument */
        Diag_Value = 0xFFFFU;
    }

    return Diag_Value;
} /* End of function R_ADC_Diag() */
