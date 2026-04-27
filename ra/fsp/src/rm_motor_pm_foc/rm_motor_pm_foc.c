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
#include "rm_motor_pm_foc_internal.h"
#include "rm_motor_pm_foc_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "FOC" in ASCII, used to determine if channel is open. */
#define MOTOR_FOC_OPEN    (0x00464F43ULL)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
const motor_algorithm_api_t g_motor_algorithm_on_motor_pm_foc =
{
    .open            = RM_MOTOR_PM_FOC_Open,
    .close           = RM_MOTOR_PM_FOC_Close,
    .reset           = RM_MOTOR_PM_FOC_Reset,
    .run             = RM_MOTOR_PM_FOC_Run,
    .stop            = RM_MOTOR_PM_FOC_Stop,
    .controlModeGet  = RM_MOTOR_PM_FOC_ControlModeGet,
    .controlModeSet  = RM_MOTOR_PM_FOC_ControlModeSet,
    .statusGet       = RM_MOTOR_PM_FOC_StatusGet,
    .parameterUpdate = RM_MOTOR_PM_FOC_ParameterUpdate,
    .errorGet        = RM_MOTOR_PM_FOC_ErrorGet,
};

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_PM_FOC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/********************************************************************************************************************//**
 * @brief Opens and configures the Vector control module. Implements @ref motor_driver_api_t::open.
 *
 * @retval FSP_SUCCESS              Motor Driver successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Open (motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
#endif

    motor_pm_foc_extended_cfg_t * p_extended_cfg = (motor_pm_foc_extended_cfg_t *) p_cfg->p_extend;

    p_pm_foc_instance_ctrl->p_inner_instance_ctrl = p_extended_cfg->pm_foc_inner_instance_ctrl;
    p_pm_foc_instance_ctrl->p_outer_instance_ctrl = p_extended_cfg->pm_foc_outer_instance_ctrl;
    p_pm_foc_instance_ctrl->p_cfg                 = p_cfg;

    rm_motor_inner_init(p_pm_foc_instance_ctrl);
    rm_motor_outer_init(p_pm_foc_instance_ctrl);

    p_pm_foc_instance_ctrl->open = MOTOR_FOC_OPEN; // Vector Control Open

    /* Motor Status initialized */
    p_pm_foc_instance_ctrl->motor_status        = 0L;
    p_pm_foc_instance_ctrl->sensor_control_mode = 0L;

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_Open */

/********************************************************************************************************************//**
 * @brief Disables specified Motor Current Module.
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Close (motor_algorithm_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    rm_motor_inner_close(p_pm_foc_instance_ctrl->p_inner_instance_ctrl);
    rm_motor_outer_close(p_pm_foc_instance_ctrl);

    p_pm_foc_instance_ctrl->sensor_control_mode = 0L; // Control mode notified by the sensor stack
    p_pm_foc_instance_ctrl->motor_status        = 0L;
    p_pm_foc_instance_ctrl->open                = 0L; // Vector Control Close

    return err;
} /* End of function RM_MOTOR_PM_FOC_Close */

/********************************************************************************************************************//**
 * @brief Disables specified Motor Current Module.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Reset (motor_algorithm_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    rm_motor_inner_reset(p_pm_foc_instance_ctrl->p_inner_instance_ctrl);
    rm_motor_outer_reset(p_pm_foc_instance_ctrl);

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_Reset */

/********************************************************************************************************************//**
 * @brief Active run motor.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Run (motor_algorithm_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_pm_foc_instance_ctrl->p_cfg->p_extend;

    p_extended_cfg->pm_foc_inner_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_SET;
    p_extended_cfg->pm_foc_outer_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_SET;

    motor_algorithm_cfg_t       * p_foc_cfg         = (motor_algorithm_cfg_t *) p_pm_foc_instance_ctrl->p_cfg;
    motor_hal_driver_instance_t * p_driver_instance =
        (motor_hal_driver_instance_t *) p_foc_cfg->p_motor_hal_driver_instance;

    p_driver_instance->p_api->activate(p_driver_instance->p_ctrl);

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_Run */

/********************************************************************************************************************//**
 * @brief Motor Stop.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_Stop (motor_algorithm_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    /* Motor driving is stopped by reset processing. */
    rm_motor_inner_reset(p_pm_foc_instance_ctrl->p_inner_instance_ctrl);
    rm_motor_outer_reset(p_pm_foc_instance_ctrl);

    motor_algorithm_cfg_t       * p_foc_cfg         = (motor_algorithm_cfg_t *) p_pm_foc_instance_ctrl->p_cfg;
    motor_hal_driver_instance_t * p_driver_instance =
        (motor_hal_driver_instance_t *) p_foc_cfg->p_motor_hal_driver_instance;

    p_driver_instance->p_api->deactivate(p_driver_instance->p_ctrl);

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_Stop */

/********************************************************************************************************************//**
 * @brief ControlModeGet.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_ControlModeGet (motor_algorithm_ctrl_t * const   p_ctrl,
                                          motor_algorithm_control_mode_t * p_control_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ASSERT(NULL != p_control_mode);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    /* Get control mode from inner instance */
    *p_control_mode = p_pm_foc_instance_ctrl->p_inner_instance_ctrl->signals.control_mode;

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_ControlModeGet */

/********************************************************************************************************************//**
 * @brief ControlModeSet.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_ControlModeSet (motor_algorithm_ctrl_t * const p_ctrl,
                                          motor_algorithm_control_mode_t control_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    /* Set control mode to inner instance */
    p_pm_foc_instance_ctrl->p_inner_instance_ctrl->signals.control_mode = control_mode;

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_ControlModeSet */

/********************************************************************************************************************//**
 * @brief StatusGet.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_StatusGet (motor_algorithm_ctrl_t * const p_ctrl, uint8_t * const p_status)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    /* under consideration */
    *p_status = p_pm_foc_instance_ctrl->motor_status;

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_StatusGet */

/********************************************************************************************************************//**
 * @brief Updates the parameters of the motor control module.
 *
 * This function overwrites the configuration pointer of the motor control instance and updates managed variables
 * such as current limits in the inner instance. It does not modify any gain parameters directly, but ensures
 * that the new configuration is applied to the instance.
 *
 * @param[in]  p_ctrl   Pointer to the motor algorithm control structure.
 * @param[in]  p_cfg    Pointer to the new motor algorithm configuration structure.
 *
 * @retval FSP_SUCCESS              Successfully updated parameters.
 * @retval FSP_ERR_ASSERTION        Null pointer detected.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_ParameterUpdate (motor_algorithm_ctrl_t * const      p_ctrl,
                                           motor_algorithm_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

    /* Overwrite p_cfg */
    p_pm_foc_instance_ctrl->p_cfg = p_cfg;

    /* Update each function gain parameters */
    rm_motor_inner_parameter_update(p_pm_foc_instance_ctrl, p_pm_foc_instance_ctrl->p_cfg->p_extend);
    rm_motor_outer_parameter_update(p_pm_foc_instance_ctrl);

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_ParameterUpdate */

/********************************************************************************************************************//**
 * @brief ErrorGet.
 *
 * @retval FSP_SUCCESS              Successfully.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 ***********************************************************************************************************************/
fsp_err_t RM_MOTOR_PM_FOC_ErrorGet (motor_algorithm_ctrl_t * const p_ctrl, uint32_t * p_error_flags)
{
    fsp_err_t err            = FSP_SUCCESS;
    uint32_t  u4_error_flags = MOTOR_PM_FOC_ERROR_NONE;

    motor_pm_foc_instance_ctrl_t * p_pm_foc_instance_ctrl = (motor_pm_foc_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_PM_FOC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_pm_foc_instance_ctrl);
    FSP_ASSERT(NULL != p_error_flags);
    FSP_ERROR_RETURN(p_pm_foc_instance_ctrl->open == MOTOR_FOC_OPEN, FSP_ERR_NOT_OPEN);
#endif

/* Check inner loop errors */
    rm_motor_inner_error_get(p_pm_foc_instance_ctrl->p_inner_instance_ctrl, &u4_error_flags);
    *p_error_flags |= u4_error_flags;

    /* Check outer loop errors */
    rm_motor_outer_error_get(p_pm_foc_instance_ctrl->p_outer_instance_ctrl, &u4_error_flags);
    *p_error_flags |= u4_error_flags;

    return err;
}                                      /* End of function RM_MOTOR_PM_FOC_ErrorGet */

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_PM_FOC)
 **********************************************************************************************************************/
