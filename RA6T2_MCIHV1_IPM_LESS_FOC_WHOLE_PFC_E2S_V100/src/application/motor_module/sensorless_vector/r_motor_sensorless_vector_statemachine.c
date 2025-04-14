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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_sensorless_vector_statemachine.c
* Description : The state machine for motor drive system
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_common.h"
#include "r_motor_sensorless_vector_statemachine.h"

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static uint8_t s_state_transition_table[STATEMACHINE_EVENT_SIZE][STATEMACHINE_STATE_SIZE] = {
/* State                  0:STATEMACHINE_STATE_STOP, 1:STATEMACHINE_STATE_RUN, 2:STATEMACHINE_STATE_ERROR */
/* Event */
/* 0:STATEMACHINE_EVENT_STOP  */
    { STATEMACHINE_STATE_STOP,    STATEMACHINE_STATE_STOP,    STATEMACHINE_STATE_ERROR },
/* 1:STATEMACHINE_EVENT_RUN   */
    { STATEMACHINE_STATE_RUN,     STATEMACHINE_STATE_RUN,     STATEMACHINE_STATE_ERROR },
/* 2:STATEMACHINE_EVENT_ERROR */
    { STATEMACHINE_STATE_ERROR,   STATEMACHINE_STATE_ERROR,   STATEMACHINE_STATE_ERROR },
/* 3:STATEMACHINE_EVENT_RESET */
    { STATEMACHINE_STATE_STOP,    STATEMACHINE_STATE_ERROR,   STATEMACHINE_STATE_STOP  },};

static mtr_action_t s_action_table[STATEMACHINE_EVENT_SIZE][STATEMACHINE_STATE_SIZE] = {
/* State                  0:STATEMACHINE_STATE_STOP, 1:STATEMACHINE_STATE_RUN, 2:STATEMACHINE_STATE_ERROR */
/* Event */
/* 0:STATEMACHINE_EVENT_STOP  */
    { motor_sensorless_vector_act_inactive, motor_sensorless_vector_act_inactive, motor_sensorless_vector_act_nowork },
/* 1:STATEMACHINE_EVENT_RUN   */
    { motor_sensorless_vector_act_active,   motor_sensorless_vector_act_nowork,   motor_sensorless_vector_act_nowork },
/* 2:STATEMACHINE_EVENT_ERROR */
    { motor_sensorless_vector_act_error,    motor_sensorless_vector_act_error,    motor_sensorless_vector_act_nowork },
/* 3:STATEMACHINE_EVENT_RESET */
    { motor_sensorless_vector_act_reset,    motor_sensorless_vector_act_error,    motor_sensorless_vector_act_reset  },};

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_statemachine_init
* Description   : Initializes state machine for motor drive system
* Arguments     : p_st_stm   - the pointer to the state machine data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_statemachine_init(st_statemachine_t * p_st_stm)
{
    motor_sensorless_vector_statemachine_reset(p_st_stm);
} /* End of function motor_sensorless_vector_statemachine_init */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_statemachine_reset
* Description   : Resets state machine
* Arguments     : p_st_stm   - the pointer to the state machine data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_statemachine_reset(st_statemachine_t * p_st_stm)
{
    p_st_stm->u1_status          = STATEMACHINE_STATE_STOP;
    p_st_stm->u1_status_next     = STATEMACHINE_STATE_STOP;
    p_st_stm->u2_error_status    = STATEMACHINE_ERROR_NONE;
    p_st_stm->u1_current_event   = 0;
} /* End of function motor_sensorless_vector_statemachine_reset */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_statemachine_event
* Description   : Performs state transition and execute corresponding action when
*                 specified event happen
* Arguments     : p_st_stm     - the pointer to the state machine data structure
*                 p_object  - the pointer to parameters passed to the action handler
*                 u1_event  - the event index to be executed
*                       STATEMACHINE_EVENT_STOP      Stop the motor drive system
*                       STATEMACHINE_EVENT_RUN       Activate the motor drive system
*                       STATEMACHINE_EVENT_ERROR     Throw an error and stop driving
*                       STATEMACHINE_EVENT_RESET     Reset the configurations of motor drive system
* Return Value  : The error flags of the state machine
*                       BIT0: Event index is out of bound, please check the u1_event
*                       BIT1: State index is out of bound
*                       BIT2: Action error (value other than 0 returned from action)
***********************************************************************************************************************/
uint16_t motor_sensorless_vector_statemachine_event(st_statemachine_t * p_st_stm, void * p_object, uint8_t u1_event)
{
    mtr_action_t func_action;
    uint8_t      action_ret;

    p_st_stm->u2_error_status = STATEMACHINE_ERROR_NONE;

    /* Check if accessing state transition table out of bound */
    if (STATEMACHINE_EVENT_SIZE <= u1_event)
    {
        /* Event is out of bound */
        u1_event = STATEMACHINE_EVENT_ERROR;
        p_st_stm->u2_error_status |= STATEMACHINE_ERROR_EVENT_OUT_BOUND;
    }
    if (STATEMACHINE_STATE_SIZE <= p_st_stm->u1_status)
    {
        /* State is out of bound */
        u1_event = STATEMACHINE_EVENT_ERROR;
        p_st_stm->u1_status = STATEMACHINE_STATE_STOP;
        p_st_stm->u2_error_status |= STATEMACHINE_ERROR_STATE_OUT_BOUND;
    }

    /*
     * u1_current_event : Event happening
     * u1_status        : Current status
     * u1_status_next   : Status after action executed
     */
    p_st_stm->u1_current_event = u1_event;
    p_st_stm->u1_status_next   = s_state_transition_table[u1_event][p_st_stm->u1_status];

    /* Get action function from action table and execute action */
    func_action = s_action_table[u1_event][p_st_stm->u1_status];
    action_ret  = func_action(p_st_stm, p_object);

    /* If return value is not zero, set the Action Exception flag */
    if (action_ret != 0U)
    {
        p_st_stm->u2_error_status |= STATEMACHINE_ERROR_ACTION_EXCEPTION;
    }
    p_st_stm->u1_status = p_st_stm->u1_status_next;

    return (p_st_stm->u2_error_status);
} /* End of function motor_sensorless_vector_statemachine_event */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_statemachine_status_get
* Description   : Gets the status of system
* Arguments     : p_st_stm - the pointer to the state machine data structure
* Return Value  : Status of system
***********************************************************************************************************************/
uint8_t motor_sensorless_vector_statemachine_status_get(st_statemachine_t * p_st_stm)
{
    return (p_st_stm->u1_status);
} /* End of function motor_sensorless_vector_statemachine_status_get */
