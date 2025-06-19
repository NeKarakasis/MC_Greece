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
* File Name     : r_clock_mon.h
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

#ifndef R_CLOCK_MON_H
#define R_CLOCK_MON_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define R_CLOCK_MON_VERSION_MAJOR       (0x01)
#define R_CLOCK_MON_VERSION_MINOR       (0x00)

/* Specifies the type of CAC interrupts the device is equipped with */
#if (0x00000100 == (RX_DEVICE_TYPE & 0x00000100))
#define CAC_GROUP_INT           		(1)
#else
#define CAC_GROUP_INT           		(0)
#endif

/* Specifies the IWDT operating clock that the device is equipped with */
#if (0x00000100 == (RX_DEVICE_TYPE & 0x00000100))
#define CAC_IWDTCLK         			(120U) /* kHz */
#else /* #if (0x00000100 == (RX_DEVICE_TYPE & 0x00000100)) */
#define CAC_IWDTCLK         			(15U)  /* kHz */
#endif/*#if (0x00000100 == (RX_DEVICE_TYPE & 0x00000100))*/

/*******************************************************************************
* Structure definitions
*******************************************************************************/
#pragma bit_order left
#pragma unpack

typedef struct st_system_mstpcrc_clock_mon {
    union {
        unsigned long LONG;
        struct {
            unsigned long :4;
            unsigned long MSTPC27:1;
            unsigned long MSTPC26:1;
            unsigned long MSTPC25:1;
            unsigned long MSTPC24:1;
            unsigned long :1;
            unsigned long MSTPC22:1;
            unsigned long :2;
            unsigned long MSTPC19:1;
            unsigned long :1;
            unsigned long MSTPC17:1;
            unsigned long :9;
            unsigned long MSTPC7:1;
            unsigned long :6;
            unsigned long MSTPC0:1;
        } BIT;
    } MSTPCRC;
} st_system_mstpcrc_clock_mon_t;

typedef struct st_system_ilococr_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :7;
            unsigned char ILCSTP:1;
        } BIT;
    } ILOCOCR;
} st_system_ilococr_clock_mon_t;

typedef struct st_system_prcr_clock_mon {
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
} st_system_prcr_clock_mon_t;

typedef struct st_cac_cacr0_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :7;
            unsigned char CFME:1;
        } BIT;
    } CACR0;
} st_cac_cacr0_clock_mon_t;

typedef struct st_cac_cacr1_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char EDGES:2;
            unsigned char TCSS:2;
            unsigned char FMCS:3;
            unsigned char CACREFE:1;
        } BIT;
    } CACR1;
} st_cac_cacr1_clock_mon_t;

typedef struct st_cac_cacr2_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char DFS:2;
            unsigned char RCDS:2;
            unsigned char RSCS:3;
            unsigned char RPS:1;
        } BIT;
    } CACR2;
} st_cac_cacr2_clock_mon_t;

typedef struct st_cac_caicr_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :1;
            unsigned char OVFFCL:1;
            unsigned char MENDFCL:1;
            unsigned char FERRFCL:1;
            unsigned char :1;
            unsigned char OVFIE:1;
            unsigned char MENDIE:1;
            unsigned char FERRIE:1;
        } BIT;
    } CAICR;
} st_cac_caicr_clock_mon_t;

typedef struct st_cac_caulvr_clock_mon {
    unsigned short CAULVR;
} st_cac_caulvr_clock_mon_t;

typedef struct st_cac_callvr_clock_mon {
    unsigned short CALLVR;
} st_cac_callvr_clock_mon_t;

#if (1 == CAC_GROUP_INT)
typedef struct st_icu_ier0d_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char IEN7:1;
            unsigned char IEN6:1;
            unsigned char IEN5:1;
            unsigned char IEN4:1;
            unsigned char IEN3:1;
            unsigned char IEN2:1;
            unsigned char IEN1:1;
            unsigned char IEN0:1;
        } BIT;
    } IER0D;
} st_icu_ier0d_clock_mon_t;

typedef struct st_icu_ipr110_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :4;
            unsigned char IPR:4;
        } BIT;
    } IPR110;
} st_icu_ipr110_clock_mon_t;

typedef struct st_icu_genbl0_clock_mon {
    union {
        unsigned long LONG;
        struct {
            unsigned long EN31:1;
            unsigned long EN30:1;
            unsigned long EN29:1;
            unsigned long EN28:1;
            unsigned long EN27:1;
            unsigned long EN26:1;
            unsigned long EN25:1;
            unsigned long EN24:1;
            unsigned long EN23:1;
            unsigned long EN22:1;
            unsigned long EN21:1;
            unsigned long EN20:1;
            unsigned long EN19:1;
            unsigned long EN18:1;
            unsigned long EN17:1;
            unsigned long EN16:1;
            unsigned long EN15:1;
            unsigned long EN14:1;
            unsigned long EN13:1;
            unsigned long EN12:1;
            unsigned long EN11:1;
            unsigned long EN10:1;
            unsigned long EN9:1;
            unsigned long EN8:1;
            unsigned long EN7:1;
            unsigned long EN6:1;
            unsigned long EN5:1;
            unsigned long EN4:1;
            unsigned long EN3:1;
            unsigned long EN2:1;
            unsigned long EN1:1;
            unsigned long EN0:1;
        } BIT;
    } GENBL0;
} st_icu_genbl0_clock_mon_t;

#else /* #if (1 == CAC_GROUP_INT) */
typedef struct st_icu_ier04_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char IEN7:1;
            unsigned char IEN6:1;
            unsigned char IEN5:1;
            unsigned char IEN4:1;
            unsigned char IEN3:1;
            unsigned char IEN2:1;
            unsigned char IEN1:1;
            unsigned char IEN0:1;
        } BIT;
    } IER04;
} st_icu_ier04_clock_mon_t;

typedef struct st_icu_ipr32_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :4;
            unsigned char IPR:4;
        } BIT;
    } IPR32;
} st_icu_ipr32_clock_mon_t;

typedef struct st_icu_ipr34_clock_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :4;
            unsigned char IPR:4;
        } BIT;
    } IPR34;
} st_icu_ipr34_clock_mon_t;

#endif /* #if (1 == CAC_GROUP_INT) */

#pragma bit_order
#pragma packoption

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_CLOCK_Mon_GetVersion(void);
uint32_t R_CLOCK_Mon_Init(uint32_t target_clk, uint32_t target_clk_freq, uint32_t tolerance_pct);
void R_CLOCK_Mon_Stop(void);

#endif /* R_CLOCK_MON_H */
