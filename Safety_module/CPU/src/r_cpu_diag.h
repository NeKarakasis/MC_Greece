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
* File Name     : r_cpu_diag.h
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

#ifndef R_CPU_DIAG_H
#define R_CPU_DIAG_H

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include "r_cpu_diag_config.h"
#include "r_cpu_diag_local.h"

/*******************************************************************************
* Macro definitions
*******************************************************************************/
/* Version Number of software */
#define CPU_DIAG_VERSION_MAJOR  (0x01)
#define CPU_DIAG_VERSION_MINOR  (0x00)

/* CPU diagnostic result */
#define CPU_INIT                (0xFFFFFFFFU)               /* Test Init */
#define CPU_FAIL                (0)                         /* Test Fail */
#define CPU_PASS                (1)                         /* Test Pass */

/* Max index */
#define CPU_DIAG_MAX_INDEX      (19)

/*******************************************************************************
* Exported global functions (to be accessed by other files)
*******************************************************************************/
uint32_t R_CPU_Diag_GetVersion(void);
void R_CPU_Diag(uint32_t index, const uint32_t forceFail, int32_t *result);

#endif /* R_CPU_DIAG_H */
