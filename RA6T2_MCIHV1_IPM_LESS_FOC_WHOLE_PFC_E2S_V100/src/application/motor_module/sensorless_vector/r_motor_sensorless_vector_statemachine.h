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
* File Name   : r_motor_sensorless_vector_statemachine.h
* Description : Definitions of the state machine for motor drive system
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_SENSORLESS_VECTOR_STATEMACHINE_H
#define R_MOTOR_SENSORLESS_VECTOR_STATEMACHINE_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* System modes (state) */
#define     STATEMACHINE_STATE_STOP                       (0x00)  /* State of motor stop */
#define     STATEMACHINE_STATE_RUN                        (0x01)  /* State of motor run */
#define     STATEMACHINE_STATE_ERROR                      (0x02)  /* State of motor error */
#define     STATEMACHINE_STATE_SIZE                       (3)     /* Number of states */

/* System events */
#define     STATEMACHINE_EVENT_STOP                       (0x00)  /* Event of motor stop */
#define     STATEMACHINE_EVENT_RUN                        (0x01)  /* Event of motor run */
#define     STATEMACHINE_EVENT_ERROR                      (0x02)  /* Event of motor error */
#define     STATEMACHINE_EVENT_RESET                      (0x03)  /* Event of motor reset */
#define     STATEMACHINE_EVENT_SIZE                       (4)     /* Number of events */

/* State machine error flags */
#define     STATEMACHINE_ERROR_NONE                       (0x00)  /* None error flag */
#define     STATEMACHINE_ERROR_EVENT_OUT_BOUND            (0x01)  /* The event index is out of bound */
#define     STATEMACHINE_ERROR_STATE_OUT_BOUND            (0x02)  /* The state index is out of bound */
#define     STATEMACHINE_ERROR_ACTION_EXCEPTION           (0x04)  /* The action function returns failure */

/***********************************************************************************************************************
* Global structure / type definitions
***********************************************************************************************************************/
/* State machine data structure */
typedef struct
{
    uint8_t       u1_status;                        /* The current system status */
    uint8_t       u1_status_next;                   /* The next system status */
    uint16_t      u2_error_status;                  /* The error flags of state machine */
    uint8_t       u1_current_event;                 /* The current event index */
} st_statemachine_t;

/*
 * State machine action handler function definition
 * Arguments :  p_st_stm - the pointer to the state machine structure
 *              p_param  - the pointer to the parameters
 * Return    :  the result of action
 *              0 - Success
 *              else - Error
 */
typedef uint8_t (*mtr_action_t)(st_statemachine_t *p_st_stm, void *p_param);

/* Declare prototypes for action functions defined in upper layer */
extern uint8_t   motor_sensorless_vector_act_active(st_statemachine_t *p_st_stm, void *p_param);
extern uint8_t   motor_sensorless_vector_act_inactive(st_statemachine_t *p_st_stm, void *p_param);
extern uint8_t   motor_sensorless_vector_act_reset(st_statemachine_t *p_st_stm, void *p_param);
extern uint8_t   motor_sensorless_vector_act_error(st_statemachine_t *p_st_stm, void *p_param);
extern uint8_t   motor_sensorless_vector_act_nowork(st_statemachine_t *p_st_stm, void *p_param);

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/

void motor_sensorless_vector_statemachine_init(st_statemachine_t *p_st_stm);
void motor_sensorless_vector_statemachine_reset(st_statemachine_t *p_st_stm);
uint16_t motor_sensorless_vector_statemachine_event(st_statemachine_t *p_st_stm, void *p_object, uint8_t u1_event);
uint8_t motor_sensorless_vector_statemachine_status_get(st_statemachine_t *p_st_stm);
#endif /* R_MOTOR_SENSORLESS_VECTOR_STATEMACHINE_H */
