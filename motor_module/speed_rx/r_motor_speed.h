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
* File Name   : r_motor_speed.h
* Description : Definitions of speed control processes
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 10.06.2021 1.00
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_H
#define R_MOTOR_SPEED_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_speed_api.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_speed_max_speed_set
* Description   : Sets parameter for maximum speed
* Arguments     : p_st_sc          - The pointer to speed control structure
*                 f4_max_speed_rpm - maximum speed
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_max_speed_set(st_speed_control_t * p_st_sc, float f4_max_speed_rpm);

/***********************************************************************************************************************
* Function Name : motor_speed_rate_limit_set
* Description   : Sets parameter for speed change limit
* Arguments     : p_st_sc                 - The pointer to speed control structure
*                 f4_speed_rate_limit_rpm - speed ramping limit[rpm/s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_rate_limit_set(st_speed_control_t * p_st_sc, float f4_speed_rate_limit_rpm);

/***********************************************************************************************************************
* Function Name : motor_speed_rate_limit_apply
* Description   : Limits the rate of change of speed reference
* Arguments     : p_st_sc          - The pointer to speed control structure
* Return Value  : Limited speed reference
***********************************************************************************************************************/
float motor_speed_rate_limit_apply(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : motor_speed_pi_control
* Description   : Speed PI control
* Arguments     : p_st_sc      - The pointer to speed control structure
*                 f4_speed_rad - The electrical speed [rad/s]
* Return Value  : The Iq reference
***********************************************************************************************************************/
float motor_speed_pi_control(st_speed_control_t * p_st_sc, float f4_speed_rad);

/***********************************************************************************************************************
* Function Name : motor_speed_ref_speed_set
* Description   : Updates the speed reference
* Arguments     : p_st_sc     - The pointer to speed control structure
* Return Value  : Speed reference
***********************************************************************************************************************/
float motor_speed_ref_speed_set(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : motor_speed_flux_weakening
* Description   : Executes the flux-weakening
* Arguments     : p_st_sc     - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_flux_weakening(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : motor_speed_pi_gain_calc
* Description   : Designs speed pi control gain by given parameters
* Arguments     : p_st_sc     - The pointer to the speed control structure
*                 f4_omega_hz - natural frequency for speed loop
*                 f4_zeta     - damping ratio for speed loop
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_pi_gain_calc(st_speed_control_t * p_st_sc, float f4_omega_hz, float f4_zeta);

void motor_speed_mtpa(st_speed_control_t * p_st_sc);


#endif /* R_MOTOR_SPEED_H */
