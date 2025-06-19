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
* File Name     : r_vol_mon.h
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

#ifndef R_VOL_MON_H
#define R_VOL_MON_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define R_VOL_MON_VERSION_MAJOR (0x01)
#define R_VOL_MON_VERSION_MINOR (0x00)

/* Specifies the LVDA module that the device is equipped with */
#define VOL_LVDA            (0)
#define VOL_LVDAb           (1)

#if   (0x00000000 == (RX_DEVICE_TYPE & 0x000000F0))
#define VOL_LVDA_VER        VOL_LVDA
#define VOL_LVDA_LVL        (3)
#elif (0x00000010 == (RX_DEVICE_TYPE & 0x000000F0))
#define VOL_LVDA_VER        VOL_LVDA
#define VOL_LVDA_LVL        (5)
#elif (0x00000020 == (RX_DEVICE_TYPE & 0x000000F0))
#define VOL_LVDA_VER        VOL_LVDAb
#define VOL_LVDA_LVL        (9)
#elif (0x00000040 == (RX_DEVICE_TYPE & 0x000000F0))
#define VOL_LVDA_VER        VOL_LVDAb
#define VOL_LVDA_LVL        (14)
#else
#error "Not supported"
#endif

/*******************************************************************************
* Structure definitions
*******************************************************************************/
#pragma bit_order left
#pragma unpack

typedef struct st_system_prcr_vol_mon {
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
} st_system_prcr_vol_mon_t;

typedef struct st_system_lvcmpcr_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :1;
            unsigned char LVD2E:1;
            unsigned char LVD1E:1;
            unsigned char :1;
            unsigned char EXVCCINP2:1;
            unsigned char :3;
        } BIT;
    } LVCMPCR;
} st_system_lvcmpcr_vol_mon_t;

typedef struct st_system_lvdlvlr_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :2;
            unsigned char LVD2LVL:2;
            unsigned char LVD1LVL:4;
        } BIT;
    } LVDLVLR;
} st_system_lvdlvlr_vol_mon_t;

typedef struct st_system_lvd1cr0_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char LVD1RN:1;
            unsigned char LVD1RI:1;
            unsigned char LVD1FSAMP:2;
            unsigned char :1;
            unsigned char LVD1CMPE:1;
            unsigned char LVD1DFDIS:1;
            unsigned char LVD1RIE:1;
        } BIT;
    } LVD1CR0;
} st_system_lvd1cr0_vol_mon_t;

typedef struct st_system_lvd1cr1_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :5;
            unsigned char LVD1IRQSEL:1;
            unsigned char LVD1IDTSEL:2;
        } BIT;
    } LVD1CR1;
} st_system_lvd1cr1_vol_mon_t;

typedef struct st_system_lvd1sr_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :6;
            unsigned char LVD1MON:1;
            unsigned char LVD1DET:1;
        } BIT;
    } LVD1SR;
} st_system_lvd1sr_vol_mon_t;

typedef struct st_icu_nmier_vol_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :2;
            unsigned char LVD2EN:1;
            unsigned char LVD1EN:1;
            unsigned char IWDTEN:1;
            unsigned char :1;
            unsigned char OSTEN:1;
            unsigned char NMIEN:1;
        } BIT;
    } NMIER;
} st_icu_nmier_vol_mon_t;

#pragma bit_order
#pragma packoption

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_VOL_Mon_GetVersion(void);
uint8_t R_VOL_Mon_Init(uint8_t evoltage);
void R_VOL_Mon_Stop(void);

#endif /* R_VOL_MON_H */
