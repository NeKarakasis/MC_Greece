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
* File Name     : r_pc_mon.h
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

#ifndef R_PC_MON_H
#define R_PC_MON_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define R_PC_MON_VERSION_MAJOR      (0x01)
#define R_PC_MON_VERSION_MINOR      (0x00)

/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef enum e_iwdt_cks
{
    IWDT_CKS_DIV_1   = 0,
    IWDT_CKS_DIV_16  = 2,
    IWDT_CKS_DIV_128 = 15,
    IWDT_CKS_DIV_256 = 5,
} e_iwdt_cks_t;

typedef enum e_iwdt_window_st
{
    IWDT_WINDOW_START_25  = 0,
    IWDT_WINDOW_START_50  = 1,
    IWDT_WINDOW_START_75  = 2,
} e_iwdt_window_st_t;

typedef enum e_iwdt_window_end
{
    IWDT_WINDOW_END_50 = 1,
    IWDT_WINDOW_END_25 = 2,
    IWDT_WINDOW_END_0  = 3,
} e_iwdt_window_end_t;

typedef enum e_iwdt_action
{
    IWDT_ACTION_NMI = 0,
    IWDT_ACTION_RST = 1,
} e_iwdt_action_t;

/*******************************************************************************
* Structure definitions
*******************************************************************************/
#pragma bit_order left
#pragma unpack

typedef struct st_system_ilococr_pc_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :7;
            unsigned char ILCSTP:1;
        } BIT;
    } ILOCOCR;
} st_system_ilococr_pc_mon_t;

typedef struct st_system_prcr_pc_mon {
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
} st_system_prcr_pc_mon_t;

typedef struct st_icu_nmier_pc_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char :1;
            unsigned char RAMEN:1;
            unsigned char LVD2EN:1;
            unsigned char LVD1EN:1;
            unsigned char IWDTEN:1;
            unsigned char WDTEN:1;
            unsigned char OSTEN:1;
            unsigned char NMIEN:1;
        } BIT;
    } NMIER;
} st_icu_nmier_pc_mon_t;

typedef struct st_iwdt_iwdtrr_pc_mon {
    unsigned char  IWDTRR;
} st_iwdt_iwdtrr_pc_mon_t;

typedef struct st_iwdt_iwdtcr_pc_mon {
    union {
        unsigned short WORD;
        struct {
            unsigned short :2;
            unsigned short RPSS:2;
            unsigned short :2;
            unsigned short RPES:2;
            unsigned short CKS:4;
            unsigned short :2;
            unsigned short TOPS:2;
        } BIT;
    } IWDTCR;
} st_iwdt_iwdtcr_pc_mon_t;

typedef struct st_iwdt_iwdtsr_pc_mon {
    union {
        unsigned short WORD;
        struct {
            unsigned short REFEF:1;
            unsigned short UNDFF:1;
            unsigned short CNTVAL:14;
        } BIT;
    } IWDTSR;
} st_iwdt_iwdtsr_pc_mon_t;

typedef struct st_iwdt_iwdtrcr_pc_mon {
    union {
        unsigned char BYTE;
        struct {
            unsigned char RSTIRQS:1;
            unsigned char :7;
        } BIT;
    } IWDTRCR;
} st_iwdt_iwdtrcr_pc_mon_t;

#pragma bit_order
#pragma packoption

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_PC_Mon_GetVersion(void);
void R_PC_Mon_Init(e_iwdt_cks_t clockdiv, e_iwdt_window_st_t startpos,
                   e_iwdt_window_end_t endpos, e_iwdt_action_t rstirq);
void R_PC_Mon_Refresh(void);
void R_PC_Mon_Check(int32_t *result);

#endif /* R_PC_MON_H */
