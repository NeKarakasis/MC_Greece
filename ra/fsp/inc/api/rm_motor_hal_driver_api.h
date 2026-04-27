/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_MOTOR_INTERFACES
 * @defgroup MOTOR_HAL_DRIVER_API Motor Hal Driver Interface
 * @brief Interface for motor hal driver functions.
 *
 * @section MOTOR_HAL_DRIVER_API_Summary Summary
 * The Motor driver interface for setting the PWM modulation duty
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_HAL_DRIVER_API_H
#define RM_MOTOR_HAL_DRIVER_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#ifndef CCAL_FLOAT
 #define CCAL_FLOAT
#endif
#include "rm_motor_shared_ccal_types.h"

/* Register definitions, common services and error codes. */
#include "bsp_api.h"
#include "r_adc_api.h"
#include "r_three_phase_api.h"
#include "r_elc_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_HAL_DRIVER_EVENT_CURRENT    (1U) ///< Event Current Control timing

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Selection of shunt type */
typedef enum e_motor_hal_driver_shunt_type
{
    MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT = 1, ///< Only use U phase current
    MOTOR_HAL_DRIVER_SHUNT_TYPE_2_SHUNT,     ///< Use U and W phase current
    MOTOR_HAL_DRIVER_SHUNT_TYPE_3_SHUNT      ///< Use all phase current
} motor_hal_driver_shunt_type_t;

/** Callback function parameter data */
typedef struct st_motor_hal_driver_callback_args
{
    uint8_t event;                     ///< Event trigger
    void  * p_context;                 ///< Placeholder for user data.
} motor_hal_driver_callback_args_t;

/** Current Data Get Structure */
typedef struct st_motor_hal_driver_current_dataset
{
    ccal_num_t i_u;                    ///< U phase current [A]
    ccal_num_t i_v;                    ///< V phase current [A]
    ccal_num_t i_w;                    ///< W phase current [A]
    ccal_num_t v_dc;                   ///< Main Line Voltage [V]
    ccal_num_t va_max;                 ///< maximum magnitude of voltage vector
} motor_hal_driver_current_dataset_t;

/** Voltage Data Get Structure */
typedef struct st_motor_hal_driver_voltage_dataset
{
    ccal_num_t v_u;                    ///< U phase voltage [V]
    ccal_num_t v_v;                    ///< V phase voltage [V]
    ccal_num_t v_w;                    ///< W phase voltage [V]
} motor_hal_driver_voltage_dataset_t;

/** Control block.  Allocate an instance specific control block to pass into the API calls.
 */
typedef void motor_hal_driver_ctrl_t;

/** Configuration parameters. */
typedef struct st_motor_hal_driver_cfg
{
    adc_instance_t const * p_adc_instance;                          ///< ADC instance for ADC module
    adc_instance_t const * p_adc2_instance;                         ///< ADC2 instance for ADC module

    three_phase_instance_t const * p_three_phase_instance;          ///< Three_phase timer instance

    void (* p_callback)(motor_hal_driver_callback_args_t * p_args); ///< User callback function
    void * p_context;                                               ///< Placeholder for user data.

    void const * p_extend;                                          ///< Pointer to extended configuration
} motor_hal_driver_cfg_t;

/** Functions implemented at the HAL layer will follow these APIs. */
typedef struct st_motor_hal_driver_api
{
    /** Initialize the Motor Driver Module.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure.
     */
    fsp_err_t (* open)(motor_hal_driver_ctrl_t * const p_ctrl, motor_hal_driver_cfg_t const * const p_cfg);

    /** Close the Motor Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(motor_hal_driver_ctrl_t * const p_ctrl);

    /** Reset variables of the Motor Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* reset)(motor_hal_driver_ctrl_t * const p_ctrl);

    /** Set (Input) Phase Voltage data into the Motor Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  u_voltage    U phase voltage [V]
     * @param[in]  v_voltage    V phase voltage [V]
     * @param[in]  w_voltage    W phase voltage [V]
     */
    fsp_err_t (* phaseVoltageSet)(motor_hal_driver_ctrl_t * const p_ctrl, ccal_num_t const u_voltage,
                                  ccal_num_t const v_voltage, ccal_num_t const w_voltage);

    /** Set Active Flag into Motor Hal Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* activate)(motor_hal_driver_ctrl_t * const p_ctrl);

    /** Clear Active Flag into Motor Hal Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* deactivate)(motor_hal_driver_ctrl_t * const p_ctrl);

    /** Get Phase current, Vdc and Va_max data from the Motor Driver Module
     *
     * @param[in]  p_ctrl           Pointer to control structure.
     * @param[out] p_current_get    Pointer to get data structure.
     */
    fsp_err_t (* currentGet)(motor_hal_driver_ctrl_t * const            p_ctrl,
                             motor_hal_driver_current_dataset_t * const p_current_dataset);

    /** Get Phase voltage from the Motor Driver Module
     *
     * @param[in]  p_ctrl           Pointer to control structure.
     * @param[out] p_voltage_get    Pointer to get data structure.
     */
    fsp_err_t (* outputVoltageGet)(motor_hal_driver_ctrl_t * const            p_ctrl,
                                   motor_hal_driver_voltage_dataset_t * const p_voltage_dataset);

    /** Get the flag of finish current offset detection from the Motor Driver Module
     *
     * @param[in]  p_ctrl           Pointer to control structure.
     * @param[out] p_offset_flag    Flag of finish current offset detection
     */
    fsp_err_t (* flagCurrentOffsetGet)(motor_hal_driver_ctrl_t * const p_ctrl, uint8_t * const p_offset_flag);

    /** Get the flag of finish bootstrap from the Motor Driver Module
     *
     * @param[in]  p_ctrl           Pointer to control structure.
     * @param[out] p_bootstrap_flag Flag of finish bootstrap
     */
    fsp_err_t (* flagBootstrapGet)(motor_hal_driver_ctrl_t * const p_ctrl, uint8_t * const p_bootstrap_flag);

    /** Restart current offset detection
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* currentOffsetRestart)(motor_hal_driver_ctrl_t * const p_ctrl);

    /** Update Configuration Parameters for the calculation in the Motor Driver Module
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure include update parameters.
     */
    fsp_err_t (* parameterUpdate)(motor_hal_driver_ctrl_t * const p_ctrl, motor_hal_driver_cfg_t const * const p_cfg);

    /** Charge the Bootstrap circuit
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* bootstrapCharge)(motor_hal_driver_ctrl_t * const p_ctrl);
} motor_hal_driver_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_motor_hal_driver_instance
{
    motor_hal_driver_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    motor_hal_driver_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    motor_hal_driver_api_t const * p_api;  ///< Pointer to the API structure for this instance
} motor_hal_driver_instance_t;

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_HAL_DRIVER_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RM_MOTOR_HAL_DRIVER_API_H */
