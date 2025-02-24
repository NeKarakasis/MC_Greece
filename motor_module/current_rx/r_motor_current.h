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
/**********************************************************************************************************************
* File Name   : r_motor_current.h
* Description : Definitions of FOC current control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.06.2021 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_H
#define R_MOTOR_CURRENT_H

/**********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_pi_control.h"
#include "r_motor_current_api.h"

/**********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/**********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_current_decoupling
* Description   : Decoupling control, cancels the interference voltage due to the magnet and inductance
* Arguments     : p_st_cc      - The pointer to the current control structure
*                 f4_speed_rad - The electrical speed [rad/s]
*                 p_st_motor   - The pointer to the motor parameter data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_decoupling(st_current_control_t *p_st_cc,
                              float f4_speed_rad,
                              const st_motor_parameter_t *p_st_motor);

/***********************************************************************************************************************
* Function Name : motor_current_pi_control
* Description   : Current PI control
*                 Calculates the output voltage vector from current vector command and actual current vector
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_pi_control(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_init
* Description   : Initializes and resets rotor angle structure
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_init(st_rotor_angle_t *p_st_angle);

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_reset
* Description   : Resets the angle to 0 degree and updates related trigonometric values
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_reset(st_rotor_angle_t *p_st_angle);

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_update
* Description   : Updates the angle and related trigonometric values
* Arguments     : p_st_angle   - The pointer to the rotor angle structure
*               : f4_angle_rad - The angle[rad] used to update the structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_update(st_rotor_angle_t *p_st_angle, float f4_angle_rad);

/***********************************************************************************************************************
* Function Name : motor_current_transform_uvw_dq_abs
* Description   : Coordinate transform UVW to dq (absolute transform);
* Arguments     : p_st_angle - the pointer to rotor angle structure
*                 f4_uvw     - the pointer to the UVW-phase array in [U,V,W] format
*                 f4_dq      - where to store the [d,q] formated array on dq coordinates
* Return Value  : None
***********************************************************************************************************************/
void motor_current_transform_uvw_dq_abs(const st_rotor_angle_t *p_st_angle, const float *f4_uvw, float *f4_dq);

/***********************************************************************************************************************
* Function Name : motor_current_transform_dq_uvw_abs
* Description   : Coordinate transform dq to UVW 3-phase (absolute transform);
* Arguments     : p_st_angle - the pointer to rotor angle structure
*                 f4_dq      - the pointer to the dq-axis value array in [D,Q] format
*                 f4_uvw     - where to store the [U,V,W] formated 3-phase quantities array
* Return Value  : None
***********************************************************************************************************************/
void motor_current_transform_dq_uvw_abs(const st_rotor_angle_t *p_st_angle, const float *f4_dq, float *f4_uvw);

/***********************************************************************************************************************
* Function Name : motor_current_volt_limit
* Description   : Limit voltage vector, d-axis voltage has higher priority than q-axis voltage
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_volt_limit(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : motor_current_reference_iq_set
* Description   : Updates the q-axis current reference
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : Iq reference
***********************************************************************************************************************/
float motor_current_reference_iq_set(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : motor_current_reference_id_set
* Description   : Updates the d-axis current reference
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : Id reference
***********************************************************************************************************************/
float motor_current_reference_id_set(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : motor_current_angle_speed_detect
* Description   : Angle and speed detection
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_angle_speed_detect(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : motor_current_angle_get
* Description   : Gets the electrical angle of rotor [rad]
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : The electrical angle of rotor [rad]
***********************************************************************************************************************/
float motor_current_angle_get(st_rotor_angle_t *p_st_angle);

/**********************************************************************************************************************
* Function Name : motor_current_pi_gain_calc
* Description   : Designs current pi control gain by given parameters
* Arguments     : p_st_cc     - The pointer to the current control structure
*                 f4_omega_hz - natural frequency for current loop
*                 f4_zeta     - damping ratio for current loop
* Return Value  : None
***********************************************************************************************************************/
void motor_current_pi_gain_calc(st_current_control_t *p_st_cc, float f4_omega_hz, float f4_zeta);

#endif /* R_MOTOR_CURRENT_H */
