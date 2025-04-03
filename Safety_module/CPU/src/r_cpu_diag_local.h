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
* File Name     : r_cpu_diag_local.h
* Version       : 1.00a
* Device        : RX Family
* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
* Description   : CPU Test
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.06.2023 1.00    First Release
*         : 03.04.2024 1.00a   Added target device.
*******************************************************************************/

#ifndef R_CPU_DIAG_LOCAL_H
#define R_CPU_DIAG_LOCAL_H

/*******************************************************************************
* Macro definitions
*******************************************************************************/

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
/* CPU diagnostic (Register) */
extern void R_CPU_Diag7_1(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_2(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_3(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_4(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_5(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_6(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_7(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_8(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_9(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_10(const uint32_t forceFail, int32_t *result);

#if (RX_CPU_TYPE >= CPU_RXv2)
extern void R_CPU_Diag7_11(const uint32_t forceFail, int32_t *result);
#else
#define R_CPU_Diag7_11  norm_null
#endif

extern void R_CPU_Diag7_12(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag7_13(const uint32_t forceFail, int32_t *result);

#if (RX_CPU_TYPE == CPU_RXv3_DFPU)
extern void R_CPU_Diag11_1(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_2(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_3(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_4(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_5(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_6(const uint32_t forceFail, int32_t *result);
extern void R_CPU_Diag11_7(const uint32_t forceFail, int32_t *result);
#else
#define R_CPU_Diag11_1  norm_null
#define R_CPU_Diag11_2  norm_null
#define R_CPU_Diag11_3  norm_null
#define R_CPU_Diag11_4  norm_null
#define R_CPU_Diag11_5  norm_null
#define R_CPU_Diag11_6  norm_null
#define R_CPU_Diag11_7  norm_null
#endif


#endif  /* R_CPU_DIAG_LOCAL_H */
