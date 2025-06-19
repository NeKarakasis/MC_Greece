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
* File Name     : r_vol_mon_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for Voltage Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_VOL_MON_CONFIG_H
#define R_VOL_MON_CONFIG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_vol_mon.h"

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Wait time setting value for at least td(E-A) */
#define VOL_WAIT_CNT_TD     (9000U)

/* For devices with LVDAb module */
#if   ((0x00000020 == (RX_DEVICE_TYPE & 0x000000F0)) || (0x00000040 == (RX_DEVICE_TYPE & 0x000000F0)))
    /* Wait time setting value for at least 2us */
    #define VOL_WAIT_CNT_2US            (60U)

/* For devices with LVDA module */
#elif ((0x00000000 == (RX_DEVICE_TYPE & 0x000000F0)) || (0x00000010 == (RX_DEVICE_TYPE & 0x000000F0)))
    /* With or without digital filters */
    #define DIGITAL_FILTER_ONOFF        (1U)    /* 0:Not used, 1:Use */

    #if (1 == DIGITAL_FILTER_ONOFF)
        /* Set sampling clock (Value to be set for LVD1CR0.BIT.LVD1FSAMP) */
        #define SAMPLING_CLOCK              (1U)    /* 1/4 LOCO frequency */

        /* Wait time setting value for at least 2n + 3 cycles of the LOCO */
        #define VOL_WAIT_CNT_LOCO           (1380U)
    #endif
#endif


/*----------------------------------------------*/
/*        Do not change below                   */
/*----------------------------------------------*/
/* Address setting for each register */
#define SYSTEM_PRCR_VOL_MON     (*(volatile struct st_system_prcr_vol_mon       __evenaccess *)0x803FE)
#define SYSTEM_LVCMPCR_VOL_MON  (*(volatile struct st_system_lvcmpcr_vol_mon    __evenaccess *)0x8C297)
#define SYSTEM_LVDLVLR_VOL_MON  (*(volatile struct st_system_lvdlvlr_vol_mon    __evenaccess *)0x8C298)
#define SYSTEM_LVD1CR0_VOL_MON  (*(volatile struct st_system_lvd1cr0_vol_mon    __evenaccess *)0x8C29A)
#define SYSTEM_LVD1CR1_VOL_MON  (*(volatile struct st_system_lvd1cr1_vol_mon    __evenaccess *)0x800E0)
#define SYSTEM_LVD1SR_VOL_MON   (*(volatile struct st_system_lvd1sr_vol_mon     __evenaccess *)0x800E1)
#define ICU_NMIER_VOL_MON       (*(volatile struct st_icu_nmier_vol_mon         __evenaccess *)0x87581)

#endif /* R_VOL_MON_CONFIG_H */
