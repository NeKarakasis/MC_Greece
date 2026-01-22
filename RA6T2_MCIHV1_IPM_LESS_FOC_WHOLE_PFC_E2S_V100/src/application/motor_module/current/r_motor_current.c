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
* File Name   : r_motor_current.c
* Description : The current control module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <stdint.h>
#include <math.h>

/* Project headers */
#include "r_motor_module_cfg.h"
#include "r_motor_common.h"
#include "r_motor_filter.h"
#include "r_motor_current.h"
#include "r_motor_driver.h"

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
                              const st_motor_parameter_t *p_st_motor)
{
    float f4_temp0;
    float f4_temp1;
    float f4_id;
    float f4_iq;

    f4_id = p_st_cc->f4_id_ad;
    f4_iq = p_st_cc->f4_iq_ad;

    f4_temp0 = p_st_motor->f4_mtr_lq * f4_iq;             /* Lq * Iq */
    f4_temp0 = (- f4_speed_rad) * f4_temp0;               /* Speed * Lq * Iq */
    f4_temp0 = f4_temp0 + (p_st_motor->f4_mtr_r * f4_id); /* + R * Id */

    f4_temp1 = p_st_motor->f4_mtr_ld * f4_id;             /* Ld * Id */
    f4_temp1 = f4_temp1 + p_st_motor->f4_mtr_m;           /* Ld * Id + Flux */
    f4_temp1 = f4_speed_rad * f4_temp1;                   /* Speed * (Ld * Id + Flux) */
    f4_temp1 = f4_temp1 + (p_st_motor->f4_mtr_r * f4_iq); /* + R * Iq */

    p_st_cc->f4_vd_ref += f4_temp0;                       /* Vd + Speed * Lq * Iq + R * Id */
    p_st_cc->f4_vq_ref += f4_temp1;                       /* Vq + Speed * (Ld * Id + Flux) + R * Iq */
} /* End of function motor_current_decoupling */

/***********************************************************************************************************************
* Function Name : motor_current_pi_control
* Description   : Current PI control
*                 Calculates the output voltage vector from current vector command and actual current vector
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_pi_control(st_current_control_t *p_st_cc)
{
    /* The d-axis */
    p_st_cc->st_pi_id.f4_err = p_st_cc->f4_ref_id_ctrl - p_st_cc->f4_id_ad;
    p_st_cc->f4_vd_ref       = motor_pi_ctrl(&(p_st_cc->st_pi_id));

    /* The q-axis */
    p_st_cc->st_pi_iq.f4_err = p_st_cc->f4_ref_iq_ctrl - p_st_cc->f4_iq_ad;
    p_st_cc->f4_vq_ref       = motor_pi_ctrl(&(p_st_cc->st_pi_iq));
} /* End of function motor_current_pi_control */

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_init
* Description   : Initializes and resets rotor angle structure
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_init(st_rotor_angle_t *p_st_angle)
{
    motor_current_rotor_angle_reset(p_st_angle);
} /* End of function motor_current_rotor_angle_init */

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_reset
* Description   : Resets the angle to 0 degree and updates related trigonometric values
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_reset(st_rotor_angle_t *p_st_angle)
{
    p_st_angle->f4_rotor_angle_rad   = 0.0f;
    p_st_angle->f4_cos               = 1.0f;
    p_st_angle->f4_sin               = 0.0f;
} /* End of function motor_current_rotor_angle_reset */

/***********************************************************************************************************************
* Function Name : motor_current_rotor_angle_update
* Description   : Updates the angle and related trigonometric values
* Arguments     : p_st_angle   - The pointer to the rotor angle structure
*               : f4_angle_rad - The angle[rad] used to update the structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_rotor_angle_update(st_rotor_angle_t *p_st_angle, float f4_angle_rad)
{
    p_st_angle->f4_rotor_angle_rad = f4_angle_rad;
#if MOTOR_MCU_CFG_TFU_OPTIMIZE
     __sincosf(f4_angle_rad, &p_st_angle->f4_sin, &p_st_angle->f4_cos);
#else
    p_st_angle->f4_cos = cosf(f4_angle_rad);
    p_st_angle->f4_sin = sinf(f4_angle_rad);
#endif
} /* End of function motor_current_rotor_angle_update */

/***********************************************************************************************************************
* Function Name : motor_current_transform_uvw_dq_abs
* Description   : Coordinate transform UVW to dq (absolute transform)
* Arguments     : p_st_angle   - the pointer to rotor angle structure
*                 f4_uvw    - the pointer to the UVW-phase array in [U,V,W] format
*                 f4_dq     - where to store the [d,q] formated array on dq coordinates
* Return Value  : None
***********************************************************************************************************************/
void motor_current_transform_uvw_dq_abs(const st_rotor_angle_t *p_st_angle, const float *f4_uvw, float *f4_dq)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;
    float f4_temp3;
    float f4_u = f4_uvw[0];
    float f4_v_sub_w = f4_uvw[1] - f4_uvw[2];

    f4_temp0 = p_st_angle->f4_cos * (1.0f / MTR_SQRT_2);
    f4_temp1 = p_st_angle->f4_sin * (1.0f / MTR_SQRT_2);
    f4_temp2 = MTR_SQRT_3 * f4_temp0;
    f4_temp3 = MTR_SQRT_3 * f4_temp1;
    f4_dq[0] = (f4_temp2 * f4_u) + (f4_temp1 * f4_v_sub_w);
    f4_dq[1] = ((-f4_temp3) * f4_u) + (f4_temp0 * f4_v_sub_w);
} /* End of function motor_current_transform_uvw_dq_abs */

/***********************************************************************************************************************
* Function Name : motor_current_transform_dq_uvw_abs
* Description   : Coordinate transform dq to UVW 3-phase (absolute transform)
* Arguments     : p_st_angle   - the pointer to rotor angle structure
*                 f4_dq     - the pointer to the dq-axis value array in [D,Q] format
*                 f4_uvw    - where to store the [U,V,W] formated 3-phase quantities array
* Return Value  : None
***********************************************************************************************************************/
void motor_current_transform_dq_uvw_abs(const st_rotor_angle_t *p_st_angle, const float *f4_dq, float *f4_uvw)
{
    float f4_cos;
    float f4_sin;
    float f4_cos_div_sqrt3;
    float f4_sin_div_sqrt3;
    float f4_output_d;
    float f4_output_q;
    float f4_input_d = f4_dq[0];
    float f4_input_q = f4_dq[1];

    f4_cos           = p_st_angle->f4_cos;
    f4_sin           = p_st_angle->f4_sin;
    f4_cos_div_sqrt3 = f4_cos * (1.0f / MTR_SQRT_3);
    f4_sin_div_sqrt3 = f4_sin * (1.0f / MTR_SQRT_3);

    f4_uvw[0]        = MTR_SQRT_2 * ((f4_cos_div_sqrt3 * f4_input_d) - (f4_sin_div_sqrt3 * f4_input_q));

    f4_output_d      = ((-f4_cos_div_sqrt3) + f4_sin) * f4_input_d;
    f4_output_q      = (f4_sin_div_sqrt3 + f4_cos) * f4_input_q;
    f4_uvw[1]        = (f4_output_d + f4_output_q) * (1.0f / MTR_SQRT_2);

    f4_output_d      = ((-f4_cos_div_sqrt3) - f4_sin) * f4_input_d;
    f4_output_q      = (f4_sin_div_sqrt3 - f4_cos) * f4_input_q;
    f4_uvw[2]        = (f4_output_d + f4_output_q) * (1.0f / MTR_SQRT_2);
} /* End of function motor_current_transform_dq_uvw_abs */

/***********************************************************************************************************************
* Function Name : motor_current_volt_limit
* Description   : Limit voltage vector, d-axis voltage has higher priority than q-axis voltage
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_volt_limit(st_current_control_t *p_st_cc)
{

    float f4_va_max;
    float f4_vq_lim;
    float f4_vd_ref;

    f4_va_max = motor_current_mod_max_va_get(&(p_st_cc->st_mod));
    p_st_cc->f4_va_max = f4_va_max;
    f4_vd_ref = p_st_cc->f4_vd_ref;

    /* The d-axis voltage has high priority than the q-axis */
    /* If absolute value of D-axis voltage exceeds maximum voltage, limit it to the maximum voltage */
 /*   if (f4_vd_ref > f4_va_max)
    {
        p_st_cc->f4_vd_ref = f4_va_max;
        p_st_cc->f4_vq_ref = 0.0f;
    }
    else if (f4_vd_ref < (-f4_va_max))
    {
        p_st_cc->f4_vd_ref = -f4_va_max;
        p_st_cc->f4_vq_ref = 0.0f;
    }
    else
    {
        f4_vq_lim            = sqrtf((f4_va_max * f4_va_max) - (f4_vd_ref *  f4_vd_ref));
        p_st_cc->f4_vq_ref   = motor_filter_limitf_abs(p_st_cc->f4_vq_ref, f4_vq_lim);
    }*/
} /* End of function motor_current_volt_limit_set */

/***********************************************************************************************************************
* Function Name : motor_current_reference_iq_set
* Description   : Updates the q-axis current reference
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : Iq reference
***********************************************************************************************************************/
float motor_current_reference_iq_set(st_current_control_t *p_st_cc)
{
    float f4_iq_ref_calc;

    switch (p_st_cc->u1_state_iq_ref)
    {
        case CURRENT_STATE_IQ_ZERO_CONST:
            /*** constant zero set ***/
            f4_iq_ref_calc = 0.0f;
        break;

        case CURRENT_STATE_IQ_SPEED_PI_OUTPUT:
            f4_iq_ref_calc              = p_st_cc->f4_iq_ref;
            p_st_cc->f4_iq_down_step     = p_st_cc->f4_iq_down_step_time_inv * f4_iq_ref_calc;
        break;

        case CURRENT_STATE_IQ_AUTO_ADJ:
            f4_iq_ref_calc = p_st_cc->f4_iq_ref;
        break;

        case CURRENT_STATE_IQ_DOWN:
            f4_iq_ref_calc = p_st_cc->f4_ref_iq_ctrl - p_st_cc->f4_iq_down_step;
            if (f4_iq_ref_calc <= 0.0f)
            {
                f4_iq_ref_calc = 0.0f;
            }
        break;

        default:
            f4_iq_ref_calc = 0.0f;
        break;
    }
    /*** iq reference limit ***/
    f4_iq_ref_calc = motor_filter_limitf_abs(f4_iq_ref_calc, p_st_cc->f4_lim_iq);
    /* return iq reference */
    return (f4_iq_ref_calc);
} /* End of function motor_current_reference_iq_set */

/***********************************************************************************************************************
* Function Name : motor_current_reference_id_set
* Description   : Updates the d-axis current reference
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : Id reference
***********************************************************************************************************************/
float motor_current_reference_id_set(st_current_control_t *p_st_cc)
{
    float f4_id_ref_calc;

    switch (p_st_cc->u1_state_id_ref)
    {
        case CURRENT_STATE_ID_ZERO_CONST:
            f4_id_ref_calc = 0.0f;
        break;

        case CURRENT_STATE_ID_UP:
            /* d axis current ramping */
            f4_id_ref_calc = p_st_cc->f4_ref_id_ctrl + p_st_cc->f4_id_up_step;
            if(f4_id_ref_calc >= p_st_cc->f4_ol_ref_id)
            {
                f4_id_ref_calc = p_st_cc->f4_ol_ref_id;
            }
        break;

        case CURRENT_STATE_ID_MANUAL:
            f4_id_ref_calc = p_st_cc->f4_ol_ref_id;
        break;

        case CURRENT_STATE_ID_DOWN:
            /* d axis current ramping */
            f4_id_ref_calc = p_st_cc->f4_ref_id_ctrl - p_st_cc->f4_id_down_step;
            if (f4_id_ref_calc <= 0.0f)
            {
                f4_id_ref_calc = 0.0f;
            }
        break;

        case CURRENT_STATE_ID_INPUT:
            f4_id_ref_calc = p_st_cc->f4_id_ref;
        break;

        default:
            f4_id_ref_calc = 0.0f;
        break;
    }
    /* return id reference */
    return (f4_id_ref_calc);
} /* End of function motor_current_reference_id_set */

/***********************************************************************************************************************
* Function Name : motor_current_angle_speed_detect
* Description   : Angle and speed detection
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_current_angle_speed_detect(st_current_control_t *p_st_cc)
{
    float f4_angle_rad;

    /* Estimate voltage disturbance */
    motor_current_bemf_observer_start(&p_st_cc->st_bemf_observer,
                                      p_st_cc->f4_vd_ref,
                                      p_st_cc->f4_vq_ref,
                                      p_st_cc->f4_id_ad,
                                      p_st_cc->f4_iq_ad);

    /* Calculate BEMF */
    p_st_cc->f4_ed = motor_current_bemf_observer_d_calc(&p_st_cc->st_bemf_observer,
                                                        p_st_cc->f4_speed_rad,
                                                        p_st_cc->f4_iq_ad);
    p_st_cc->f4_eq = motor_current_bemf_observer_q_calc(&p_st_cc->st_bemf_observer,
                                                        p_st_cc->f4_speed_rad,
                                                        p_st_cc->f4_id_ad);

    /* Calculate phase error */
#if MOTOR_MCU_CFG_TFU_OPTIMIZE
    p_st_cc->f4_phase_err_rad = atan2f((p_st_cc->f4_ed / p_st_cc->f4_eq), 1);
#else
    p_st_cc->f4_phase_err_rad = atanf(p_st_cc->f4_ed / p_st_cc->f4_eq);
#endif

    /* Prevent phase error becoming NAN when eq is 0 */
    if (isnan(p_st_cc->f4_phase_err_rad))
    {
        p_st_cc->f4_phase_err_rad = 0.0f;
    }

    if (CURRENT_STATE_IQ_SPEED_PI_OUTPUT == p_st_cc->u1_state_iq_ref)
    {
        /* Estimate angle and speed */
        motor_current_angle_speed_pll(&p_st_cc->st_pll_est, p_st_cc->f4_phase_err_rad, &p_st_cc->f4_speed_rad);
    }
    else
    {
        p_st_cc->f4_speed_rad = p_st_cc->f4_ol_speed_rad;
    }

    f4_angle_rad = motor_current_angle_get(&p_st_cc->st_rotor_angle);
    f4_angle_rad += (p_st_cc->f4_speed_rad * p_st_cc->f4_ctrl_period);
    if (f4_angle_rad >= MTR_TWOPI)
    {
        f4_angle_rad = f4_angle_rad - MTR_TWOPI;
    }
    else if (f4_angle_rad < 0.0f)
    {
        f4_angle_rad = f4_angle_rad + MTR_TWOPI;
    }
    else
    {
        /* Do nothing */
    }
    motor_current_rotor_angle_update(&p_st_cc->st_rotor_angle, f4_angle_rad);
} /* End of function motor_current_angle_speed_detect */

/***********************************************************************************************************************
* Function Name : motor_current_angle_get
* Description   : Gets the electrical angle of rotor [rad]
* Arguments     : p_st_angle - The pointer to the rotor angle structure
* Return Value  : The electrical angle of rotor [rad]
***********************************************************************************************************************/
float motor_current_angle_get(st_rotor_angle_t *p_st_angle)
{
    return (p_st_angle->f4_rotor_angle_rad);
} /* End of function motor_current_angle_get */

/***********************************************************************************************************************
* Function Name: motor_current_control_smpldly_comp
* Description  : current control sampling delay compensation
* Arguments    : p_st_cc - The pointer to the current control structure
*              : f4_angle_rad - motor angle (electrical) [rad]
*              : f4_speed_rad - motor speed (electrical) [rad/s]
* Return Value : motor angle (electrical) [rad/s]
***********************************************************************************************************************/
float motor_current_control_smpldly_comp(st_current_control_t *p_st_cc, float f4_angle_rad, float f4_speed_rad)
{
    float f4_comp_angle_rad;
    float f4_temp_angle_rad;

    f4_comp_angle_rad = f4_speed_rad * p_st_cc->f4_ctrl_period * CURRENT_CFG_PERIOD_MAG_VALUE;
    f4_temp_angle_rad = f4_angle_rad + f4_comp_angle_rad;
    /* limit angle value of one rotation */
    if (f4_temp_angle_rad >= MTR_TWOPI)
    {
        f4_temp_angle_rad -= MTR_TWOPI;
    }
    else if (f4_temp_angle_rad < 0.0f)
    {
        f4_temp_angle_rad += MTR_TWOPI;
    }
    else
    {
        /* Do Nothing */
    }
    return (f4_temp_angle_rad);
} /* End of function motor_current_control_smpldly_comp */
