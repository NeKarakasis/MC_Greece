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
* File Name   : r_motor_sensorless_vector_action.c
* Description : The implementations of action functions required by FOC state machine (r_motor_sensorless_vector_statemachine)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */

/* Project headers */
#include "r_motor_sensorless_vector_manager.h"
#include "r_motor_driver.h"

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_act_active
* Description   : Activates the motor control system and enables PWM output
* Arguments     : p_st_stm   - The pointer to the state machine structure
*                 p_param    - The pointer to the FOC structure
* Return Value  : The result of action, Always success(0)
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_act_active(st_statemachine_t *p_st_stm, void *p_param)
{
    st_sensorless_vector_control_t *p_st_sensorless_vector = p_param;
    (void)p_st_stm;                                                     /* This line is to avoid Warning due to unused parameter  */
    motor_sensorless_vector_reset(p_st_sensorless_vector);              /* Initialize variables when motor control start */
    R_MOTOR_DRIVER_BldcCompareDutySet(p_st_sensorless_vector->p_st_driver);

    R_MOTOR_DRIVER_BldcDutySet(p_st_sensorless_vector->p_st_driver, 0.5f, 0.5f, 0.5f);
    if (MTR_DISABLE == p_st_sensorless_vector->u1_flag_flying_start_use)
    {
        R_MOTOR_CURRENT_ParameterGet(p_st_sensorless_vector->p_st_cc, &p_st_sensorless_vector->st_current_output);
        if (MTR_FLG_SET == p_st_sensorless_vector->st_current_output.u1_flag_offset_calc)
        {
            R_MOTOR_DRIVER_PWMControlStart(p_st_sensorless_vector->p_st_driver); /* PWM output enable */
            motor_sensorless_vector_module_activate(p_st_sensorless_vector);
        }
    }
    return (0);
} /* End of function motor_sensorless_vector_act_active */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_act_inactive
* Description   : Deactivates the motor control system and disables PWM output
* Arguments     : p_st_stm   - The pointer to the state machine structure
*                 p_param    - The pointer to the FOC structure
* Return Value  : The result of action, Always success(0)
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_act_inactive(st_statemachine_t *p_st_stm, void *p_param)
{
    st_sensorless_vector_control_t *p_st_sensorless_vector = p_param;
    (void)p_st_stm;                                                       /* This line is to avoid Warning due to unused parameter  */
    R_MOTOR_DRIVER_PWMControlStop(p_st_sensorless_vector->p_st_driver);

    R_MOTOR_DRIVER_BldcDutySet(p_st_sensorless_vector->p_st_driver, 0.5f, 0.5f, 0.5f);

    motor_sensorless_vector_reset(p_st_sensorless_vector);                /* Initialize variables when motor control start */
    return (0);
} /* End of function motor_sensorless_vector_act_inactive */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_act_nowork
* Description   : The empty dummy function used to fill the blank in action table
* Arguments     : p_st_stm   - The pointer to the state machine structure
*                 p_param    - The pointer to the FOC structure
* Return Value  : Always success (0)
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_act_nowork(st_statemachine_t *p_st_stm, void *p_param)
{
    /* Do Nothing */
    (void)p_st_stm; /* This line is to avoid Warning due to unused parameter  */
    (void)p_param;  /* This line is to avoid Warning due to unused parameter  */
    return (0);
} /* End of function motor_sensorless_vector_act_nowork */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_act_reset
* Description   : Resets the configurations to default and clear error flags
* Arguments     : p_st_stm   - The pointer to the state machine structure
*                 p_param    - The pointer to the FOC structure
* Return Value  : The result of action, Always success(0)
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_act_reset(st_statemachine_t *p_st_stm, void *p_param)
{
    st_sensorless_vector_control_t *p_st_sensorless_vector = p_param;
    (void)p_st_stm; /* This line is to avoid Warning due to unused parameter  */
    motor_sensorless_vector_default_init(p_st_sensorless_vector);
    return (0);
} /* End of function motor_sensorless_vector_act_reset */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_act_error
* Description   : Executes the post-processing (include stopping the PWM output) when an error has been detected
* Arguments     : p_st_stm   - The pointer to the state machine structure
*                 p_param    - The pointer to the FOC structure
* Return Value  : The result of action, Always success(0)
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_act_error(st_statemachine_t *p_st_stm, void *p_param)
{
    st_sensorless_vector_control_t *p_st_sensorless_vector = p_param;
    (void)p_st_stm; /* This line is to avoid Warning due to unused parameter  */
    R_MOTOR_DRIVER_PWMControlStop(p_st_sensorless_vector->p_st_driver);
    return (0);
} /* End of function motor_sensorless_vector_act_error */
