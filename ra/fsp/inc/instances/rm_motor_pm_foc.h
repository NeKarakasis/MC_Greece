/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_PM_FOC
 * @{
 **********************************************************************************************************************/

#ifndef RM_PM_FOC_H
#define RM_PM_FOC_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_tuner_api.h"
#include "rm_motor_algorithm_api.h"
#include "rm_motor_shared_data_foc.h"
#include "rm_motor_shared_pos_speed_data.h"
#include "rm_motor_pm_foc_outer_fnc.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_PM_FOC_SHARED_BUFFER_COUNT    (2U) ///< Number of double-buffered shared data structures

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Error information */
typedef enum e_rm_motor_pm_foc_error
{
    MOTOR_PM_FOC_ERROR_NONE                     = 0x0000,
    MOTOR_PM_FOC_ERROR_OVER_CURRENT_HW          = 0x0001, /* Hardware Detection entered */
    MOTOR_PM_FOC_ERROR_OVER_VOLTAGE             = 0x0002, /* vdc High voltage */
    MOTOR_PM_FOC_ERROR_OVER_SPEED               = 0x0004, /* The motor's speed limit has been exceeded */
    MOTOR_PM_FOC_ERROR_HALL_TIMEOUT             = 0x0008, /* Unable to get value from hall sensor */
    MOTOR_PM_FOC_ERROR_BEMF_TIMEOUT             = 0x0010, /* Back electromotive force cannot be obtained from the sensor */
    MOTOR_PM_FOC_ERROR_HALL_PATTERN             = 0x0020, /* The hall pattern is different from what was expected */
    MOTOR_PM_FOC_ERROR_BEMF_PATTERN             = 0x0040, /* The back electromotive force reception pattern is different from what was expected */
    MOTOR_PM_FOC_ERROR_LOW_VOLTAGE              = 0x0080, /* vdc low voltage */
    MOTOR_PM_FOC_ERROR_OVER_CURRENT_SW          = 0x0100, /* u,v,w, Which one is over limit */
    MOTOR_PM_FOC_ERROR_INDUCTION_CORRECT        = 0x0200, /* The motor inductance setting is incorrect */
    MOTOR_PM_FOC_ERROR_ABNOMAL_OFFSET_VALUE     = 0x0400, /* Current offset value (result) is exceeding n% or more */
    MOTOR_PM_FOC_ERROR_PHASE_CONNECTION_FAILURE = 0x0800, /* Detects the difference of Duty waveform and current waveform */
    MOTOR_PM_FOC_ERROR_DISCONNECTION_DETECTION  = 0x1000, /* Current value always 0A when duty outputs */
    MOTOR_PM_FOC_ERROR_MTPA_INCORRECT           = 0x2000, /* MTPA Parameters incorrect error */
    MOTOR_PM_FOC_ERROR_FLUX_WEAKENING           = 0x4000, /* Flux weakening error */
    MOTOR_PM_FOC_ERROR_UNKNOWN                  = 0xFFFF, /* Does not exist error */
} motor_error_pm_foc_t;

/** Configuration parameters. */
typedef struct st_motor_pm_foc_inner_cfg
{
    void const * p_context;            ///< Placeholder for user data
    void const * p_extend;             ///< Pointer to extended configuration
} motor_pm_foc_inner_cfg_t;

/** Configuration parameters. */
typedef struct st_pm_foc_motor_outer_cfg
{
    void const * p_context;            ///< Placeholder for user data.
    void const * p_extend;             ///< Pointer to extended configuration

    /* Callback function to perform at trajectory interrupt */
    void (* p_trajectory_callback)(void const * p_context, float * p_speed_ref);
} motor_pm_foc_outer_cfg_t;

/** Sample delay compensation enable/disable selection */
typedef enum  e_motor_inner_sample_delay_compensation
{
    MOTOR_INNER_SAMPLE_DELAY_COMPENSATION_DISABLE = 0, ///< Sample delay compensation disabled
    MOTOR_INNER_SAMPLE_DELAY_COMPENSATION_ENABLE  = 1  ///< Sample delay compensation enabled
} motor_inner_sample_delay_compensation_t;

/** Sensor slot selection for inner control */
typedef enum  e_motor_inner_sensor_selected_slot
{
    MOTOR_INNER_SLOT_1_ENABLE = 0,     ///< Use sensor slot 1
    MOTOR_INNER_SLOT_2_ENABLE = 1      ///< Use sensor slot 2
} motor_inner_sensor_selected_slot_t;

/** Structure to retrieve motor voltage references */
typedef struct st_motor_inner_get_voltage
{
    float v_u_ref;                     ///< U phase voltage reference [V]
    float v_v_ref;                     ///< V phase voltage reference [V]
    float v_w_ref;                     ///< W phase voltage reference [V]
    float v_d_ref;                     ///< d-axis voltage reference
    float v_q_ref;                     ///< q-axis voltage reference
} motor_inner_get_voltage_t;

/** Control mode data structure from inner to outer loop */
typedef struct st_motor_mode_inner_to_outer
{
    float   id_ref_ow;                 ///< d-axis current reference overwrite value [A]
    float   iq_ref_ow;                 ///< q-axis current reference overwrite value [A]
    uint8_t mode_outer;                ///< Control mode specification for outer loop
} motor_mode_inner_to_outer_t;

/** Outer loop function pointer type definition */
typedef void (OuterLoopFunction)(void * p_ctrl, void * p_cfg);

/** Outerloop Function Pointer Table */
typedef struct motor_pm_foc_outer_loop_func_table
{
    OuterLoopFunction * control_mode_ctrl; ///< Position/Speed control function pointer
    OuterLoopFunction * mtpa_ctrl;         ///< MTPA (Maximum Torque Per Ampere) control function pointer
    OuterLoopFunction * fw_ctrl;           ///< Fluxweak Control function pointer
    OuterLoopFunction * spdobsv_ctrl;      ///< Speed Observer function pointer
} motor_pm_foc_outer_loop_func_table_t;

/** Design parameter structure for outer control */
typedef struct motor_outer_design_params
{
    float speed_omega;                 ///< Natural frequency[Hz] for speed loop gain design
    float speed_zeta;                  ///< Damping ratio for speed loop gain design
    float observer_omega;              ///< Natural frequency[Hz] for speed observer and extended observer
    float observer_zeta;               ///< Damping ratio for speed observer
} motor_outer_design_params_t;

/** Outer control status enumeration */
typedef enum  e_motor_outer_ctrl_status
{
    MOTOR_SPEED_CTRL_STATUS_INIT = 0,  ///< Speed control initialization state
    MOTOR_SPEED_CTRL_STATUS_BOOT,      ///< Speed control bootstrap state
    MOTOR_SPEED_CTRL_STATUS_RUN,       ///< Speed control running state
} motor_outer_ctrl_status_t;

/** Motor outer control instance control block */
typedef struct st_motor_pm_foc_outer_instance_signals_ctrl
{
    uint32_t error_info;                         ///< Error flags detected during outer loop execution
    uint8_t  active;                             ///< Flag to set active/inactive the speed control
    float    i_q_ref_disturbance;                ///< Anti disturbance Q-axis current reference [A]
    float    speed_mech_que_ref;                 ///< Command speed value for speed PI control[rad/s]
    float    speed_mech_ref;                     ///< Speed reference value [rad/s]
    float    speed_mech_lpf;                     ///< Low-pass filtered speed [rad/s]
    float    speed_mech;                         ///< Mechanical Speed used by outer control loop [rad/s]
    float    angle_mech;                         ///< Mechanical Angle Position used by outer control loop [rad]

    /* Flux Weakening related variables */
    motor_flux_weakening_t flux_weakening_ctrl;  ///< Flux-weakening control data

    motor_mode_inner_to_outer_t inner_mode_data; ///< control mode data from inner loop (request to overwrite references)
    pi_ctrl_t speed_pi_ctrl;                     ///< Speed PI controller data

    /* Speed observer related */
    motor_common_lpf_t  speed_lpf_ctrl;          ///< Speed low-pass filter data
    motor_spdobserver_t speed_observer_ctrl;     ///< Speed observer data
    motor_extobserver_t extobserver_ctrl;        ///< Disturbance torque/speed observer data
} motor_pm_foc_outer_instance_signals_ctrl_t;

typedef struct st_motor_pm_foc_outer_instance_ctrl
{
    motor_pm_foc_outer_instance_signals_ctrl_t signals;                                     ///< Motor outer control instance control block
    motor_pm_foc_outer_cfg_t const           * p_cfg;                                       ///< Pointer to configuration structure

    /* cyclic timer callback */
    timer_callback_args_t timer_args;                                                       ///< Cyclic timer callback

    motor_sensor_instance_t const * p_sensor_active_instance;                               ///< Sensor instance used for controlling the motor

    /* pointers to shared data */
    const motor_shared_inner_to_outer_foc_t * p_from_inner;                                 ///< Pointer to Input data structure
    motor_shared_outer_to_inner_foc_t       * p_to_inner[MOTOR_PM_FOC_SHARED_BUFFER_COUNT]; ///< Pointer to Output data structure -- two pointers to manage inner loop interrupting outer loop
    motor_shared_outer_to_inner_foc_t       * p_to_outer_active;                            ///< Active output pointer
    motor_shared_outer_to_inner_foc_t       * p_to_outer_copy;                              ///< Buffered pointer
} motor_pm_foc_outer_instance_ctrl_t;

/** Motor inner control instance control block */
typedef struct st_motor_pm_foc_inner_instance_signals_ctrl
{
    uint32_t error_info;                                 ///< Error flags detected during inner loop execution
    uint8_t  active;                                     ///< Flag to set active/inactive the current control
    motor_algorithm_control_mode_t control_mode;         ///< Current control mode

    motor_shared_inner_to_position_speed_t to_pos_speed; ///< Data structure for sensor module (it includes iu, iv, iw)
    float v_dc;                                          ///< DC bus voltage [V]
    float angle_el;                                      ///< Electrical rotor angle [radian]
    float speed_el;                                      ///< Electrical rotor speed [radian/s]
    float v_u_ref;                                       ///< U phase voltage reference [V]
    float v_v_ref;                                       ///< V phase voltage reference [V]
    float v_w_ref;                                       ///< W phase voltage reference [V]

    pi_ctrl_t               id_pi_ctrl;                  ///< D-axis current PI controller
    pi_ctrl_t               iq_pi_ctrl;                  ///< Q-axis current PI controller
    motor_volt_error_comp_t vcomp_ctrl;                  ///< Voltage error compensation data
} motor_pm_foc_inner_instance_signals_ctrl_t;

typedef struct st_motor_pm_foc_inner_instance_ctrl
{
    motor_pm_foc_inner_instance_signals_ctrl_t signals;                                     ///< Motor inner control instance control block
    motor_pm_foc_inner_cfg_t const           * p_cfg;                                       ///< Pointer to configuration structure

    motor_sensor_instance_t const     * p_sensor_instance;                                  ///< Angle sensor instance (Slot 1)
    motor_sensor_instance_t const     * p_sensor_instance2;                                 ///< Angle sensor instance (Slot 2)
    motor_hal_driver_instance_t const * p_hal_driver_instance;                              ///< Motor driver instance

    /* pointers to shared data */
    float * p_angle_el;                                                                     ///< Pointer to electrical angle (connected to sensor module)
    float * p_speed_el;                                                                     ///< Pointer to electrical speed (connected to sensor module)
    const motor_shared_pos_speed_to_inner_t * p_from_pos_speed;                             ///< Pointer to data from position speed to inner

    /* Sensor monitor pointers */
    float * p_speed_el_monitor;                                                             ///< Pointer to Speed monitoring value [rad/s]
    float * p_angle_el_monitor;                                                             ///< Pointer to Rotor angle monitoring value [radian]
    const motor_shared_outer_to_inner_foc_t * p_from_outer;                                 ///< Pointer to Input data structure
    motor_shared_inner_to_outer_foc_t       * p_to_outer[MOTOR_PM_FOC_SHARED_BUFFER_COUNT]; ///< Pointer to Output data structure
    motor_shared_inner_to_outer_foc_t       * p_to_outer_active;                            ///< Active output pointer
    motor_shared_inner_to_outer_foc_t       * p_to_outer_copy;                              ///< Output data working pointer(p_output[0], p_output[1] selected)
} motor_pm_foc_inner_instance_ctrl_t;

/** Extended configurations for motor PM FOC control */
typedef struct st_motor_pm_foc_extended_data_cfg
{
    float comp_v[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE];               ///< Voltage error compensation table: voltage axis
    float comp_i[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE];               ///< Voltage error compensation table: current axis
    float comp_vdc_ref;                                               ///< Voltage error compensation: DC bus reference voltage for table
    float comp_limit_factor;                                          ///< Voltage error compensation: limit factor
    motor_volt_error_comp_select_t          vcomp_enable;             ///< Enable/disable voltage error compensation
    motor_inner_sample_delay_compensation_t sample_delay_comp_enable; ///< Enable/disable sample delay compensation
    motor_inner_sensor_selected_slot_t      sensor_slot;              ///< Selected sensor slot
    float period_magnitude_value;                                     ///< Period magnification value for sampling delay compensation
    float inner_control_period;                                       ///< Current control period [usec]
    float current_limit;                                              ///< Current limit [A]

    float outer_control_period;                                       ///< Speed control period [sec]
    float speed_mech_change_limit;                                    ///< Maximum speed reference change rate [rad/s^2]
    float speed_mech_cmd_limit;                                       ///< Maximum speed command value [rad/s]
    float speed_lpf_omega_t;                                          ///< Speed LPF design value
    float iq_limit;                                                   ///< Limit of q-axis current [A]

    /* Error detection limits */
    float overcurrent_limit;                                          ///< Over-current detection limit [A]
    float overvoltage_limit;                                          ///< Over-voltage detection limit [V]
    float lowvoltage_limit;                                           ///< Low-voltage detection limit [V]
    float overspeed_limit;                                            ///< Over-speed detection limit [rpm]
} motor_pm_foc_extended_data_cfg_t;

typedef struct st_motor_pm_foc_extended_cfg
{
    motor_pm_foc_extended_data_cfg_t     data;
    motor_pm_foc_inner_instance_ctrl_t * pm_foc_inner_instance_ctrl; ///< Inner control instance
    motor_pm_foc_outer_instance_ctrl_t * pm_foc_outer_instance_ctrl; ///< Outer control instance

    motor_parameter_ramp_t      * pm_motor_parameter;                ///< Motor parameters
    motor_design_param_t        * pm_inner_design_parameter;         ///< Inner loop design parameters
    motor_outer_design_params_t * pm_outer_design_parameter;         ///< Design parameter for speed control

    motor_shared_buffer_foc_t          * p_shared_data;              ///< Pointer to shared data buffer
    motor_pm_foc_outer_loop_func_table_t outerfunc_table;            ///< Outerloop Function Pointer Table

    /* Callback function */
    void (* p_callback)(motor_algorithm_callback_args_t * p_args);   ///< User callback function

    motor_tuner_instance_t const   * p_tuner_instance;               ///< Tuner instance for parameter adjustment
    motor_pm_foc_outer_cfg_t const * p_motor_outer_cfg;              ///< Outer loop configuration
    motor_pm_foc_inner_cfg_t const * p_motor_inner_cfg;              ///< Inner loop configuration
} motor_pm_foc_extended_cfg_t;

/** PM FOC control instance control block */
typedef struct st_motor_pm_foc_instance_ctrl
{
    uint32_t open;                                              ///< Vector control open flag
    uint8_t  sensor_control_mode;                               ///< Sensor control mode
    uint8_t  motor_status;                                      ///< Motor Status
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl; ///< Pointer to inner control instance
    motor_pm_foc_outer_instance_ctrl_t * p_outer_instance_ctrl; ///< Pointer to outer control instance
    motor_algorithm_cfg_t const        * p_cfg;
} motor_pm_foc_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in interface API structure for this instance. */
extern const motor_algorithm_api_t g_motor_algorithm_on_motor_pm_foc;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Open(motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_cfg_t const * const p_cfg);
fsp_err_t RM_MOTOR_PM_FOC_Close(motor_algorithm_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_PM_FOC_Reset(motor_algorithm_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_PM_FOC_Run(motor_algorithm_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_PM_FOC_Stop(motor_algorithm_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_PM_FOC_ControlModeGet(motor_algorithm_ctrl_t * const   p_ctrl,
                                         motor_algorithm_control_mode_t * p_control_mode);
fsp_err_t RM_MOTOR_PM_FOC_ControlModeSet(motor_algorithm_ctrl_t * const p_ctrl,
                                         motor_algorithm_control_mode_t control_mode);
fsp_err_t RM_MOTOR_PM_FOC_StatusGet(motor_algorithm_ctrl_t * const p_ctrl, uint8_t * const p_status);
fsp_err_t RM_MOTOR_PM_FOC_ParameterUpdate(motor_algorithm_ctrl_t * const      p_ctrl,
                                          motor_algorithm_cfg_t const * const p_cfg);
fsp_err_t RM_MOTOR_PM_FOC_ErrorGet(motor_algorithm_ctrl_t * const p_ctrl, uint32_t * p_error_flags);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_PM_FOC_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_PM_FOC)
 **********************************************************************************************************************/
