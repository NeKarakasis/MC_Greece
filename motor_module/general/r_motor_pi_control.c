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
* File Name   : r_motor_pi_control.c
* Description : The general purpose PI control modules
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.06.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include <math.h>
#include "r_motor_filter.h"
#include "r_motor_pi_control.h"

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl
* Description   : PI control
* Arguments     : p_st_pi_ctrl - The pointer to the PI control structure
* Return Value  : PI control output value
***********************************************************************************************************************/
float motor_pi_ctrl(st_pi_ctrl_t *p_st_pi_ctrl)
{
    float f4_err;
    float f4_kp;
    float f4_ki;
    float f4_refp;
    float f4_refi;
    float f4_ref;
    float f4_ilimit;

    f4_err    = p_st_pi_ctrl->f4_err;
    f4_kp     = p_st_pi_ctrl->f4_kp;
    f4_ki     = p_st_pi_ctrl->f4_ki;
    f4_refi   = p_st_pi_ctrl->f4_refi;
    f4_ilimit = p_st_pi_ctrl->f4_ilimit;

    f4_refp = f4_err * f4_kp;                      /* Proportional part */
    f4_refi += (f4_err * f4_ki);                   /* Integral part */

    /*** Integral part limit ***/
    f4_refi = motor_filter_limitf_abs(f4_refi, f4_ilimit);
    p_st_pi_ctrl->f4_refi = f4_refi;

    f4_ref = f4_refp + f4_refi;                    /* PI output */
    f4_ref = motor_filter_limitf_abs(f4_ref, f4_ilimit);

    return (f4_ref);
} /* End of function motor_pi_ctrl */

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl_reset
* Description   : Resets PI control
* Arguments     : p_st_pi_ctrl - The pointer to the PI control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_pi_ctrl_reset(st_pi_ctrl_t *p_st_pi_ctrl)
{
    p_st_pi_ctrl->f4_refi = 0.0f;
} /* End of function motor_pi_ctrl_reset */

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl_kp_set
* Description   : Sets proportional gain of PI control
* Arguments     : p_st_pi_ctrl - The pointer to the PI control structure
*               : f4_kp        - The proportional gain to set
* Return Value  : None
***********************************************************************************************************************/
void motor_pi_ctrl_kp_set(st_pi_ctrl_t *p_st_pi_ctrl, float f4_kp)
{
    p_st_pi_ctrl->f4_kp = f4_kp;
} /* End of function motor_pi_ctrl_kp_set */

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl_set_ki
* Description   : Sets integral gain of PI control
* Arguments     : p_st_pi_ctrl - The pointer to the PI control structure
*               : f4_ki        - The integral gain to set
* Return Value  : None
***********************************************************************************************************************/
void motor_pi_ctrl_set_ki(st_pi_ctrl_t *p_st_pi_ctrl, float f4_ki)
{
    p_st_pi_ctrl->f4_ki = f4_ki;
} /* End of function motor_pi_ctrl_set_ki */

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl_integrator_set
* Description   : Sets value integrator of PI control directly
* Arguments     : p_st_pi_ctrl  - The pointer to the PI control structure
*               : f4_integrator - The new value for integrator
* Return Value  : None
***********************************************************************************************************************/
void motor_pi_ctrl_integrator_set(st_pi_ctrl_t *p_st_pi_ctrl, float f4_integrator)
{
    p_st_pi_ctrl->f4_refi = f4_integrator;
} /* End of function motor_pi_ctrl_integrator_set */

/***********************************************************************************************************************
* Function Name : motor_pi_ctrl_integral_limit_set
* Description   : Sets integral limit of PI control
* Arguments     : p_st_pi_ctrl - The pointer to the PI control structure
*               : f4_ilimit    - The integral limit value to set
* Return Value  : None
***********************************************************************************************************************/
void motor_pi_ctrl_integral_limit_set(st_pi_ctrl_t *p_st_pi_ctrl, float f4_ilimit)
{
    p_st_pi_ctrl->f4_ilimit = f4_ilimit;
} /* End of function motor_pi_ctrl_integral_limit_set */
