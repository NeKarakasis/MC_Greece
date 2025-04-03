;/*******************************************************************************
;* DISCLAIMER
;* This software is supplied by Renesas Electronics Corporation and is only
;* intended for use with Renesas products. No other uses are authorized. This
;* software is owned by Renesas Electronics Corporation and is protected under
;* all applicable laws, including copyright laws.
;* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
;* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
;* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
;* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
;* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
;* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
;* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
;* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
;* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;* Renesas reserves the right, without notice, to make changes to this software
;* and to discontinue the availability of this software. By using this software,
;* you agree to the additional terms and conditions found by accessing the
;* following link:
;* http://www.renesas.com/disclaimer
;*******************************************************************************/
;/* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.   */
;/*******************************************************************************
;* File Name     : r_cpu_diag_config.inc
;* Version       : 1.00a
;* Device        : RX Family
;* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
;* Description   : Configuration for CPU Test
;*******************************************************************************/
;/*******************************************************************************
;* History : DD.MM.YYYY Version Description
;*         : 09.06.2023 1.00    First Release
;*         : 03.04.2024 1.00a   Added target device.
;*******************************************************************************/

;/*----------------------------------------------*/
;/*        Do not change below                   */
;/*----------------------------------------------*/
; ==== Define RX CPU type ==== ;
CPU_RXv3_DFPU               .EQU        4   ; RXv3 core with double precision FPU
CPU_RXv3                    .EQU        3   ; RXv3 core without double precision FPU
CPU_RXv2                    .EQU        2   ; RXv2 core
CPU_RXv1_FPU                .EQU        1   ; RXv1 core with single precision FPU
CPU_RXv1                    .EQU        0   ; RXv1 core without single precision FPU

REGBANK_AVAILABLE           .EQU        1   ; RXv3 core with register bank
REGBANK_UNAVAILABLE         .EQU        0   ; RXv3 core without register bank

.IF   (RX_DEVICE_TYPE & 33000000H) == 10000000H
RX_CPU_TYPE                 .EQU        CPU_RXv1
.ELIF (RX_DEVICE_TYPE & 33000000H) == 11000000H
RX_CPU_TYPE                 .EQU        CPU_RXv1_FPU
.ELIF (RX_DEVICE_TYPE & 33000000H) == 21000000H
RX_CPU_TYPE                 .EQU        CPU_RXv2
.ELIF (RX_DEVICE_TYPE & 33000000H) == 31000000H
RX_CPU_TYPE                 .EQU        CPU_RXv3
.ELIF (RX_DEVICE_TYPE & 33000000H) == 33000000H
RX_CPU_TYPE                 .EQU        CPU_RXv3_DFPU
.ELSE
.ASSERT "Not supported!"
.ENDIF

.IF   (RX_DEVICE_TYPE & 34000000H) == 34000000H
OPT_REGBANK                 .EQU        REGBANK_AVAILABLE
.ELSE
OPT_REGBANK                 .EQU        REGBANK_UNAVAILABLE
.ENDIF
