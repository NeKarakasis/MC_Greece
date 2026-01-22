/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : mtr_interrupt.c
* Description : interrupt functions.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_app_mcu.h"
#include "mtr_interrupt.h"
#include "r_system_manager_api.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : mtr_interrupt_current_cyclic
* Description   : Calls current control module cyclic and all other cyclic function synchronized with it.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_current_cyclic(void)
{
    R_SYSTEM_MANAGER_MotorCurrentInterrupt();
    r_app_rmw_interrupt_handler();
} /* End of function mtr_interrupt_current_cyclic */

/***********************************************************************************************************************
* Function Name : mtr_interrupt_speed_cyclic
* Description   : Calls speed control module cyclic and all other cyclic function synchronized with it.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_speed_cyclic(void)
{
    R_SYSTEM_MANAGER_MotorSpeedInterrupt();
} /* End of function mtr_interrupt_speed_cyclic */

/***********************************************************************************************************************
* Function Name : mtr_interrupt_system_manager_cyclic
* Description   : Calls system manager module cyclic and all other cyclic function synchronized with it.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_system_manager_cyclic(void)
{
    R_SYSTEM_MANAGER_ControlInterrupt();
} /* End of function mtr_interrupt_system_manager_cyclic */

/***********************************************************************************************************************
* Function Name : mtr_interrupt_hw_overcurrent
* Description   : Calls over-current control handlers belong to middle or upper layer.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_hw_overcurrent(void)
{
    R_SYSTEM_MANAGER_MotorOverCurrentInterrupt();
} /* End of function mtr_interrupt_hw_overcurrent */

/***********************************************************************************************************************
* Function Name : mtr_interrupt_pfc_current_cyclic
* Description   : Calls pfc current control module cyclic and all other cyclic function synchronized with it.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_pfc_current_cyclic(void)
{
    R_SYSTEM_MANAGER_PfcCurrentInterrupt();
} /* End of function mtr_interrupt_pfc_current_cyclic */

/***********************************************************************************************************************
* Function Name : pfc_interrupt_pfc_hw_overcurrent
* Description   : Calls pfc over-current control handlers belong to middle or upper layer.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_interrupt_pfc_hw_overcurrent(void)
{
    R_SYSTEM_MANAGER_PfcOverCurrentInterrupt();
} /* End of function mtr_interrupt_pfc_hw_overcurrent */

void pfc_pwm_cyclic(timer_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if( TIMER_EVENT_CREST == p_args->event)
    {
        R_Config_MC_PWM_PFC_DUTY_UpdDuty();
    }
}


