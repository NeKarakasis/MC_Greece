/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "rm_motor_fundlib.h"
#include "rm_motor_hal_driver_modulation.h"

#define     MOD_DEFAULT_MAX_DUTY          (1.0F)      /* Default maximum duty cycle */
#define     MOD_DEFAULT_MIN_DUTY          (CCAL_ZERO) /* Default minimum duty cycle */

#define     MOTOR_DRIVER_SATFLAG_BITU     (1 << 0)    /* Saturation flag bit for U phase */
#define     MOTOR_DRIVER_SATFLAG_BITV     (1 << 1)    /* Saturation flag bit for V phase */
#define     MOTOR_DRIVER_SATFLAG_BITW     (1 << 2)    /* Saturation flag bit for W phase */

/*
 * Vamax in this module is calculated by the following equation
 *   SVPWM :  Vdc * (MOD_VDC_TO_VAMAX_MULT) * (Max duty - Min duty) * (MOD_SVPWM_MULT)
 *   SPWM  :  Vdc * (MOD_VDC_TO_VAMAX_MULT) * (Max duty - Min duty)
 */
#define MOTOR_DRIVER_VDC_TO_VAMAX_MULT    (0.6124F)   /* The basic coefficient used to convert Vdc to Vamax */
#define MOTOR_DRIVER_SVPWM_MULT           (1.155F)    /* The usable voltage is multiplied by sqrt(4/3) when using SVPWM */

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_svpwm
 * Description  : Space vector modulation
 * Arguments    : p_v_in -
 *                    Input data, in an array [Vu,Vv,Vw]
 *                p_v_out -
 *                    Where to store output data, in an array [Vu,Vv,Vw]
 * Return Value : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_modulation_svpwm (const ccal_num_t * p_v_in, ccal_num_t * p_v_out)
{
    ccal_num_t v_max;
    ccal_num_t v_min;
    ccal_num_t v_com;

    /* Sort vu vv vw */
    if (p_v_in[0] > p_v_in[1])
    {
        v_max = p_v_in[0];
        v_min = p_v_in[1];
    }
    else
    {
        v_max = p_v_in[1];
        v_min = p_v_in[0];
    }

    /* Vcom = (Vmin + Vmax)/2 */
    if (p_v_in[2] > v_max)
    {
        v_com = (p_v_in[2] + v_min);
    }
    else if (p_v_in[2] < v_min)
    {
        v_com = (v_max + p_v_in[2]);
    }
    else
    {
        v_com = (v_max + v_min);
    }

    v_com = CCAL_DIV_BY_2(v_com);

    p_v_out[0] = p_v_in[0] - v_com;
    p_v_out[1] = p_v_in[1] - v_com;
    p_v_out[2] = p_v_in[2] - v_com;
}                                      /* End of function rm_motor_hal_driver_modulation_svpwm() */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_dpwm
 * Description  : Two phase modulation
 * Arguments    : p_v_in -
 *                    Input data, in an array [Vu,Vv,Vw]
 *                p_v_out -
 *                    Where to store output data, in an array [Vu,Vv,Vw]
 *                v_dc -
 *                    Bus voltage [V]
 *                p_maxvoltage_flag -
 *                    Phase of maximum voltage in DPWM method
 * Return Value : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_modulation_dpwm (const ccal_num_t * p_v_in,
                                                 ccal_num_t       * p_v_out,
                                                 const ccal_num_t   v_dc,
                                                 uint8_t          * p_maxvoltage_flag)
{
    ccal_num_t v_max = p_v_in[0];
    ccal_num_t v_min = p_v_in[0];
    uint8_t    u1_buffer_maxvoltage_flag = 0;
    uint8_t    index;

    for (index = 1; index < 3; index++)
    {
        if (v_max < p_v_in[index])
        {
            v_max = p_v_in[index];
            u1_buffer_maxvoltage_flag = index;
        }

        if (v_min > p_v_in[index])
        {
            v_min = p_v_in[index];
        }
    }

    (*p_maxvoltage_flag) = u1_buffer_maxvoltage_flag;

    p_v_out[0] = p_v_in[0] - CCAL_DIV_BY_2(v_dc) - v_min;
    p_v_out[1] = p_v_in[1] - CCAL_DIV_BY_2(v_dc) - v_min;
    p_v_out[2] = p_v_in[2] - CCAL_DIV_BY_2(v_dc) - v_min;
}                                      /* End of function rm_motor_hal_driver_modulation_dpwm() */

/**
 * @brief Limits the duty cycle values within the minimum and maximum boundaries
 *
 * This function ensures that the three-phase duty cycle values stay within the allowed
 * range defined by minimum and maximum duty cycle parameters. If saturation detection
 * is enabled, it also sets appropriate saturation flags when limits are reached.
 *
 * @param[in,out] p_mod   Pointer to the modulation structure containing min/max duty values
 * @param[in,out] p_duty  Pointer to an array of three duty cycle values (U, V, W phases)
 */
static inline void rm_motor_hal_driver_modulation_limit (motor_hal_driver_modulation_t * p_mod, ccal_num_t * p_duty)
{
    ccal_num_t min_duty = p_mod->min_duty;
    ccal_num_t max_duty = p_mod->max_duty;

    if (p_duty[0] > max_duty)
    {
        p_duty[0] = max_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITU;
#endif
    }
    else if (p_duty[0] < min_duty)
    {
        p_duty[0] = min_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITU;
#endif
    }
    else
    {
        /* Clear correspond saturation flag bit */
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag &= (~MOTOR_DRIVER_SATFLAG_BITU);
#endif
    }

    if (p_duty[1] > max_duty)
    {
        p_duty[1] = max_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITV;
#endif
    }
    else if (p_duty[1] < min_duty)
    {
        p_duty[1] = min_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITV;
#endif
    }
    else
    {
        /* Clear correspond saturation flag bit */
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag &= (~MOTOR_DRIVER_SATFLAG_BITV);
#endif
    }

    if (p_duty[2] > max_duty)
    {
        p_duty[2] = max_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITW;
#endif
    }
    else if (p_duty[2] < min_duty)
    {
        p_duty[2] = min_duty;
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag |= MOTOR_DRIVER_SATFLAG_BITW;
#endif
    }
    else
    {
        /* Clear correspond saturation flag bit */
#if (MOTOR_HAL_DRIVER_CFG_SATURATION_CHECKING_ENABLE == 1)
        p_mod->saturation_flag &= (~MOTOR_DRIVER_SATFLAG_BITW);
#endif
    }
}                                      /* End of function rm_motor_hal_driver_modulation_limit() */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_run
 * Description  : Calculates duty cycle from input 3-phase voltage (bipolar)
 * Arguments    : p_ctrl -
 *                    Pointer to the motor driver module valuables
 *              : p_v_in -
 *                    Pointer to the 3-phase input voltage
 *              : p_duty_out -
 *                    Where to store the 3-phase output duty cycle
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_run (motor_hal_driver_modulation_t * p_mod,
                                         motor_hal_driver_modulation_t * p_cfg_mod,
                                         const ccal_num_t              * p_v_in,
                                         ccal_num_t                    * p_duty_out,
                                         uint8_t                       * p_maxvoltage_flag,
                                         uint8_t                         modulation_method)
{
    ccal_num_t v_out[3];
    ccal_num_t inv_vdc;

    inv_vdc = CCAL_RECI(p_mod->v_dc, MTR_Q_VOLTAGE);

    if (MOTOR_HAL_DRIVER_MODULATION_METHOD_SVPWM == modulation_method)
    {
        rm_motor_hal_driver_modulation_svpwm(&p_v_in[0], &v_out[0]);

        rm_motor_hal_driver_modulation_set_min_duty(p_mod, p_cfg_mod->min_duty);
        rm_motor_hal_driver_modulation_set_max_duty(p_mod, p_cfg_mod->max_duty);
    }
    else if (MOTOR_HAL_DRIVER_MODULATION_METHOD_DPWM == modulation_method)
    {
        rm_motor_hal_driver_modulation_dpwm(&p_v_in[0], &v_out[0], p_mod->v_dc, p_maxvoltage_flag);

        rm_motor_hal_driver_modulation_set_min_duty(p_mod, MOD_DEFAULT_MIN_DUTY);
        rm_motor_hal_driver_modulation_set_max_duty(p_mod, MOD_DEFAULT_MAX_DUTY);
    }
    else                               /* SPWM */
    {
        v_out[0] = p_v_in[0];
        v_out[1] = p_v_in[1];
        v_out[2] = p_v_in[2];

        rm_motor_hal_driver_modulation_set_min_duty(p_mod, p_cfg_mod->min_duty);
        rm_motor_hal_driver_modulation_set_max_duty(p_mod, p_cfg_mod->max_duty);
    }

    /* Convert the phase voltage to the PWM duty ratio */
    p_duty_out[0] = (v_out[0] * inv_vdc) + p_mod->neutral_duty;
    p_duty_out[1] = (v_out[1] * inv_vdc) + p_mod->neutral_duty;
    p_duty_out[2] = (v_out[2] * inv_vdc) + p_mod->neutral_duty;

    rm_motor_hal_driver_modulation_limit(p_mod, p_duty_out);
}                                      /* End of function rm_motor_hal_driver_modulation_run() */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_set_max_duty
 * Description  : Sets the maximum duty cycle
 * Arguments    : p_mod -
 *                    Pointer to the modulation data structure
 *                max_duty -
 *                    Maximum duty cycle to set
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_set_max_duty (motor_hal_driver_modulation_t * p_mod, ccal_num_t max_duty)
{
    if ((0 != p_mod) && ((max_duty > CCAL_ZERO) && (max_duty <= 1.0F)))
    {
        p_mod->max_duty     = max_duty;
        p_mod->neutral_duty = CCAL_DIV_BY_2(p_mod->max_duty + p_mod->min_duty);
    }
}                                      /* End of function rm_motor_hal_driver_modulation_set_max_duty() */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_set_min_duty
 * Description  : Sets the minimum duty cycle
 * Arguments    : p_mod -
 *                    Pointer to the modulation data structure
 *                min_duty -
 *                    Minimum duty cycle to set
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_set_min_duty (motor_hal_driver_modulation_t * p_mod, ccal_num_t min_duty)
{
    if ((0 != p_mod) && (min_duty >= CCAL_ZERO))
    {
        p_mod->min_duty     = min_duty;
        p_mod->neutral_duty = CCAL_DIV_BY_2(p_mod->max_duty + p_mod->min_duty);
    }
}                                      /* End of function rm_motor_hal_driver_modulation_set_min_duty() */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_get_vamax
 * Description  : Gets the maximum magnitude of voltage vector
 * Arguments    : p_mod -
 *                    The pointer to the modulation data structure
 *                u1_method -
 *                    Modulation method (SPWM, SVPWM, DPWM)
 * Return Value : The maximum magnitude of voltage vector
 **********************************************************************************************************************/
ccal_num_t rm_motor_hal_driver_modulation_get_vamax (motor_hal_driver_modulation_t * p_mod, uint8_t u1_method)
{
    ccal_num_t usable_duty_cycle;
    ccal_num_t ret = (MOTOR_DRIVER_VDC_TO_VAMAX_MULT * p_mod->v_dc);

    usable_duty_cycle = (p_mod->max_duty - p_mod->min_duty) - CCAL_MUL_BY_2(p_mod->voltage_error_ratio);

    if (MOTOR_HAL_DRIVER_MODULATION_METHOD_SPWM != u1_method)
    {
        usable_duty_cycle = usable_duty_cycle * MOTOR_DRIVER_SVPWM_MULT;
    }

    ret *= usable_duty_cycle;

    return ret;
}                                      /* End of function rm_motor_hal_driver_modulation_get_vamax */
