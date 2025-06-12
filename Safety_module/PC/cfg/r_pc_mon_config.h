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
* File Name     : r_pc_mon_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for Program Counter Monitor
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_PC_MON_CONFIG_H
#define R_PC_MON_CONFIG_H

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Wait time for IWDT oscillation stabilization */
#define PC_MON_WAIT_CNT         (5700U)


/*----------------------------------------------*/
/*          Do not change below                 */
/*----------------------------------------------*/
/* Address setting for each register */
#define SYSTEM_ILOCOCR_PC_MON   (*(volatile struct st_system_ilococr_pc_mon __evenaccess *)0x80035)
#define SYSTEM_PRCR_PC_MON      (*(volatile struct st_system_prcr_pc_mon    __evenaccess *)0x803FE)

#define ICU_NMIER_PC_MON        (*(volatile struct st_icu_nmier_pc_mon      __evenaccess *)0x87581)

#define IWDT_IWDTRR_PC_MON      (*(volatile struct st_iwdt_iwdtrr_pc_mon    __evenaccess *)0x88030)
#define IWDT_IWDTCR_PC_MON      (*(volatile struct st_iwdt_iwdtcr_pc_mon    __evenaccess *)0x88032)
#define IWDT_IWDTSR_PC_MON      (*(volatile struct st_iwdt_iwdtsr_pc_mon    __evenaccess *)0x88034)
#define IWDT_IWDTRCR_PC_MON     (*(volatile struct st_iwdt_iwdtrcr_pc_mon   __evenaccess *)0x88036)

#endif /* R_PC_MON_CONFIG_H */
