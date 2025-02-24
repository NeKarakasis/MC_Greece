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
* File Name   : r_motor_speed_pi_gain_calc.c
* Description : The processes of speed pi control gain designing
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
#include "r_motor_speed.h"

/***********************************************************************************************************************
* Function Name : motor_speed_pi_gain_calc
* Description   : Designs speed pi control gain by given parameters
* Arguments     : p_st_sc     - The pointer to the speed control structure
*                 f4_omega_hz - natural frequency for speed loop
*                 f4_zeta     - damping ratio for speed loop
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_pi_gain_calc(st_speed_control_t * p_st_sc, float f4_omega_hz, float f4_zeta)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;
    float f4_j = p_st_sc->st_motor.f4_mtr_j;
    float f4_pp = p_st_sc->st_motor.u2_mtr_pp;
    float f4_m = p_st_sc->st_motor.f4_mtr_m;

    /* Speed PI gain */
    f4_temp0 = 2.0f * f4_zeta;
    f4_temp1 = MTR_TWOPI * f4_omega_hz;
    f4_temp2 = (f4_temp0 * f4_temp1) * f4_j;
    f4_temp0 = (f4_temp1 * f4_temp1) * f4_j;
    f4_temp0 = f4_temp0  * p_st_sc->f4_speed_ctrl_period;
    f4_temp1 = f4_pp * f4_m;
    p_st_sc->st_pi_speed.f4_kp = f4_temp2 / f4_temp1;
    p_st_sc->st_pi_speed.f4_ki = f4_temp0 / f4_temp1;
} /* End of function motor_speed_pi_gain_calc */
