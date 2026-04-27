/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup MOTOR_SENSOR_BEMF
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_SENSOR_BEMF_H
#define RM_MOTOR_SENSOR_BEMF_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_sensor_api.h"
#include "rm_motor_shared_pos_speed_data.h"
#include "rm_motor_shared_data_foc.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** For switching open-loop to sensorless FOC */
typedef struct st_motor_ol_adjust
{
    motor_bemf_observer_t bemf_obs_ctrl;           ///< BEMF observer structure
    motor_pll_est_t       pll_est_ctrl;            ///< PLL estimator structure
    uint8_t               activate_ol_adjust_mode; ///< Open loop adjust mode activation flag (1: activated)
    uint8_t               pll_engage_status;       ///< PLL engage status (0: not engaged, 1: engaged)
    float                 pll_start_speed;         ///< Speed threshold for PLL activation [rad/s electrical]
    float                 speed_el_pll;            ///< PLL estimated electrical speed [rad/s]
    float                 angle_el_pll;            ///< PLL estimated electrical angle [rad]
    float                 e_d;                     ///< Estimated d-axis back-EMF voltage [V]
    float                 e_q;                     ///< Estimated q-axis back-EMF voltage [V]
    float                 angle_err_el;            ///< Estimated electrical angle error [rad]
} motor_ol_adjust_t;

/** Design parameter for open-loop damping*/
typedef struct st_motor_mcsensor_design_params
{
    float ed_hpf_omega;                ///< Natural frequency[Hz] for HPF in open-loop damping gain design
    float ol_damping_zeta;             ///< Damping ratio for open-loop damping gain design
} motor_mcsensor_design_params_t;

/** Extended configuration for sensor bemf module */
typedef struct st_motor_sensor_bemf_extended_cfg
{
    motor_oldamp_t                 ol_param;         ///< Parameter for open-loop damping
    motor_oldamp_sub_t             ol_sub_param;     ///< Sub parameter for open-loop damping
    motor_mcsensor_design_params_t d_param;          ///< Design parameter for speed control
    motor_openloop_damping_t       openloop_damping; ///< *Openloop damping active flag
    motor_mcsensor_less_switch_t   less_switch;      ///< *Soft switching active flag

    float e_obs_omega;                               ///< Natural frequency of BEMF observer [Hz]
    float e_obs_zeta;                                ///< Damping ratio of BEMF observer
    float pll_est_omega;                             ///< Natural frequency of PLL speed estimate loop [Hz]
    float pll_est_zeta;                              ///< Damping ratio of PLL speed estimate loop
} motor_sensor_bemf_extended_cfg_t;

/** Signals for BEMF sensorless control */
typedef struct st_motor_sensor_bemf_instance_signals_ctrl
{
    float speed_el;                             ///< Estimated electrical speed [rad/s]
    float angle_el;                             ///< Estimated electrical angle [rad]
    float inner_control_period;                 ///< Inner control cycle period [sec]
    float outer_control_period;                 ///< Outer control cycle period [sec]

    float e_d;                                  ///< Estimated d-axis back-EMF voltage [V]
    float e_q;                                  ///< Estimated q-axis back-EMF voltage [V]
    float angle_err_el;                         ///< Estimated electrical angle error [rad]

    uint8_t speed_pi_status_flag;               ///< Flag for speed pi ctrl
    uint8_t pll_status_flag;                    ///< Flag to start pll

    /* Openloop damping related valiable */
    uint8_t flag_id_up_executed;                ///< Initial rotor pole position detection executed flag
    uint8_t flag_down_to_ol;                    ///< Open-loop drive transition flag
    float   ol_iq_down_step;                    ///< Q-axis current reference ramping down rate [A/control period]
    float   phase_err_rad_lpf;                  ///< LPF value of phase error [rad]
    float   init_phase_err_rad;                 ///< Initialization value of phase error
    float   opl_torque_current;                 ///< Torque current (Iq) in sensor-less switching [A]
    float   speed_el_damp_comp;                 ///< Speed compensation for open-loop damping [rad/s]
    float   speed_el_lpf;                       ///< Low-pass filtered electrical speed [rad/s]
    uint8_t state_id_ref;                       ///< D-axis current command state
    uint8_t state_iq_ref;                       ///< Q-axis current command state
    float   speed_ref_el_ctrl;                  ///< Command electrical speed value [rad/s]
    motor_shared_pos_speed_to_inner_t to_inner; ///< Structure of data to be sent to inner loop

    motor_oldamp_sub_t    openloop_ctrl;        ///< Open-loop damping control structure
    motor_bemf_observer_t bemf_obs_ctrl;        ///< BEMF observer control structure
    motor_pll_est_t       pll_est_ctrl;         ///< PLL estimator control structure
    motor_ol_adjust_t     ol_adjust_ctrl;       ///< BEMF and PLL adjust/tuning structure for open-loop mode
} motor_sensor_bemf_instance_signals_ctrl_t;

/** BEMF sensor instance control block */
typedef struct st_motor_sensor_bemf_instance_ctrl
{
    uint32_t open;                                               ///< module open flag
    motor_sensor_bemf_instance_signals_ctrl_t      signals;      ///< Signals and controls structure
    const motor_shared_inner_to_position_speed_t * p_from_inner; ///< pointer to data from inner loop
    motor_sensor_cfg_t const * p_cfg;                            ///< Angle related configuration structure
} motor_sensor_bemf_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const motor_sensor_api_t g_motor_sensor_on_motor_sensor_bemf;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_Open(motor_sensor_ctrl_t * const p_ctrl, motor_sensor_cfg_t const * const p_cfg);

fsp_err_t RM_MOTOR_SENSOR_BEMF_Close(motor_sensor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_SENSOR_BEMF_Reset(motor_sensor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_SENSOR_BEMF_ParameterUpdate(motor_sensor_ctrl_t * const p_ctrl, motor_sensor_cfg_t const * p_cfg);

fsp_err_t RM_MOTOR_SENSOR_BEMF_AngleSpeedInit(motor_sensor_ctrl_t * const          p_ctrl,
                                              float                             ** pp_speed,
                                              float                             ** pp_angle,
                                              motor_sensor_data_exchange_t * const p_out_extra_data);

fsp_err_t RM_MOTOR_SENSOR_BEMF_AngleSpeedCalc(motor_sensor_ctrl_t * const        p_ctrl,
                                              motor_sensor_input_inner_t * const p_input_inner,
                                              motor_sensor_input_outer_t * const p_input_outer);

fsp_err_t RM_MOTOR_SENSOR_BEMF_PositionSpeedGet(motor_sensor_ctrl_t * const p_ctrl,
                                                float * const               p_pos,
                                                float * const               p_speed);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_SENSOR_BEMF_H

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_SENSOR_BEMF)
 **********************************************************************************************************************/
