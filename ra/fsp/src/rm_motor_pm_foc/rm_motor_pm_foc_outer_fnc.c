/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdint.h>
#include "rm_motor_pm_foc.h"
#include "rm_motor_pm_foc_outer_fnc.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define MOTOR_OUTER_FW_THRESHOLD_SPEED_MECH    0.01F

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_PM_FOC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Dummy function for disabled algorithms.
 *
 * This function is used as a placeholder in the function pointer table when a specific algorithm
 * (e.g., MTPA, flux weakening, speed observer) is disabled in the configuration.
 *
 * @param[in] p_ctrl  Pointer to control structure (unused)
 * @param[in] p_cfg   Pointer to configuration structure (unused)
 *
 * @retval None
 **********************************************************************************************************************/
void algorithm_disabled (void * p_ctrl, void * p_cfg) {
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);
}

/*******************************************************************************************************************//**
 * @brief Speed observer control function.
 *
 * This function applies the speed observer algorithm to estimate the mechanical speed
 * based on q-axis current reference and measured speed. The estimated speed is stored
 * in the low-pass filtered speed variable.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_spdobsrv_ctrl (void * p_ctrl, void * p_cfg)
{
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_cfg;

    p_instance_ctrl->signals.speed_mech_lpf = rm_motor_spdobserver(&p_instance_ctrl->signals.speed_observer_ctrl,
                                                                   p_extended_cfg->pm_motor_parameter,
                                                                   p_instance_ctrl->p_to_outer_copy->i_q_ref,
                                                                   p_instance_ctrl->signals.speed_mech);
}

/*******************************************************************************************************************//**
 * @brief Direct speed assignment control function.
 *
 * This function directly assigns the measured mechanical speed to the low-pass filtered speed variable
 * without applying any filtering or observer algorithm.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration (unused)
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_spddirct_ctrl (void * p_ctrl, void * p_cfg)
{
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;

    FSP_PARAMETER_NOT_USED(p_cfg);

    /* Directly assign the measured speed to the filtered speed value */
    p_instance_ctrl->signals.speed_mech_lpf = p_instance_ctrl->signals.speed_mech;
}

/*******************************************************************************************************************//**
 * @brief Low-pass filter processing for speed.
 *
 * This function applies a first-order low-pass filter to the measured mechanical speed
 * and updates the filtered speed value in the control instance.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration (unused)
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_spdlpf_ctrl (void * p_ctrl, void * p_cfg)
{
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;

    FSP_PARAMETER_NOT_USED(p_cfg);

    /* Apply first-order low-pass filter to speed measurement and update filtered speed */
    p_instance_ctrl->signals.speed_mech_lpf =
        rm_motor_filter_first_order_lpff(&(p_instance_ctrl->signals.speed_lpf_ctrl),
                                         p_instance_ctrl->signals.speed_mech);
}

/*******************************************************************************************************************//**
 * @brief Extended observer for disturbance torque and speed estimation.
 *
 * This function applies an extended observer to estimate mechanical speed excluding disturbance torque effects.
 * It also generates a disturbance compensation current reference for torque ripple suppression.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_extobsrv_ctrl (void * p_ctrl, void * p_cfg)
{
    /* Extended observer for disturbance torque / speed estimation */
    float f4_torque_nm;
    float f4_speed_obsrv_rad;
    float f4_disturbance_current = 0.0F;

    /* Instance */
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_cfg;

    /* Calculate torque */
    f4_torque_nm =
        ((float) p_extended_cfg->pm_motor_parameter->u2_mtr_pp * p_extended_cfg->pm_motor_parameter->f4_mtr_m) *
        p_instance_ctrl->p_to_outer_copy->i_q_ref;

    /* Extended observer */
    rm_motor_extobserver_start(&p_instance_ctrl->signals.extobserver_ctrl,
                               f4_torque_nm,
                               p_instance_ctrl->signals.speed_mech);
    f4_speed_obsrv_rad = rm_motor_extobserver_mech_speed_get(&p_instance_ctrl->signals.extobserver_ctrl);
    p_instance_ctrl->signals.speed_mech_lpf = f4_speed_obsrv_rad;

    /* Torque command for ASR FF */
    f4_disturbance_current = rm_motor_extobserver_disturbance_get(&p_instance_ctrl->signals.extobserver_ctrl) /
                             ((float) p_extended_cfg->pm_motor_parameter->u2_mtr_pp *
                              p_extended_cfg->pm_motor_parameter->f4_mtr_m);

    p_instance_ctrl->signals.i_q_ref_disturbance = f4_disturbance_current;
}

/*******************************************************************************************************************//**
 * @brief MTPA (Maximum Torque Per Ampere) control function.
 *
 * This function applies the MTPA algorithm to optimize d- and q-axis current references
 * for maximum torque efficiency. If flux weakening is active, it integrates flux weakening
 * control with MTPA to select appropriate current commands. The function ensures that
 * d-axis current reference remains non-positive for non-salient PMSM.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_mtpa_ctrl (void * p_ctrl, void * p_cfg)
{
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_cfg;

    uint16_t u2_fw_status = p_instance_ctrl->signals.flux_weakening_ctrl.u2_fw_status;
    float    f4_idq_ref[2];
    float    f4_idq_ref_temp[2];

    /* Initialize reference arrays with current id/iq references */
    f4_idq_ref[0]      = p_instance_ctrl->p_to_outer_copy->i_d_ref;
    f4_idq_ref[1]      = p_instance_ctrl->p_to_outer_copy->i_q_ref;
    f4_idq_ref_temp[0] = p_instance_ctrl->p_to_outer_copy->i_d_ref;
    f4_idq_ref_temp[1] = p_instance_ctrl->p_to_outer_copy->i_q_ref;

    /* Run MTPA algorithm to adjust dq-axis references */
    rm_motor_mtpa_run(p_extended_cfg->pm_motor_parameter, &(f4_idq_ref_temp[0]), &(f4_idq_ref_temp[1]));

    /* If flux weakening is active and the flux weakening control function is enabled */
    if ((MOTOR_FLUXWKN_STATE_FLUXWKN == u2_fw_status) &&
        (p_extended_cfg->outerfunc_table.fw_ctrl != &algorithm_disabled))
    {
        /* Select between MTPA and flux weakening control */
        rm_motor_mtpa_wekn_judge(p_extended_cfg->pm_motor_parameter,
                                 p_instance_ctrl->signals.speed_mech_lpf,
                                 p_instance_ctrl->signals.flux_weakening_ctrl.f4_va_max,
                                 f4_idq_ref_temp[0],
                                 f4_idq_ref[0],
                                 &f4_idq_ref[0]);
    }
    else
    {
        /* If flux weakening is disabled, just use the MTPA result */
        f4_idq_ref[0] = f4_idq_ref_temp[0];
        f4_idq_ref[1] = f4_idq_ref_temp[1];
    }

    /* Prevent positive d-axis current reference (for non-salient PMSM, id_ref should be <= 0) */
    if (f4_idq_ref[0] > 0.0F)
    {
        f4_idq_ref[0] = 0.0F;
    }

    /* Update current references in the control instance */
    p_instance_ctrl->p_to_outer_copy->i_d_ref = f4_idq_ref[0];
    p_instance_ctrl->p_to_outer_copy->i_q_ref = f4_idq_ref[1];
}

/*******************************************************************************************************************//**
 * @brief Flux weakening control function.
 *
 * This function applies flux weakening control to adjust d- and q-axis current references
 * for high-speed motor operation, ensuring the voltage limit is not exceeded.
 * The function overwrites the dq-axis current commands when flux weakening is enabled.
 *
 * @param[in,out] p_ctrl  Pointer to motor outer control instance
 * @param[in]     p_cfg   Pointer to extended configuration
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_fnctbl_fw_ctrl (void * p_ctrl, void * p_cfg)
{
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = (motor_pm_foc_outer_instance_ctrl_t *) p_ctrl;
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_cfg;

    FSP_PARAMETER_NOT_USED(p_cfg);

    if (MOTOR_OUTER_FW_THRESHOLD_SPEED_MECH < fabsf(p_instance_ctrl->signals.speed_mech_lpf))
    {
        /* This function will over-write the dq-axis current command */
        rm_motor_fluxwkn_set_vamax(&(p_instance_ctrl->signals.flux_weakening_ctrl),
                                   p_instance_ctrl->p_from_inner->v_ampl_max);
        rm_motor_fluxwkn_run(&(p_instance_ctrl->signals.flux_weakening_ctrl),
                             p_extended_cfg->pm_motor_parameter,
                             p_instance_ctrl->signals.speed_mech_lpf,
                             p_instance_ctrl->p_from_inner->i_d,
                             p_instance_ctrl->p_from_inner->i_q,
                             &(p_instance_ctrl->p_to_outer_copy->i_d_ref),
                             &(p_instance_ctrl->p_to_outer_copy->i_q_ref));
    }
    else
    {
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_PM_FOC)
 **********************************************************************************************************************/
