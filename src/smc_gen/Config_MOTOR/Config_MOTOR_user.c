/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MOTOR_user.c
* Component Version: 1.1.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_MOTOR.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_MOTOR.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_motor_sensorless_vector_api.h"
#include "r_app_rmw.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_Create_UserInit
* Description  : This function adds user code after initializing the MTU channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* Re-initialise in Config_S12AD0 */
    //S12AD.ADANSA0.WORD |= _0080_AD_ANx07_USED;
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
