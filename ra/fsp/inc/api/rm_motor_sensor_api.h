/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_MOTOR_INTERFACES
 * @defgroup MOTOR_SENSOR_API Motor sensor Interface
 * @brief Interface for motor angle and speed estimation functions.
 *
 * @section MOTOR_SENSOR_API_Summary Summary
 * The Motor Sensor interface provides rotor angle and rotational speed estimation
 * from various sensor types (BEMF observer, Hall sensors, etc.).
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_SENSOR_API_H
#define RM_MOTOR_SENSOR_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Includes board and MCU related header files. */
#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Motor sensor control block. Allocate an instance specific control block to pass into the API calls. */
typedef void motor_sensor_ctrl_t;

/** Motor sensor input and output data structure for inner loop. */
typedef void motor_sensor_data_exchange_t;

/** Motor sensor output data structure for extra data to inner loop. */
typedef void motor_sensor_output_t;

/** Motor sensor input data structure from inner loop. */
typedef void motor_sensor_input_inner_t;

/** Motor sensor input data structure from outer loop. */
typedef void motor_sensor_input_outer_t;

/** Configuration parameters. */
typedef struct st_motor_sensor_cfg
{
    void const * p_context;            ///< Placeholder for user data
    void const * p_extend;             ///< Pointer to extended configuration structure
} motor_sensor_cfg_t;

/** Functions implemented as application interface will follow these APIs. */
typedef struct st_motor_sensor_api
{
    /** Initialize the Motor_Angle.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure.
     */
    fsp_err_t (* open)(motor_sensor_ctrl_t * const p_ctrl, motor_sensor_cfg_t const * const p_cfg);

    /** Close (Finish) the Motor_Angle.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(motor_sensor_ctrl_t * const p_ctrl);

    /** Reset the Motor_Angle.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* reset)(motor_sensor_ctrl_t * const p_ctrl);

    /** Update Parameters for the calculation in the Motor_Angle.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure include update parameters.
     */
    fsp_err_t (* parameterUpdate)(motor_sensor_ctrl_t * const p_ctrl, motor_sensor_cfg_t const * p_cfg);

    /** Set angle_speed data pointers.
     *
     * @param[in]  p_ctrl                 Pointer to motor angle control structure.
     * @param[out] pp_speed               Pointer to output variable for speed pointer.
     * @param[out] pp_angle               Pointer to output variable for angle pointer.
     * @param[out] p_out_extra_data       Pointer to extra output data structure pointer.
     */
    fsp_err_t (* angleSpeedInit)(motor_sensor_ctrl_t * const p_ctrl, float ** pp_speed, float ** pp_angle,
                                 motor_sensor_data_exchange_t * const p_out_extra_data);                            

    /** Calculate rotor angle and rotational speed of Motor_Angle.
     *
     * @param[in] p_ctrl        Pointer to control structure.
     * @param[in] p_input_inner Pointer to input data from inner loop
     * @param[in] p_input_outer Pointer to input data from outer loop
     */
    fsp_err_t (* angleSpeedCalc)(motor_sensor_ctrl_t * const p_ctrl, motor_sensor_input_inner_t * const p_input_inner,
                                 motor_sensor_input_outer_t * const p_input_outer);

    /** Get rotor mechanical position and speed from the Motor_Sensor.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_pos        Pointer to mechanical position.
     * @param[out] p_speed      Pointer to mechanical speed.
     */
    fsp_err_t (* positionSpeedGet)(motor_sensor_ctrl_t * const p_ctrl, float * const p_pos, float * const p_speed);
} motor_sensor_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_motor_sensor_instance
{
    motor_sensor_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    motor_sensor_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    motor_sensor_api_t const * p_api;  ///< Pointer to the API structure for this instance
} motor_sensor_instance_t;

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_SENSOR_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RM_MOTOR_SENSOR_API_H */
