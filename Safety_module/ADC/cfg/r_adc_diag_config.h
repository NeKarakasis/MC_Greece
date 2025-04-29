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
* File Name     : r_adc_diag_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for A/D Converter Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_ADC_DIAG_CONFIG_H
#define R_ADC_DIAG_CONFIG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_adc_diag.h"

/*******************************************************************************
* Macro definitions
*******************************************************************************/
#if   ((RX_DEVICE_TYPE & 0x00000003) == 0x00000001)
#define UNIT0_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit0 */
#elif ((RX_DEVICE_TYPE & 0x00000003) == 0x00000002)
#define UNIT0_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit0 */
#define UNIT1_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit1 */
#elif ((RX_DEVICE_TYPE & 0x00000003) == 0x00000003)
#define UNIT0_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit0 */
#define UNIT1_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit1 */
#define UNIT2_DIAGNOSTIC            (1)     /* 0:OFF, 1:Diagnose unit2 */
#endif

/*----------------------------------------------*/
/*        Do not change below                   */
/*----------------------------------------------*/
/* Address setting for each register */
#define SYSTEM_MSTPCRA_ADC_DIAG (*(volatile struct st_system_mstpcra_adc_diag   __evenaccess *)0x80010)
#define SYSTEM_PRCR_ADC_DIAG    (*(volatile struct st_system_prcr_adc_diag      __evenaccess *)0x803FE)

/* Unit0 */
#define S12AD0_ADCSR_ADC_DIAG   (*(volatile struct st_s12ad_adcsr_adc_diag      __evenaccess *)0x89000)
#define S12AD0_ADCER_ADC_DIAG   (*(volatile struct st_s12ad_adcer_adc_diag      __evenaccess *)0x8900E)
#define S12AD0_ADRD_ADC_DIAG    (*(volatile struct st_s12ad_adrd_adc_diag       __evenaccess *)0x8901E)

#if (2 == NUM_OF_MOUNTED_UNIT)
/* Unit1 */
#define S12AD1_ADCSR_ADC_DIAG   (*(volatile struct st_s12ad_adcsr_adc_diag      __evenaccess *)0x89100)
#define S12AD1_ADCER_ADC_DIAG   (*(volatile struct st_s12ad_adcer_adc_diag      __evenaccess *)0x8910E)
#define S12AD1_ADRD_ADC_DIAG    (*(volatile struct st_s12ad_adrd_adc_diag       __evenaccess *)0x8911E)
#elif (3 == NUM_OF_MOUNTED_UNIT)
/* Unit1 */
#define S12AD1_ADCSR_ADC_DIAG   (*(volatile struct st_s12ad_adcsr_adc_diag      __evenaccess *)0x89200)
#define S12AD1_ADCER_ADC_DIAG   (*(volatile struct st_s12ad_adcer_adc_diag      __evenaccess *)0x8920E)
#define S12AD1_ADRD_ADC_DIAG    (*(volatile struct st_s12ad_adrd_adc_diag       __evenaccess *)0x8921E)

/* Unit2 */
#define S12AD2_ADCSR_ADC_DIAG   (*(volatile struct st_s12ad_adcsr_adc_diag      __evenaccess *)0x89400)
#define S12AD2_ADCER_ADC_DIAG   (*(volatile struct st_s12ad_adcer_adc_diag      __evenaccess *)0x8940E)
#define S12AD2_ADRD_ADC_DIAG    (*(volatile struct st_s12ad_adrd_adc_diag       __evenaccess *)0x8941E)
#endif

#endif /* R_ADC_DIAG_CONFIG_H */
