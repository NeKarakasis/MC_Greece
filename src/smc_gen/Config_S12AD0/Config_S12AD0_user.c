/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_S12AD0_user.c
* Component Version: 2.5.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_S12AD0.
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
#include "Config_S12AD0.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_motor_sensorless_vector_api.h"
#include "r_app_rmw.h"
#include "safety_functions_api.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint16_t current_loop_ticks = 0;
uint16_t available_ticks = 0;
uint16_t current_loop_ticks_max = 0;
uint16_t MTU_point1,MTU_point2 = 0;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_S12AD0_Create_UserInit
* Description  : This function adds user code after initializing the S12AD0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* Re-initialise in Config_MOTOR */
    S12AD.ADCSR.WORD = _0000_MTR_AD_SINGLE_SCAN_MODE | _1000_MTR_AD_SCAN_END_INTERRUPT_ENABLE;
    S12AD.ADANSA0.WORD |= _0001_MTR_AD_AN000_USED | _0004_MTR_AD_AN002_USED | _0040_MTR_AD_AN006_USED;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_S12AD0_interrupt
* Description  : This function is S12ADI interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
/* Start user code for include. Do not edit comment generated here */
uint16_t pause_counter;
/* End user code. Do not edit comment generated here */

#if FAST_INTERRUPT_VECTOR == VECT_S12AD_S12ADI
#pragma interrupt r_Config_S12AD0_interrupt(vect=VECT(S12AD,S12ADI),fint)
#else
#pragma interrupt r_Config_S12AD0_interrupt(vect=VECT(S12AD,S12ADI))
#endif
static void r_Config_S12AD0_interrupt(void)
{
    /* Start user code for r_Config_S12AD0_interrupt. Do not edit comment generated here */
	MTU.TRWERA.BIT.RWE = 1U;
	int32_t mtu_counter = MTU4.TCNT;
	MTU_point1 = mtu_counter;
    R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt(&g_st_sensorless_vector);
    r_app_rmw_interrupt_handler();
    if (STATEMACHINE_STATE_RUN == motor_sensorless_vector_statemachine_status_get(&g_st_sensorless_vector.st_stm))
    {
    	if (pause_counter > 1000)
    	{
    MTU_point2 = MTU4.TCNT;
    //current_loop_ticks  = 5330 - mtu_counter - MTU4.TCNT;//5330 - (counter1 - 40) - counter2
    current_loop_ticks = 5330 - MTU_point2 - 0x40;
    if ((current_loop_ticks_max < current_loop_ticks) && (current_loop_ticks < 4000))
    {
    	current_loop_ticks_max = current_loop_ticks;
        available_ticks = MTU_point2 - 0x40;
    }
    MTU.TRWERA.BIT.RWE = 0U;
    clrpsw_i();
    FuSa_Manager_Run(&g_cfg, &g_st);
    setpsw_i();
    }
    	else
    	{
    		pause_counter ++;
    	}
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
