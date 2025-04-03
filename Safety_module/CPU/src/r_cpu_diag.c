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
* File Name     : r_cpu_diag.c
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

/*******************************************************************************
* Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_cpu_diag.h"

/*******************************************************************************
* Section definitions
*******************************************************************************/
#pragma section P RX_DSW_P
#pragma section C RX_DSW_C

/*******************************************************************************
* Typedef definitions
*******************************************************************************/
typedef void (*Func)(const uint32_t forceFail, int32_t *result);

/*******************************************************************************
* Private global variables and functions
*******************************************************************************/
static void norm_null(const uint32_t forceFail, int32_t *result);

static const Func norm_func[CPU_DIAG_MAX_INDEX + 1] = {
    /* CPU diagnostic (Register) */
    R_CPU_Diag7_1,
    R_CPU_Diag7_2,
    R_CPU_Diag7_3,
    R_CPU_Diag7_4,
    R_CPU_Diag7_5,
    R_CPU_Diag7_6,
    R_CPU_Diag7_7,
    R_CPU_Diag7_8,
    R_CPU_Diag7_9,
    R_CPU_Diag7_10,
    R_CPU_Diag7_11,
    R_CPU_Diag7_12,
    R_CPU_Diag7_13,
    R_CPU_Diag11_1,
    R_CPU_Diag11_2,
    R_CPU_Diag11_3,
    R_CPU_Diag11_4,
    R_CPU_Diag11_5,
    R_CPU_Diag11_6,
    R_CPU_Diag11_7,
};

static const uint32_t RCpuDiagVer = ((uint32_t)CPU_DIAG_VERSION_MAJOR << 16u) | (uint16_t)CPU_DIAG_VERSION_MINOR;

/*******************************************************************************
* Function Name: R_CPU_Diag_GetVersion
* Description  : Get version number of CPU diagnostic module.
* Arguments    : None
* Return Value : Version number
* Note         : None
*******************************************************************************/
uint32_t R_CPU_Diag_GetVersion(void)
{
    return RCpuDiagVer;
} /* End of function R_CPU_Diag_GetVersion() */

/*******************************************************************************
* Function Name: R_CPU_Diag
* Description  : CPU core diagnostic operation.
* Arguments    : index           - Specify executing function
*                forceFail       - Forcing fail flag
*                result          - Result of diagnosis
* Return Value : None
* Note         : This function applies without lookup table.
*******************************************************************************/
void R_CPU_Diag(uint32_t index, const uint32_t forceFail, int32_t *result)
{
    *result = (int32_t)CPU_INIT;

    if (index <= (uint32_t)CPU_DIAG_MAX_INDEX)
    {
        norm_func[index](forceFail, result);
    }
    else
    {
        /* Do nothing */
    }

    if (CPU_PASS != *result)
    {
        *result = (int32_t)CPU_FAIL;
    }
    else
    {
        /* Do nothing */
    }

    return;

} /* End of R_CPU_Diag() */

/*******************************************************************************
* Function Name: norm_null
* Description  : CPU core diagnostic operation. (Empty)
* Arguments    : forceFail       - Forcing fail flag
*                result          - Result of diagnosis
* Return Value : None
* Note         : This function is called only when no function implements.
*******************************************************************************/
static void norm_null(const uint32_t forceFail, int32_t *result)
{
    (void)forceFail;
    *result = (int32_t)CPU_PASS;

    return;

} /* End of norm_null() */

/* End of File */
