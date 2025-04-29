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
* File Name     : r_adc_diag.h
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

#ifndef R_ADC_DIAG_H
#define R_ADC_DIAG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define R_ADC_DIAG_VERSION_MAJOR    (0x01)
#define R_ADC_DIAG_VERSION_MINOR    (0x00)

/* Definition of number of units installed */
#if   ((RX_DEVICE_TYPE & 0x00000003) == 0x00000001)
#define NUM_OF_MOUNTED_UNIT         (1)
#define UNIT1_DIAGNOSTIC            (0)     /* 0:OFF, 1:Diagnose unit1 */
#define UNIT2_DIAGNOSTIC            (0)     /* 0:OFF, 1:Diagnose unit2 */
#elif ((RX_DEVICE_TYPE & 0x00000003) == 0x00000002)
#define NUM_OF_MOUNTED_UNIT         (2)
#define UNIT2_DIAGNOSTIC            (0)     /* 0:OFF, 1:Diagnose unit2 */
#elif ((RX_DEVICE_TYPE & 0x00000003) == 0x00000003)
#define NUM_OF_MOUNTED_UNIT         (3)
#endif

/*******************************************************************************
* Structure definitions
*******************************************************************************/
#pragma bit_order left
#pragma unpack

typedef struct st_system_mstpcra_adc_diag {
    union {
        unsigned long LONG;
        struct {
            unsigned long ACSE:1;
            unsigned long :1;
            unsigned long MSTPA29:1;
            unsigned long MSTPA28:1;
            unsigned long MSTPA27:1;
            unsigned long :2;
            unsigned long MSTPA24:1;
            unsigned long MSTPA23:1;
            unsigned long :3;
            unsigned long MSTPA19:1;
            unsigned long :1;
            unsigned long MSTPA17:1;
            unsigned long MSTPA16:1;
            unsigned long MSTPA15:1;
            unsigned long MSTPA14:1;
            unsigned long MSTPA13:1;
            unsigned long :1;
            unsigned long MSTPA11:1;
            unsigned long MSTPA10:1;
            unsigned long MSTPA9:1;
            unsigned long :3;
            unsigned long MSTPA5:1;
            unsigned long MSTPA4:1;
            unsigned long :2;
            unsigned long MSTPA1:1;
            unsigned long MSTPA0:1;
        } BIT;
    } MSTPCRA;
} st_system_mstpcra_adc_diag_t;

typedef struct st_system_prcr_adc_diag {
    union {
        unsigned short WORD;
        struct {
            unsigned short PRKEY:8;
            unsigned short :4;
            unsigned short PRC3:1;
            unsigned short :1;
            unsigned short PRC1:1;
            unsigned short PRC0:1;
        } BIT;
    } PRCR;
} st_system_prcr_adc_diag_t;

typedef struct st_s12ad_adcsr_adc_diag {
    union {
        unsigned short WORD;
        struct {
            unsigned short ADST:1;
            unsigned short ADCS:2;
            unsigned short ADIE:1;
            unsigned short :2;
            unsigned short TRGE:1;
            unsigned short EXTRG:1;
            unsigned short DBLE:1;
            unsigned short GBADIE:1;
            unsigned short :1;
            unsigned short DBLANS:5;
        } BIT;
    } ADCSR;
} st_s12ad_adcsr_adc_diag_t;

typedef struct st_s12ad_adcer_adc_diag {
    union {
        unsigned short WORD;
        struct {
            unsigned short ADRFMT:1;
            unsigned short :3;
            unsigned short DIAGM:1;
            unsigned short DIAGLD:1;
            unsigned short DIAGVAL:2;
            unsigned short :2;
            unsigned short ACE:1;
            unsigned short :2;
            unsigned short ADPRC:2;
            unsigned short :1;
        } BIT;
    } ADCER;
} st_s12ad_adcer_adc_diag_t;

typedef struct st_s12ad_adrd_adc_diag {
    union {
        unsigned short WORD;
        union {
            struct {
                unsigned short DIAGST:2;
                unsigned short :2;
                unsigned short AD:12;
            } RIGHT;
            struct {
                unsigned short AD:12;
                unsigned short :2;
                unsigned short DIAGST:2;
            } LEFT;
        } BIT;
    } ADRD;
} st_s12ad_adrd_adc_diag_t;

#pragma bit_order
#pragma packoption

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_ADC_Diag_GetVersion(void);
void R_ADC_Diag_Init(void);
uint16_t R_ADC_Diag(uint8_t unit, uint8_t voltage);

#endif /* R_ADC_DIAG_H */
