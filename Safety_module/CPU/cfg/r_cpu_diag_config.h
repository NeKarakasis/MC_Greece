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
* File Name     : r_cpu_diag_config.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : Configuration for CPU Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_CPU_DIAG_CONFIG_H
#define R_CPU_DIAG_CONFIG_H

/*******************************************************************************
* Macro definitions
*******************************************************************************/

/*----------------------------------------------*/
/*        Do not change below                   */
/*----------------------------------------------*/

/* ==== Define RX CPU type ==== */
#define CPU_RXv3_DFPU (4) /* RXv3 core with double precision FPU */
#define CPU_RXv3      (3) /* RXv3 core without double precision FPU */
#define CPU_RXv2      (2) /* RXv2 core */
#define CPU_RXv1_FPU  (1) /* RXv1 core with single precision FPU */
#define CPU_RXv1      (0) /* RXv1 core without single precision FPU */

#define REGBANK_AVAILABLE     (1) /* RXv3 core with register bank */
#define REGBANK_UNAVAILABLE   (0) /* RXv3 core without register bank */

#if   ((RX_DEVICE_TYPE & 0x33000000) == 0x10000000)
#define RX_CPU_TYPE             CPU_RXv1
#elif ((RX_DEVICE_TYPE & 0x33000000) == 0x11000000)
#define RX_CPU_TYPE             CPU_RXv1_FPU
#elif ((RX_DEVICE_TYPE & 0x33000000) == 0x21000000)
#define RX_CPU_TYPE             CPU_RXv2
#elif ((RX_DEVICE_TYPE & 0x33000000) == 0x31000000)
#define RX_CPU_TYPE             CPU_RXv3
#elif ((RX_DEVICE_TYPE & 0x33000000) == 0x33000000)
#define RX_CPU_TYPE             CPU_RXv3_DFPU
#else
#error "Not supported"
#endif

#if ((RX_DEVICE_TYPE & 0x34000000) == 0x34000000)
#define OPT_REGBANK   REGBANK_AVAILABLE
#else
#define OPT_REGBANK   REGBANK_UNAVAILABLE
#endif

#endif /* R_CPU_DIAG_CONFIG_H */
