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
* File Name     : r_clock_mon_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for Clock Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_CLOCK_MON_CONFIG_H
#define R_CLOCK_MON_CONFIG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_clock_mon.h"

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Clock mon interrupt priority level setting (CAC Error interrupt) */
#define CLOCK_MON_INTERRUPT_PRIORITY    (15U)

/* Wait time for IWDT oscillation stabilization */
#define CLOCK_MON_WAIT_CNT          (5700U)


/*----------------------------------------------*/
/*          Do not change below                 */
/*----------------------------------------------*/
/* Address setting for each register */
#define SYSTEM_MSTPCRC_CLOCK_MON    (*(volatile struct st_system_mstpcrc_clock_mon  __evenaccess *)0x80018)
#define SYSTEM_ILOCOCR_CLOCK_MON    (*(volatile struct st_system_ilococr_clock_mon  __evenaccess *)0x80035)
#define SYSTEM_PRCR_CLOCK_MON       (*(volatile struct st_system_prcr_clock_mon     __evenaccess *)0x803FE)

#define CAC_CACR0_CLOCK_MON         (*(volatile struct st_cac_cacr0_clock_mon       __evenaccess *)0x8B000)
#define CAC_CACR1_CLOCK_MON         (*(volatile struct st_cac_cacr1_clock_mon       __evenaccess *)0x8B001)
#define CAC_CACR2_CLOCK_MON         (*(volatile struct st_cac_cacr2_clock_mon       __evenaccess *)0x8B002)
#define CAC_CAICR_CLOCK_MON         (*(volatile struct st_cac_caicr_clock_mon       __evenaccess *)0x8B003)
#define CAC_CAULVR_CLOCK_MON        (*(volatile struct st_cac_caulvr_clock_mon      __evenaccess *)0x8B006)
#define CAC_CALLVR_CLOCK_MON        (*(volatile struct st_cac_callvr_clock_mon      __evenaccess *)0x8B008)

#if (1 == CAC_GROUP_INT)
#define ICU_IER0D_CLOCK_MON         (*(volatile struct st_icu_ier0d_clock_mon       __evenaccess *)0x8720D)
#define ICU_IPR110_CLOCK_MON        (*(volatile struct st_icu_ipr110_clock_mon      __evenaccess *)0x8736E)
#define ICU_GENBL0_CLOCK_MON        (*(volatile struct st_icu_genbl0_clock_mon      __evenaccess *)0x87670)
#else
#define ICU_IER04_CLOCK_MON         (*(volatile struct st_icu_ier04_clock_mon       __evenaccess *)0x87204)
#define ICU_IPR32_CLOCK_MON         (*(volatile struct st_icu_ipr32_clock_mon       __evenaccess *)0x87320)
#define ICU_IPR34_CLOCK_MON         (*(volatile struct st_icu_ipr34_clock_mon       __evenaccess *)0x87322)
#endif

#endif /* R_CLOCK_MON_CONFIG_H */
