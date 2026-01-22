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
* File Name   : r_app_mcu_callbacks.c
* Description : Generated FSP callback functions.
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

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : callback_gpt_adc_cyclic
* Description   : A/D conversion completion interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_gpt_adc_cyclic(adc_callback_args_t *p_args)
{
    if (ADC_EVENT_SCAN_COMPLETE == p_args->event)
    {
        switch (p_args->group_mask)
        {
            case ADC_GROUP_MASK_0:
                /* Motor control cyclic */
                mtr_interrupt_current_cyclic();
            break;

            case ADC_GROUP_MASK_1:
                /* PFC control cyclic */
                //mtr_interrupt_pfc_current_cyclic();
            break;

            default:
                /* Do nothing */
            break;
        }
    }
} /* End of function callback_gpt_adc_cyclic */

/***********************************************************************************************************************
* Function Name : callback_agt_motor_speed_cyclic
* Description   : Speed control cycle interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_agt_motor_speed_cyclic(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        mtr_interrupt_speed_cyclic();
    }
} /* End of function callback_agt_motor_speed_cyclic */

/***********************************************************************************************************************
* Function Name : callback_agt_system_manager_cyclic
* Description   : System manager control cycle interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_agt_system_manager_cyclic(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        mtr_interrupt_system_manager_cyclic();

    }
} /* End of function callback_agt_system_manager_cyclic */


/***********************************************************************************************************************
* Function Name : callback_poe_overcurrent
* Description   : POEG3 Interrupt callback function
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_poe_overcurrent(poeg_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        r_app_pwm_highz_reset();
        mtr_interrupt_hw_overcurrent();
    }
} /* End of function callback_poe_overcurrent */
