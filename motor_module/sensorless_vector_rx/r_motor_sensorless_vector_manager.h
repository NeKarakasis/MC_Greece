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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_sensorless_vector_manager.h
* Description : Definitions of the state machine for motor drive system
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_SENSORLESS_VECTOR_MANAGER_H
#define R_MOTOR_SENSORLESS_VECTOR_MANAGER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

#include "r_motor_module_cfg.h"
#include "r_motor_sensorless_vector_api.h"
#include "r_motor_driver.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions for sequence control
***********************************************************************************************************************/
/* Run mode */
#define     MOTOR_MODE_INIT               (0x00)
#define     MOTOR_MODE_BOOT               (0x01)
#define     MOTOR_MODE_DRIVE              (0x02)
#define     MOTOR_MODE_ANALYSIS           (0x03)
#define     MOTOR_MODE_TUNE               (0x04)

/***********************************************************************************************************************
* Global structure / type definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_default_init
* Description   : Initializes motor drive modules with default configuration
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_default_init(st_sensorless_vector_control_t * p_st_sensorless_vector);

/**********************************************************************************************************************
* Function Name : motor_sensorless_vector_reset
* Description   : Resets motor drive modules, configurations will not be reset
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_reset(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_decide_direction
* Description   : Updates direction by sign of electrical speed
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_decide_direction(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_error_process
* Description   : Sets error flags and trigger error event when u2_error_flag is not zero
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 u2_error_flag - The error flags of the errors to report
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_error_process(st_sensorless_vector_control_t * p_st_sensorless_vector,
                                           uint16_t u2_error_flag);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_speed_status_select
* Description   : Performs state transition related to speed reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_speed_status_select(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_iq_status_select
* Description   : Performs state transition related to q-axis current reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_iq_status_select(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_id_status_select
* Description   : Performs state transition related to d-axis current reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_id_status_select(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_nominal_current_set
* Description   : Setup current related limit values by the nominal current of motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*               : f4_nominal_current_rms - The nominal current[Arms] of motor
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_nominal_current_set(st_sensorless_vector_control_t * p_st_sensorless_vector,
                                                 float f4_nominal_current_rms);

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_module_activate
* Description   : Activate modules
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_module_activate(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : motor_sensorless_phase_err_lpf_setup
* Description   : Setups LPF for estimated phase error by given cutoff frequency[Hz]
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 f4_phase_err_lpf_cut_freq - Cut off frequency[Hz] of phase error LPF
*                 f4_ctrl_period - Control period [s]
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_phase_err_lpf_setup(st_sensorless_vector_control_t *p_st_sensorless_vector,
                                          float f4_phase_err_lpf_cut_freq,
                                          float f4_ctrl_period);

#endif /* R_MOTOR_SENSORLESS_VECTOR_MANAGER_H */
