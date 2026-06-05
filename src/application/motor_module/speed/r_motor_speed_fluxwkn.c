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
/**********************************************************************************************************************
* File Name    : r_motor_speed_fluxwkn.c
* Description  : This module performs flux weakening in FOC
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 11.06.2021 1.00
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <stdint.h>
#include <math.h>

/* Main associated header file */
#include "r_motor_speed_fluxwkn.h"

float Id_fw;
float Iq_fw;

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_get_vec
* Description  : Calculates one component of a vector from the absolute value and the other component of the vector
* Arguments    : f4_abs - The magnitude of the vector
*                f4_vec1 - The known component of the vector
* Return Value : The other component of the vector
***********************************************************************************************************************/
static inline float motor_speed_flux_weakn_get_vec(float f4_abs, float f4_vec1)
{
    float res;
    res = (f4_abs * f4_abs) - (f4_vec1 * f4_vec1);

    if (res < 0.0f)
    {
        res = 0.0f;
    }
    else
    {
        res = sqrtf(res);
    }

    return (res);
} /* End of function motor_speed_flux_weakn_get_vec */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_vec_abs
* Description  : Calculates magnitude of the vector
* Arguments    : f4_vec1 and f4_vec2 - The components of the vector
* Return Value : magnitude of the vector
***********************************************************************************************************************/
static inline float motor_speed_flux_weakn_vec_abs(float f4_vec1, float f4_vec2)
{
    return (sqrtf((f4_vec1 * f4_vec1) + (f4_vec2 * f4_vec2)));
} /* End of function motor_speed_flux_weakn_vec_abs */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_init
* Description  : Initializes flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                f4_ia_max    - The maximum magnitude of current vector
*                f4_va_max    - The maximum magnitude of voltage vector
*                f4_vfw_ratio - The maximum ratio of voltage available for flux-weakening module
*                p_st_motor   - The pointer to the motor parameter data structure
* Note         : Other parameters will be set to the corresponding default value
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_init(st_fluxwkn_t * p_st_fluxwkn,
                                 float f4_ia_max,
                                 float f4_va_max,
                                 float f4_vfw_ratio,
                                 st_motor_parameter_t *p_st_motor)
{
    /* Set up motor */
    motor_speed_flux_weakn_motor_set(p_st_fluxwkn, p_st_motor);

    /* Set up circle limitation of current vector and voltage vector */
    motor_speed_flux_weakn_vamax_set(p_st_fluxwkn, f4_va_max);
    motor_speed_flux_weakn_iamax_set(p_st_fluxwkn, f4_ia_max);

    /* Set up flux-weakening voltage margin */
    motor_speed_flux_weakn_vfw_ratio_set(p_st_fluxwkn, f4_vfw_ratio);

    p_st_fluxwkn->f4_v_fw = 0.0f;

    /* Reset module */
    motor_speed_flux_weakn_reset(p_st_fluxwkn);
} /* End of function motor_speed_flux_weakn_init */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_reset
* Description  : Resets flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_reset(st_fluxwkn_t * p_st_fluxwkn)
{
    /* Reset only when parameters are valid */
    if (0 == motor_speed_flux_weakn_invalid_parameter_check(p_st_fluxwkn))
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
    }
} /* End of function motor_speed_flux_weakn_reset */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_start
* Description  : Executes the flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                f4_speed_rad - The electrical speed of motor in [rad/s]
*                p_f4_idq     - The pointer to the measured current vector (array) in format {Id,Iq}
*                p_f4_idq_ref - The pointer to the reference current vector (array) in format {Idref, Iqref}
* Return Value : Status of the module, refer to @ref Flux-weakening-states
***********************************************************************************************************************/
uint16_t motor_speed_flux_weakn_start(st_fluxwkn_t * p_st_fluxwkn,
                                      float f4_speed_rad,
                                      const float *p_f4_idq,
                                      float *p_f4_idq_ref)
{
    float f4_id_ref;
    float f4_iq_ref;
    float f4_v_om;
    float f4_id_est;
    float f4_iq_max_abs;
    float f4_flux_abs;
    float f4_flux_q;
    float f4_flux_d;
    float f4_temp;
    float f4_flux_pm    =  p_st_fluxwkn->p_motor->f4_mtr_m;
    float f4_ld         =  p_st_fluxwkn->p_motor->f4_mtr_ld;
    float f4_lq         =  p_st_fluxwkn->p_motor->f4_mtr_lq;
    float f4_r          =  p_st_fluxwkn->p_motor->f4_mtr_r;

    /* Check if any error have occurred */
    if (motor_speed_flux_weakn_error_check(p_st_fluxwkn))
    {
        return (p_st_fluxwkn->u2_fw_status);
    }

    /* Prevent zero denominator, return a runtime error when input speed is 0 */
    if (0.01f > fabs(f4_speed_rad))
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_ERROR;
        return (p_st_fluxwkn->u2_fw_status);
    }

    /* Calculate Voltage vector limit exclude resistance voltage vector */
    f4_temp = f4_r * motor_speed_flux_weakn_vec_abs(p_f4_idq[0], p_f4_idq[1]);
    f4_v_om = (p_st_fluxwkn->f4_va_max - f4_temp) * p_st_fluxwkn->f4_vfw_ratio;

    /* Store the Vom for monitoring */
    p_st_fluxwkn->f4_v_fw = f4_v_om;

    /* Calculate d-axis value of the intersection between voltage limit circle and current limit circle */
    f4_temp = (f4_v_om * f4_v_om) / ((f4_speed_rad * f4_speed_rad) * (f4_ld * f4_ld));
    f4_temp -= (p_st_fluxwkn->f4_id_demag * p_st_fluxwkn->f4_id_demag);
    f4_temp -= (p_st_fluxwkn->f4_ia_max * p_st_fluxwkn->f4_ia_max);
    f4_id_est = f4_temp / ((-2.0f) * p_st_fluxwkn->f4_id_demag);

    /* Calculate q-axis current limit */
    if (f4_id_est > 0.0f)
    {
        /* If id_est > 0, flux-weakening is not necessary at max torque, maximum Iq is only limited by current limit
         and maximum iq is determined by ia_max */
        f4_iq_max_abs = p_st_fluxwkn->f4_ia_max;
    }
    else if (f4_id_est < p_st_fluxwkn->f4_id_demag)
    {
        /* Since more current on minus d-axis dose not benefit, maximum Iq is only limited by the voltage limit circle
         */
        f4_iq_max_abs = fabsf(f4_v_om / (f4_lq * f4_speed_rad));
    }
    else
    {
        /* Normal flux-weakening, at max torque */
        f4_iq_max_abs = motor_speed_flux_weakn_get_vec(p_st_fluxwkn->f4_ia_max, f4_id_est);
    }

    /* Limit Iq reference */
    f4_iq_ref = p_f4_idq_ref[1];
    if (f4_iq_ref > f4_iq_max_abs)
    {
        f4_iq_ref = f4_iq_max_abs;
    }
    else if (f4_iq_ref < (-f4_iq_max_abs))
    {
        f4_iq_ref = -f4_iq_max_abs;
    }
    else
    {
        /* Do nothing */
    }

    /* Calculate Id reference */
    f4_flux_abs = f4_v_om / f4_speed_rad;
    f4_flux_d   = f4_lq * f4_iq_ref;
    f4_flux_q   = motor_speed_flux_weakn_get_vec(f4_flux_abs , f4_flux_d);
    f4_id_ref   = (f4_flux_q - f4_flux_pm) / (f4_ld);

    if (f4_id_ref <= p_st_fluxwkn->f4_id_min)
    {
        f4_id_ref =  p_st_fluxwkn->f4_id_min;;
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_IDSAT;
    }
    else if (f4_id_ref >= 0.0f)
    {
        f4_id_ref = 0.0f;
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
    }
    else
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_FLUXWKN;
    }

    p_f4_idq_ref[0] = f4_id_ref;
    p_f4_idq_ref[1] = f4_iq_ref;

    Id_fw = f4_id_ref;
    Iq_fw = f4_iq_ref;

    return (p_st_fluxwkn->u2_fw_status);
} /* End of function motor_speed_flux_weakn_start */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_motor_set
* Description  : Set up the motor parameter
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                p_st_motor   - The motor parameters to be used flux-weakening module
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_motor_set(st_fluxwkn_t * p_st_fluxwkn,  st_motor_parameter_t * p_st_motor)
{
    uint8_t u1_is_valid_motor;

    /* Check if any of motor parameter is obviously invalid */
    u1_is_valid_motor = (0 != p_st_motor);
    u1_is_valid_motor = u1_is_valid_motor && (p_st_motor->f4_mtr_m  > 0.0f);
    u1_is_valid_motor = u1_is_valid_motor && (p_st_motor->f4_mtr_ld > 0.0f);
    u1_is_valid_motor = u1_is_valid_motor && (p_st_motor->f4_mtr_lq > 0.0f);
    u1_is_valid_motor = u1_is_valid_motor && (p_st_motor->f4_mtr_r  > 0.0f);

    if (u1_is_valid_motor)
    {
        p_st_fluxwkn->p_motor     = p_st_motor;
        p_st_fluxwkn->f4_id_demag = -(p_st_motor->f4_mtr_m / p_st_motor->f4_mtr_ld);
        if (FLUXWKN_STATE_INVALID_MOTOR == p_st_fluxwkn->u2_fw_status)
        {
            p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_INVALID_MOTOR;
    }
} /* End of function motor_speed_flux_weakn_motor_set */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_iamax_set
* Description  : Sets the maximum magnitude of current vector
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                f4_ia_max    - The maximum magnitude of current vector
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_iamax_set(st_fluxwkn_t * p_st_fluxwkn, float f4_ia_max)
{
    if (f4_ia_max > 0.0f)
    {
        p_st_fluxwkn->f4_ia_max = f4_ia_max;

        /* Limit minimum d-axis current with min(-f4_ia_max,f4_id_demag) */
        if (p_st_fluxwkn->f4_id_demag > (-p_st_fluxwkn->f4_ia_max))
        {
            p_st_fluxwkn->f4_id_min = p_st_fluxwkn->f4_id_demag;
        }
        else
        {
            p_st_fluxwkn->f4_id_min = -p_st_fluxwkn->f4_ia_max;
        }

        if (FLUXWKN_STATE_INVALID_IAMAX == p_st_fluxwkn->u2_fw_status)
        {
            p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_INVALID_IAMAX;
    }
} /* End of function motor_speed_flux_weakn_iamax_set */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_vamax_set
* Description  : Sets the maximum magnitude of voltage vector
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                f4_va_max    - The maximum magnitude of voltage vector
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_vamax_set(st_fluxwkn_t * p_st_fluxwkn, float f4_va_max)
{
    if (f4_va_max > 0.0f)
    {
        p_st_fluxwkn->f4_va_max = f4_va_max;

        if (FLUXWKN_STATE_INVALID_VAMAX == p_st_fluxwkn->u2_fw_status)
        {
            p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_INVALID_VAMAX;
    }
} /* End of function motor_speed_flux_weakn_vamax_set */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_vfw_ratio_set
* Description  : Sets the maximum available voltage ratio for the flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
*                f4_vfw_ratio - The maximum ratio of voltage available for flux-weakening module
* Return Value : None
***********************************************************************************************************************/
void motor_speed_flux_weakn_vfw_ratio_set(st_fluxwkn_t * p_st_fluxwkn, float f4_vfw_ratio)
{
    if ((f4_vfw_ratio >= FLUXWKN_DEF_VFWRATIO_MIN) && (f4_vfw_ratio <= 1.0f))
    {
        p_st_fluxwkn->f4_vfw_ratio = f4_vfw_ratio;

        if (FLUXWKN_STATE_INVALID_VFWRATIO == p_st_fluxwkn->u2_fw_status)
        {
            p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_st_fluxwkn->u2_fw_status = FLUXWKN_STATE_INVALID_VFWRATIO;
    }
} /* End of function motor_speed_flux_weakn_vfw_ratio_set */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_iamax_get
* Description  : Gets the maximum magnitude of current vector
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : The maximum magnitude of current vector
***********************************************************************************************************************/
float motor_speed_flux_weakn_iamax_get(st_fluxwkn_t * p_st_fluxwkn)
{
    return (p_st_fluxwkn->f4_ia_max);
} /* End of function motor_speed_flux_weakn_iamax_get */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_vamax_get
* Description  : Gets the maximum magnitude of voltage vector
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : The maximum magnitude of voltage vector
***********************************************************************************************************************/
float motor_speed_flux_weakn_vamax_get(st_fluxwkn_t * p_st_fluxwkn)
{
    return (p_st_fluxwkn->f4_va_max);
} /* End of function motor_speed_flux_weakn_vamax_get */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_vfw_ratio_get
* Description  : Gets the maximum available voltage ratio for the flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : Maximum available voltage ratio for the flux-weakening module
***********************************************************************************************************************/
float motor_speed_flux_weakn_vfw_ratio_get(st_fluxwkn_t * p_st_fluxwkn)
{
    return (p_st_fluxwkn->f4_vfw_ratio);
} /* End of function motor_speed_flux_weakn_vfw_ratio_get */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_status_get
* Description  : Gets the status of flux-weakening module
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : Status of flux-weakening module, refer to Flux-weakening-states
***********************************************************************************************************************/
uint16_t motor_speed_flux_weakn_status_get(st_fluxwkn_t * p_st_fluxwkn)
{
    return (p_st_fluxwkn->u2_fw_status);
} /* End of function motor_speed_flux_weakn_status_get */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_error_check
* Description  : Checks if the status is an error (include "invalid parameter error")
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : True(1) or False(0)
***********************************************************************************************************************/
uint8_t motor_speed_flux_weakn_error_check(st_fluxwkn_t * p_st_fluxwkn)
{
    return ((p_st_fluxwkn->u2_fw_status & 0xF000) != 0);
} /* End of function motor_speed_flux_weakn_error_check */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_invalid_parameter_check
* Description  : Checks if the status is an invalid parameter error
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : True(1) or False(0)
***********************************************************************************************************************/
uint8_t motor_speed_flux_weakn_invalid_parameter_check(st_fluxwkn_t * p_st_fluxwkn)
{
    return ((p_st_fluxwkn->u2_fw_status & 0xF000) == 0x9000);
} /* End of function motor_speed_flux_weakn_invalid_parameter_check */

/***********************************************************************************************************************
* Function Name: motor_speed_flux_weakn_run_time_error_check
* Description  : Checks if the status is an run-time error
* Arguments    : p_st_fluxwkn - The pointer to the structure of flux-weakening module
* Return Value : True(1) or False(0)
***********************************************************************************************************************/
uint8_t motor_speed_flux_weakn_run_time_error_check(st_fluxwkn_t * p_st_fluxwkn)
{
    return ((p_st_fluxwkn->u2_fw_status & 0xF000) == 0x8000);
} /* End of function motor_speed_flux_weakn_run_time_error_check */
