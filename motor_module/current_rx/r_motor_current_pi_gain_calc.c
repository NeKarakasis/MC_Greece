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
* File Name   : r_motor_current_pi_gain_calc.c
* Description : The processes of current pi control gain designing
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 11.06.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <math.h>
#include "r_motor_common.h"
#include "r_motor_current_api.h"

/***********************************************************************************************************************
* Function Name : motor_current_pi_gain_calc
* Description   : Designs current pi control gain by given parameters
* Arguments     : p_st_cc     - The pointer to the current control structure
*                 f4_omega_hz - natural frequency for current loop
*                 f4_zeta     - damping ratio for current loop
* Return Value  : None
***********************************************************************************************************************/
void motor_current_pi_gain_calc(st_current_control_t *p_st_cc, float f4_omega_hz, float f4_zeta)
{
    float f4_temp0;
    float f4_temp1;
    float f4_ld = p_st_cc->st_motor.f4_mtr_ld;
    float f4_lq = p_st_cc->st_motor.f4_mtr_lq;
    float f4_r = p_st_cc->st_motor.f4_mtr_r;

    /* D-axis q-axis current PI gain */
    f4_temp0 = 2.0f * f4_zeta;
    f4_temp1 = MTR_TWOPI * f4_omega_hz;
    f4_temp0 = f4_temp0 * f4_temp1;
    f4_temp1 = (f4_temp1 * f4_temp1) * p_st_cc->f4_ctrl_period;

    p_st_cc->st_pi_id.f4_kp = (f4_temp0 * f4_ld) - f4_r;
    p_st_cc->st_pi_id.f4_ki = f4_temp1 * f4_ld;
    p_st_cc->st_pi_iq.f4_kp = (f4_temp0 * f4_lq) - f4_r;
    p_st_cc->st_pi_iq.f4_ki = f4_temp1 * f4_lq;
} /* End of function motor_current_pi_gain_calc */
