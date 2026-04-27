/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <bsp_api.h>
#include "rm_motor_fundlib.h"
#include "rm_motor_opl2less.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/
float rm_motor_opl2less_torque_current_calc(const motor_parameter_ramp_t * p_motor,
                                            float                          f4_opl2less_sw_time,
                                            float                          f4_ol_id_ref,
                                            float                          f4_phase_err_rad_lpf);
float rm_motor_opl2less_iq_calc(float f4_ed, float f4_eq, float f4_id, float f4_torque_current, float f4_phase_err);

/***********************************************************************************************************************
 * @brief Calculates the Iq reference for sensor-less switch control in motor applications.
 *
 * This function determines the appropriate q-axis current (Iq) reference value during the transition
 * from open-loop to sensor-less control. It uses motor electrical parameters and phase error
 * to compute the required Iq for maintaining desired torque and proper synchronization.
 *
 * @param[in] f4_ed             d-axis Back Electromotive Force (BEMF)
 * @param[in] f4_eq             q-axis Back Electromotive Force (BEMF)
 * @param[in] f4_id             d-axis current reference
 * @param[in] f4_torque_current Torque current in open-loop mode
 * @param[in] f4_phase_err      Phase error [rad] between the real motor axis and controlled axis
 *
 * @return                      Calculated Iq reference value
 ***********************************************************************************************************************/
float rm_motor_opl2less_iq_calc (float f4_ed, float f4_eq, float f4_id, float f4_torque_current, float f4_phase_err)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp_iq_ref;

    f4_temp0       = f4_ed / f4_eq;
    f4_temp0       = f4_id * f4_temp0;
    f4_temp1       = f4_torque_current / cosf(f4_phase_err);
    f4_temp_iq_ref = f4_temp1 - f4_temp0;

    return f4_temp_iq_ref;
}                                      /* End of function rm_motor_opl2less_iq_calc */

/***********************************************************************************************************************
 * @brief Calculates the torque current required to eliminate phase error during the transition from open-loop to sensor-less (FOC) control.
 *
 * This function determines the necessary torque-producing current based on the motor's physical parameters, the transition duration,
 * the reference d-axis current in open-loop mode, and the phase error (filtered with a low-pass filter). It helps ensure smooth
 * synchronization by compensating for accumulated phase error when switching control modes.
 *
 * @param[in] p_motor              Pointer to motor parameter/ramp data structure (motor_parameter_ramp_t)
 * @param[in] f4_opl2less_sw_time  Transition period [s] from open-loop to sensor-less (FOC) control
 * @param[in] f4_ol_id_ref         d-axis current reference in open-loop drive mode
 * @param[in] f4_phase_err_rad_lpf Phase error [rad], low-pass filtered
 *
 * @return     Torque current required [A] for phase error compensation during transition
 ***********************************************************************************************************************/
float rm_motor_opl2less_torque_current_calc (const motor_parameter_ramp_t * p_motor,
                                             float                          f4_opl2less_sw_time,
                                             float                          f4_ol_id_ref,
                                             float                          f4_phase_err_rad_lpf)
{
    float f4_temp0;
    float f4_temp1;
    float f4_delta_i_gamma;
    float f4_init_phase_err_rad;
    float f4_opl_torque_current;

    f4_init_phase_err_rad = f4_phase_err_rad_lpf;

    /* Torque current calculation for switching open-loop to sensor-less */
    f4_temp0              = p_motor->f4_mtr_j / ((float) p_motor->u2_mtr_pp * p_motor->f4_mtr_m);
    f4_temp1              = f4_opl2less_sw_time * f4_opl2less_sw_time;
    f4_temp1              = (MOTOR_FUNDLIB_MULTIPLE_2 * f4_init_phase_err_rad) / f4_temp1;
    f4_delta_i_gamma      = f4_temp0 * f4_temp1;
    f4_opl_torque_current = (f4_ol_id_ref * sinf(f4_init_phase_err_rad)) + f4_delta_i_gamma;

    return f4_opl_torque_current;
}                                      /* End of function rm_motor_opl2less_torque_current_calc */
