/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_CMT0_user.c
* Component Version: 2.3.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_CMT0.
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
#include "Config_CMT0.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_motor_sensorless_vector_api.h"
#include "r_pc_mon.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
static uint16_t g_watchdog_refresh_counter = 0;
uint16_t speed_loop_ticks = 0;
uint16_t speed_loop_ticks_max = 0;
uint16_t CMT_point1 = 0;
extern uint16_t motor_restart_delay_counter;
extern uint8_t motor_restart_delay_activated;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_CMT0_Create_UserInit
* Description  : This function adds user code after initializing the CMT0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_CMT0_cmi0_interrupt
* Description  : This function is CMI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_CMT0_CMI0
#pragma interrupt r_Config_CMT0_cmi0_interrupt(vect=VECT(CMT0,CMI0),fint)
#else
#pragma interrupt r_Config_CMT0_cmi0_interrupt(vect=VECT(CMT0,CMI0))
#endif
static void r_Config_CMT0_cmi0_interrupt(void)
{
    /* Start user code for r_Config_CMT0_cmi0_interrupt. Do not edit comment generated here */
    CMT_point1 = CMT0.CMCNT;
	g_watchdog_refresh_counter ++;
	if (g_watchdog_refresh_counter > 500) // it is about 50% of watchdog timer
	{
		FuSa_PC_refresh();
		g_watchdog_refresh_counter = 0;
	}
	R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt(&g_st_sensorless_vector);
	if (STATEMACHINE_STATE_RUN == motor_sensorless_vector_statemachine_status_get(&g_st_sensorless_vector.st_stm))
	    {
	    speed_loop_ticks  =  CMT0.CMCNT - CMT_point1;

	    if ((speed_loop_ticks_max < speed_loop_ticks))
	    {
	    	speed_loop_ticks_max = speed_loop_ticks;
	    }
	    }
	if(motor_restart_delay_activated)
	{
		motor_restart_delay_counter++;
	}
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
