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
#include "rm_motor_mc_lib.h"
#include "rm_motor_hal_driver.h"
#include "rm_motor_hal_driver_modulation.h"
#include "r_gpt.h"

#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)
 #include "r_adc_b.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define     MOTOR_HAL_DRIVER_OPEN                   (0X4D445241L)
#define     MOTOR_HAL_DRIVER_SHARED_ADC_OPEN        (('M' << 24U) | ('T' << 16U) | ('S' << 8U) | ('A' << 0U))

#define     MOTOR_HAL_DRIVER_ADC_DATA_MASK          (0x00000FFF)
#define     MOTOR_HAL_DRIVER_SHIFT_MASK             (0x000FU)

#define MOTOR_HAL_DRIVER_IO_PORT_CFG_LOW            (0x3000004) /* Set I/O port with low output */
#define MOTOR_HAL_DRIVER_IO_PORT_CFG_HIGH           (0x3000005) /* Set I/O port with high output*/
#define MOTOR_HAL_DRIVER_IO_PORT_PERIPHERAL_MASK    (0x0010000) /* Mask for pin to operate as a peripheral pin */
#define MOTOR_HAL_DRIVER_IO_PORT_GPIO_MASK          (0xFFEFFFF) /* Mask for pin to operate as a GPIO pin */

#ifndef MOTOR_HAL_DRIVER_ERROR_RETURN
 #define    MOTOR_HAL_DRIVER_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void rm_motor_hal_driver_cyclic(adc_callback_args_t * p_args);
static void rm_motor_hal_driver_1shunt_cyclic(timer_callback_args_t * p_args);

#if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 1)
static void rm_motor_hal_driver_shared_cyclic(adc_callback_args_t * p_args);

#endif

#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)
static void rm_motor_hal_driver_adc_b_open(motor_hal_driver_instance_ctrl_t   * p_instance_ctrl,
                                           motor_hal_driver_cfg_t const * const p_cfg);

static void rm_motor_hal_driver_adc_b_close(motor_hal_driver_instance_ctrl_t * p_instance_ctrl);

#else
static void rm_motor_hal_driver_adc_open(motor_hal_driver_instance_ctrl_t   * p_instance_ctrl,
                                         motor_hal_driver_cfg_t const * const p_cfg);

static void rm_motor_hal_driver_adc_close(motor_hal_driver_instance_ctrl_t * p_instance_ctrl);

#endif

static void rm_motor_hal_driver_reset(motor_hal_driver_instance_ctrl_t * p_ctrl);
static void rm_motor_hal_driver_set_uvw_duty(motor_hal_driver_instance_ctrl_t * p_ctrl,
                                             ccal_num_t                         duty_u,
                                             ccal_num_t                         duty_v,
                                             ccal_num_t                         duty_w);
static void rm_motor_hal_driver_current_get(motor_hal_driver_instance_ctrl_t * p_ctrl);
static void rm_motor_hal_driver_1shunt_current_get(motor_hal_driver_instance_ctrl_t * p_ctrl);
static void rm_motor_hal_driver_modulation(motor_hal_driver_instance_ctrl_t * p_ctrl);
static void rm_motor_hal_driver_1shunt_modulation(motor_hal_driver_instance_ctrl_t * p_ctrl);

static void rm_motor_hal_driver_pin_cfg(bsp_io_port_pin_t pin, uint32_t cfg);
static void rm_motor_hal_driver_output_enable(motor_hal_driver_instance_ctrl_t * p_ctrl);
static void rm_motor_hal_driver_output_disable(motor_hal_driver_instance_ctrl_t * p_ctrl);

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
const motor_hal_driver_api_t g_motor_hal_driver_on_motor_hal_driver =
{
    .open                 = RM_MOTOR_HAL_DRIVER_Open,
    .close                = RM_MOTOR_HAL_DRIVER_Close,
    .reset                = RM_MOTOR_HAL_DRIVER_Reset,
    .phaseVoltageSet      = RM_MOTOR_HAL_DRIVER_PhaseVoltageSet,
    .activate             = RM_MOTOR_HAL_DRIVER_Activate,
    .deactivate           = RM_MOTOR_HAL_DRIVER_Deactivate,
    .currentGet           = RM_MOTOR_HAL_DRIVER_CurrentGet,
    .outputVoltageGet     = RM_MOTOR_HAL_DRIVER_OutputVoltageGet,
    .flagCurrentOffsetGet = RM_MOTOR_HAL_DRIVER_FlagCurrentOffsetGet,
    .flagBootstrapGet     = RM_MOTOR_HAL_DRIVER_FlagBootstrapGet,
    .currentOffsetRestart = RM_MOTOR_HAL_DRIVER_CurrentOffsetRestart,
    .bootstrapCharge      = RM_MOTOR_HAL_DRIVER_ChargeBootstrap,
    .parameterUpdate      = RM_MOTOR_HAL_DRIVER_ParameterUpdate,
};

/*******************************************************************************************************************//**
 * @addtogroup MOTOR_HAL_DRIVER
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Opens and configures the Motor Driver module. Implements @ref motor_hal_driver_api_t::open.
 *
 * @retval FSP_SUCCESS              Motor Driver successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Open (motor_hal_driver_ctrl_t * const p_ctrl, motor_hal_driver_cfg_t const * const p_cfg)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
#endif

    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_extended_cfg);
#endif

    FSP_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->carrier_base   = p_extended_cfg->carrier_base;
    p_instance_ctrl->deadtime_count = p_extended_cfg->deadtime;

    rm_motor_hal_driver_reset(p_instance_ctrl);

    p_instance_ctrl->modulation_method = (uint8_t) p_extended_cfg->modulation_method;
    p_instance_ctrl->st_modulation     = p_extended_cfg->mod_param;
    rm_motor_hal_driver_modulation_set_max_duty(&(p_instance_ctrl->st_modulation), p_extended_cfg->mod_param.max_duty);
    rm_motor_hal_driver_modulation_set_min_duty(&(p_instance_ctrl->st_modulation), p_extended_cfg->mod_param.min_duty);

    /* Start GPT three phase module */
    if (p_cfg->p_three_phase_instance != NULL)
    {
        /* For Port setting */
        three_phase_instance_t const * p_three_phase;
        timer_instance_t const       * p_u_phase_gpt;
        timer_cfg_t const            * p_u_phase_gpt_cfg;
        gpt_extended_cfg_t const     * p_u_phase_gpt_extend;

        p_three_phase        = p_cfg->p_three_phase_instance;
        p_u_phase_gpt        = p_three_phase->p_cfg->p_timer_instance[p_extended_cfg->trigger_phase];
        p_u_phase_gpt_cfg    = p_u_phase_gpt->p_cfg;
        p_u_phase_gpt_extend = p_u_phase_gpt_cfg->p_extend;

        if (p_u_phase_gpt_extend->gtioca.stop_level == GPT_PIN_LEVEL_LOW)
        {
            p_instance_ctrl->gtioca_low_cfg = MOTOR_HAL_DRIVER_IO_PORT_CFG_LOW;
        }
        else
        {
            p_instance_ctrl->gtioca_low_cfg = MOTOR_HAL_DRIVER_IO_PORT_CFG_HIGH;
        }

        if (p_u_phase_gpt_extend->gtiocb.stop_level == GPT_PIN_LEVEL_HIGH)
        {
            p_instance_ctrl->gtiocb_low_cfg = MOTOR_HAL_DRIVER_IO_PORT_CFG_LOW;
        }
        else
        {
            p_instance_ctrl->gtiocb_low_cfg = MOTOR_HAL_DRIVER_IO_PORT_CFG_HIGH;
        }

        p_cfg->p_three_phase_instance->p_api->open(p_cfg->p_three_phase_instance->p_ctrl,
                                                   p_cfg->p_three_phase_instance->p_cfg);

        if (MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT == p_extended_cfg->shunt)
        {
            p_u_phase_gpt->p_api->callbackSet(p_u_phase_gpt->p_ctrl,
                                              rm_motor_hal_driver_1shunt_cyclic,
                                              p_instance_ctrl,
                                              &(p_instance_ctrl->timer_callback_args));
        }
        else
        {
            rm_motor_hal_driver_set_uvw_duty(p_instance_ctrl,
                                             p_instance_ctrl->st_modulation.neutral_duty,
                                             p_instance_ctrl->st_modulation.neutral_duty,
                                             p_instance_ctrl->st_modulation.neutral_duty);
        }

        if (p_extended_cfg->port_up != 0)
        {
            rm_motor_hal_driver_output_disable(p_instance_ctrl);
        }

        p_cfg->p_three_phase_instance->p_api->start(p_cfg->p_three_phase_instance->p_ctrl);
    }

    /* Start ADC module */
#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)
    rm_motor_hal_driver_adc_b_open(p_instance_ctrl, p_cfg);
#else                                  /* ADC_B_SUPPORT == 0 (for adc) */
    rm_motor_hal_driver_adc_open(p_instance_ctrl, p_cfg);
#endif                                 /* ADC_B_SUPPORT == 0 (for adc) */

    /* Mark driver as open */
    p_instance_ctrl->open = MOTOR_HAL_DRIVER_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Disables specified Motor Driver Module. Implements @ref motor_hal_driver_api_t::close.
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Close (motor_hal_driver_ctrl_t * const p_ctrl)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_hal_driver_cfg_t          * p_cfg          = (motor_hal_driver_cfg_t *) p_instance_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    if (p_extended_cfg->port_up != 0)
    {
        rm_motor_hal_driver_output_disable(p_instance_ctrl);
    }

    rm_motor_hal_driver_reset(p_instance_ctrl);

    /* Close ADC module */
#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)
    rm_motor_hal_driver_adc_b_close(p_instance_ctrl);
#else                                  /* ADC_B_SUPPORTED == 0 (for adc) */
    rm_motor_hal_driver_adc_close(p_instance_ctrl);
#endif                                 /* ADC_B_SUPPORTED == 0 (for adc) */

    /* Close GPT three phase module */
    if (p_cfg->p_three_phase_instance != NULL)
    {
        p_cfg->p_three_phase_instance->p_api->close(p_cfg->p_three_phase_instance->p_ctrl);
    }

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Reset variables of Motor Driver Module. Implements @ref motor_hal_driver_api_t::reset.
 *
 * @retval FSP_SUCCESS              Successfully reset.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Reset (motor_hal_driver_ctrl_t * const p_ctrl)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_hal_driver_cfg_t          * p_cfg          = (motor_hal_driver_cfg_t *) p_instance_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    p_instance_ctrl->ref_u = p_instance_ctrl->st_modulation.neutral_duty;
    p_instance_ctrl->ref_v = p_instance_ctrl->st_modulation.neutral_duty;
    p_instance_ctrl->ref_w = p_instance_ctrl->st_modulation.neutral_duty;

    rm_motor_hal_driver_set_uvw_duty(p_instance_ctrl,
                                     p_instance_ctrl->st_modulation.neutral_duty,
                                     p_instance_ctrl->st_modulation.neutral_duty,
                                     p_instance_ctrl->st_modulation.neutral_duty);

    if (p_extended_cfg->port_up != 0)
    {
        rm_motor_hal_driver_output_disable(p_instance_ctrl);
    }

    rm_motor_hal_driver_reset(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set Phase Voltage Data to calculate PWM duty. Implements @ref motor_hal_driver_api_t::phaseVoltageSet
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_PhaseVoltageSet (motor_hal_driver_ctrl_t * const p_ctrl,
                                               ccal_num_t const                u_voltage,
                                               ccal_num_t const                v_voltage,
                                               ccal_num_t const                w_voltage)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->ref_u = u_voltage;
    p_instance_ctrl->ref_v = v_voltage;
    p_instance_ctrl->ref_w = w_voltage;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set Active flag. Implements @ref motor_hal_driver_api_t::activate
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Activate (motor_hal_driver_ctrl_t * const p_ctrl)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->active = MOTOR_FUNDLIB_FLAG_SET;

    if ((p_instance_ctrl->offset_calc_flag == MOTOR_FUNDLIB_FLAG_SET) && (p_instance_ctrl->port_enable_flag == 1U))
    {
        rm_motor_hal_driver_output_enable(p_instance_ctrl);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Clear Active flag. Implements @ref motor_hal_driver_api_t::deactivate
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Deactivate (motor_hal_driver_ctrl_t * const p_ctrl)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->active = MOTOR_FUNDLIB_FLAG_CLEAR;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Get calculated phase Current, Vdc & Va_max data. Implements @ref motor_hal_driver_api_t::currentGet
 *
 * @retval FSP_SUCCESS              Successful data get.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Input parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_CurrentGet (motor_hal_driver_ctrl_t * const            p_ctrl,
                                          motor_hal_driver_current_dataset_t * const p_current_dataset)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_HAL_DRIVER_ERROR_RETURN(p_current_dataset != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_current_dataset->i_u = p_instance_ctrl->i_u_ad;
    p_current_dataset->i_v = p_instance_ctrl->i_v_ad;
    p_current_dataset->i_w = p_instance_ctrl->i_w_ad;

    p_current_dataset->v_dc   = p_instance_ctrl->v_dc_ad;
    p_current_dataset->va_max = rm_motor_hal_driver_modulation_get_vamax(&(p_instance_ctrl->st_modulation),
                                                                         p_instance_ctrl->modulation_method);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Get calculated phase Voltage data. Implements @ref motor_hal_driver_api_t::outputVoltageGet
 *
 * @retval FSP_SUCCESS              Successful data get.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Input parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_OutputVoltageGet (motor_hal_driver_ctrl_t * const            p_ctrl,
                                                motor_hal_driver_voltage_dataset_t * const p_voltage_dataset)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_HAL_DRIVER_ERROR_RETURN(p_voltage_dataset != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_voltage_dataset->v_u = p_instance_ctrl->v_u_ad;
    p_voltage_dataset->v_v = p_instance_ctrl->v_v_ad;
    p_voltage_dataset->v_w = p_instance_ctrl->v_w_ad;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Get the flag of finish current offset detection. Implements @ref motor_hal_driver_api_t::flagCurrentOffsetGet
 *
 * @retval FSP_SUCCESS              Successful data get.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Input parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_FlagCurrentOffsetGet (motor_hal_driver_ctrl_t * const p_ctrl,
                                                    uint8_t * const                 p_offset_flag)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_HAL_DRIVER_ERROR_RETURN(p_offset_flag != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif

    *p_offset_flag = p_instance_ctrl->offset_calc_flag;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Restart the current offset detection. Implements @ref motor_hal_driver_api_t::currentOffsetRestart
 *
 * @retval FSP_SUCCESS              Successfully restarted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_CurrentOffsetRestart (motor_hal_driver_ctrl_t * const p_ctrl)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->offset_calc_flag  = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_instance_ctrl->offset_calc_times = 0U;
    p_instance_ctrl->sum_i_u_ad        = 0;
    p_instance_ctrl->sum_i_v_ad        = 0;
    p_instance_ctrl->sum_i_w_ad        = 0;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Update the parameters of Driver Module. Implements @ref motor_hal_driver_api_t::parameterUpdate
 *
 * @retval FSP_SUCCESS              Successfully data was updated.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_ParameterUpdate (motor_hal_driver_ctrl_t * const      p_ctrl,
                                               motor_hal_driver_cfg_t const * const p_cfg)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ASSERT(p_cfg);
#endif

    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->carrier_base   = p_extended_cfg->carrier_base;
    p_instance_ctrl->deadtime_count = p_extended_cfg->deadtime;

    p_instance_ctrl->st_modulation = p_extended_cfg->mod_param;
    rm_motor_hal_driver_modulation_set_max_duty(&(p_instance_ctrl->st_modulation), p_extended_cfg->mod_param.max_duty);
    rm_motor_hal_driver_modulation_set_min_duty(&(p_instance_ctrl->st_modulation), p_extended_cfg->mod_param.min_duty);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Get the flag of finish bootstrap. Implements @ref motor_hal_driver_api_t::flagBootstrapGet
 *
 * @retval FSP_SUCCESS              Successful data get.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Input parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_FlagBootstrapGet (motor_hal_driver_ctrl_t * const p_ctrl,
                                                uint8_t * const                 p_bootstrap_flag)
{
    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_HAL_DRIVER_ERROR_RETURN(p_bootstrap_flag != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif

    *p_bootstrap_flag = p_instance_ctrl->charge_bootstrap_flag;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Charge the Bootstrap circuit. Implements @ref motor_hal_driver_api_t::bootstrapCharge
 *
 * @retval FSP_SUCCESS              Successfully data was updated.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_ChargeBootstrap (motor_hal_driver_ctrl_t * const p_ctrl)
{
    ccal_num_t duty_out;
    uint16_t   u2_bootstrap_time;

    motor_hal_driver_instance_ctrl_t * p_instance_ctrl = (motor_hal_driver_instance_ctrl_t *) p_ctrl;

#if MOTOR_HAL_DRIVER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_HAL_DRIVER_ERROR_RETURN(MOTOR_HAL_DRIVER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_hal_driver_cfg_t          * p_cfg          = (motor_hal_driver_cfg_t *) p_instance_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    /*========================*/
    /*    Charge Bootstrap    */
    /*========================*/
    u2_bootstrap_time = p_extended_cfg->charge_bootstrap_time;

    p_instance_ctrl->charge_bootstrap_cnt++;
    if (u2_bootstrap_time <= p_instance_ctrl->charge_bootstrap_cnt)
    {
        p_instance_ctrl->charge_bootstrap_flag = MOTOR_FUNDLIB_FLAG_SET;
        p_instance_ctrl->charge_bootstrap_cnt  = u2_bootstrap_time;
    }
    else
    {
        duty_out = CCAL_DIV_BY_2(1);
        rm_motor_hal_driver_set_uvw_duty(p_ctrl, duty_out, duty_out, duty_out);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_HAL_DRIVER)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_reset
 * Description   : Resets driver variables
 * Arguments     : p_ctrl - The pointer to the motor driver instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_reset (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    motor_hal_driver_cfg_t const    * p_cfg        = p_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extend_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    p_ctrl->i_u_ad  = CCAL_ZERO;
    p_ctrl->i_v_ad  = CCAL_ZERO;
    p_ctrl->i_w_ad  = CCAL_ZERO;
    p_ctrl->v_dc_ad = CCAL_ZERO;
    p_ctrl->ref_u   = CCAL_ZERO;
    p_ctrl->ref_v   = CCAL_ZERO;
    p_ctrl->ref_w   = CCAL_ZERO;
    p_ctrl->v_u_ad  = CCAL_ZERO;
    p_ctrl->v_v_ad  = CCAL_ZERO;
    p_ctrl->v_w_ad  = CCAL_ZERO;

    p_ctrl->offset_calc_digit = (uint16_t) log2(p_extend_cfg->offset_calc_count);
    p_ctrl->offset_calc_flag  = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_ctrl->offset_calc_times = 0U;
    p_ctrl->offset_i_u        = 0;
    p_ctrl->offset_i_v        = 0;
    p_ctrl->offset_i_w        = 0;
    p_ctrl->sum_i_u_ad        = 0;
    p_ctrl->sum_i_v_ad        = 0;
    p_ctrl->sum_i_w_ad        = 0;

    p_ctrl->modulation_method = (uint8_t) p_extend_cfg->modulation_method;
    p_ctrl->maxvoltage_flag   = 0U;

    p_ctrl->st_modulation.saturation_flag = 0U;

    p_ctrl->charge_bootstrap_flag = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_ctrl->charge_bootstrap_cnt  = 0U;

    p_ctrl->active = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_ctrl->port_enable_flag = 1U;
}                                      /* End of function rm_motor_hal_driver_reset */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_set_uvw_duty
 * Description   : PWM duty setting
 * Arguments     : p_instance - Pointer to Motor Driver instance
 *                 duty_u - The duty cycle of Phase-U (0.0 - 1.0)
 *                 duty_v - The duty cycle of Phase-V (0.0 - 1.0)
 *                 duty_w - The duty cycle of Phase-W (0.0 - 1.0)
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_set_uvw_duty (motor_hal_driver_instance_ctrl_t * p_ctrl,
                                              ccal_num_t                         duty_u,
                                              ccal_num_t                         duty_v,
                                              ccal_num_t                         duty_w)
{
    motor_hal_driver_cfg_t const    * p_cfg         = p_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extend_cfg  = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
    three_phase_instance_t const    * p_three_phase = p_ctrl->p_cfg->p_three_phase_instance;

    uint32_t                 u4_count_u = 0U;
    uint32_t                 u4_count_v = 0U;
    uint32_t                 u4_count_w = 0U;
    three_phase_duty_cycle_t temp_duty;
    uint32_t                 u4_temp_base   = p_ctrl->carrier_base;
    uint16_t                 u2_temp_deadt2 = p_ctrl->deadtime_count >> 1;

    u4_count_u = u4_temp_base - (uint32_t) CCAL_MUL((ccal_num_t) u4_temp_base, duty_u, MTR_Q_DUTY) + u2_temp_deadt2;
    u4_count_v = u4_temp_base - (uint32_t) CCAL_MUL((ccal_num_t) u4_temp_base, duty_v, MTR_Q_DUTY) + u2_temp_deadt2;
    u4_count_w = u4_temp_base - (uint32_t) CCAL_MUL((ccal_num_t) u4_temp_base, duty_w, MTR_Q_DUTY) + u2_temp_deadt2;

    temp_duty.duty[0] = u4_count_u;
    temp_duty.duty[1] = u4_count_v;
    temp_duty.duty[2] = u4_count_w;

    if (MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT == p_extend_cfg->shunt)
    {
        temp_duty.duty_buffer[0] = u4_count_u;
        temp_duty.duty_buffer[1] = u4_count_v;
        temp_duty.duty_buffer[2] = u4_count_w;
    }

    if (p_three_phase != NULL)
    {
        p_three_phase->p_api->dutyCycleSet(p_three_phase->p_ctrl, &temp_duty);
    }
}                                      /* End of function rm_motor_hal_driver_set_uvw_duty */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_current_get
 * Description   : Get Iu/Iw & Vdc process
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_current_get (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    uint16_t u2_temp_offset_count                  = 0U;
    uint16_t u2_addata[7]                          = {0U};
    int32_t  s4_addata[7]                          = {0};
    motor_hal_driver_cfg_t const    * p_cfg        = p_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extend_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    adc_instance_t const * p_adc_instance;

#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)
 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    p_adc_instance = p_cfg->p_adc_instance;
 #else
    p_adc_instance = p_extend_cfg->p_shared_cfg->p_adc_instance_first;
 #endif

    /* Read A/D converted data */
    if (p_adc_instance != NULL)
    {
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_u_ad_ch, &u2_addata[0]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_v_ad_ch, &u2_addata[1]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_w_ad_ch, &u2_addata[2]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_dc_ad_ch, &u2_addata[3]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_u_ad_ch, &u2_addata[4]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_v_ad_ch, &u2_addata[5]);
        p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_w_ad_ch, &u2_addata[6]);
    }

#else                                               /* MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 0 */
 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    p_adc_instance = p_cfg->p_adc_instance;
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_u_ad_ch, &u2_addata[0]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_v_ad_ch, &u2_addata[1]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_w_ad_ch, &u2_addata[2]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_dc_ad_ch, &u2_addata[3]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_u_ad_ch, &u2_addata[4]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_v_ad_ch, &u2_addata[5]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_w_ad_ch, &u2_addata[6]);
 #else                                 /* SUPPORT_SHARED_ADC == 1 */
    /* Read A/D converted data */
    adc_instance_t const * pp_adc_instance[2] =
    {
        p_extend_cfg->p_shared_cfg->p_adc_instance_first,
        p_extend_cfg->p_shared_cfg->p_adc_instance_second,
    };

    p_adc_instance = pp_adc_instance[p_extend_cfg->i_u_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_u_ad_ch, &u2_addata[0]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->i_v_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_v_ad_ch, &u2_addata[1]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->i_w_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->i_w_ad_ch, &u2_addata[2]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->v_dc_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_dc_ad_ch, &u2_addata[3]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->v_u_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_u_ad_ch, &u2_addata[4]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->v_v_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_v_ad_ch, &u2_addata[5]);

    p_adc_instance = pp_adc_instance[p_extend_cfg->v_w_ad_unit];
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_w_ad_ch, &u2_addata[6]);
 #endif                                /* SUPPORT_SHARED_ADC == 1 */
#endif                                 /* ADC_B_SUPPORTED == 0 */

    s4_addata[0] = (int32_t) u2_addata[0];
    s4_addata[0] = s4_addata[0] - (int32_t) p_extend_cfg->ad_current_offset;

    s4_addata[2] = (int32_t) u2_addata[2];
    s4_addata[2] = s4_addata[2] - (int32_t) p_extend_cfg->ad_current_offset;

    if (MOTOR_HAL_DRIVER_SHUNT_TYPE_3_SHUNT == p_extend_cfg->shunt)
    {
        s4_addata[1] = (int32_t) u2_addata[1];
        s4_addata[1] = s4_addata[1] - (int32_t) p_extend_cfg->ad_current_offset;
    }
    else                               /* for 2SHUNT */
    {
        s4_addata[1] = -(s4_addata[0] + s4_addata[2]);
    }

    s4_addata[3] = (int32_t) u2_addata[3];
    s4_addata[3] = s4_addata[3] - (int32_t) (p_extend_cfg->ad_vdc_offset);

    s4_addata[4] = (int32_t) u2_addata[4];
    s4_addata[4] = s4_addata[4] - (int32_t) (p_extend_cfg->ad_voltage_offset);

    s4_addata[5] = (int32_t) u2_addata[5];
    s4_addata[5] = s4_addata[5] - (int32_t) (p_extend_cfg->ad_voltage_offset);

    s4_addata[6] = (int32_t) u2_addata[6];
    s4_addata[6] = s4_addata[6] - (int32_t) (p_extend_cfg->ad_voltage_offset);

    /* Measure current A/D offset error */
    if (MOTOR_FUNDLIB_FLAG_SET == p_ctrl->active)
    {
        if (MOTOR_FUNDLIB_FLAG_CLEAR == p_ctrl->offset_calc_flag)
        {
            u2_temp_offset_count = (uint16_t) (1U << ((p_ctrl->offset_calc_digit) & MOTOR_HAL_DRIVER_SHIFT_MASK));

            if (p_ctrl->offset_calc_times < u2_temp_offset_count)
            {
                p_ctrl->sum_i_u_ad += s4_addata[0];
                p_ctrl->sum_i_v_ad += s4_addata[1];
                p_ctrl->sum_i_w_ad += s4_addata[2];
                p_ctrl->offset_calc_times++;
            }
            else
            {
                p_ctrl->offset_i_u = p_ctrl->sum_i_u_ad >>
                                     (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK);
                p_ctrl->offset_i_v = p_ctrl->sum_i_v_ad >>
                                     (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK);
                p_ctrl->offset_i_w = p_ctrl->sum_i_w_ad >>
                                     (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK);
                p_ctrl->offset_calc_flag = MOTOR_FUNDLIB_FLAG_SET;

                /* Enable PWM output */
                rm_motor_hal_driver_output_enable(p_ctrl);
            }
        }
    }
    else
    {
        rm_motor_hal_driver_output_disable(p_ctrl);
    }

    /* Current offset error removal */
    if (MOTOR_FUNDLIB_FLAG_SET == p_ctrl->offset_calc_flag)
    {
        s4_addata[0] = s4_addata[0] - p_ctrl->offset_i_u;
        s4_addata[2] = s4_addata[2] - p_ctrl->offset_i_w;
        if (MOTOR_HAL_DRIVER_SHUNT_TYPE_3_SHUNT == p_extend_cfg->shunt)
        {
            s4_addata[1] = s4_addata[1] - p_ctrl->offset_i_v;
        }
        else                           /* 2SHUNT */
        {
            s4_addata[1] = -(s4_addata[0] + s4_addata[2]);
        }
    }

    p_ctrl->i_u_ad  = -CCAL_MUL((ccal_num_t) s4_addata[0], p_extend_cfg->sf_ad_current, 0);
    p_ctrl->i_v_ad  = -CCAL_MUL((ccal_num_t) s4_addata[1], p_extend_cfg->sf_ad_current, 0);
    p_ctrl->i_w_ad  = -CCAL_MUL((ccal_num_t) s4_addata[2], p_extend_cfg->sf_ad_current, 0);
    p_ctrl->v_dc_ad = CCAL_MUL((ccal_num_t) s4_addata[3], p_extend_cfg->sf_ad_vdc, 0);
    p_ctrl->v_u_ad  = CCAL_MUL((ccal_num_t) s4_addata[4], p_extend_cfg->sf_ad_voltage, 0);
    p_ctrl->v_v_ad  = CCAL_MUL((ccal_num_t) s4_addata[5], p_extend_cfg->sf_ad_voltage, 0);
    p_ctrl->v_w_ad  = CCAL_MUL((ccal_num_t) s4_addata[6], p_extend_cfg->sf_ad_voltage, 0);
}                                      /* End of function rm_motor_hal_driver_current_get */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_1shunt_current_get
 * Description   : Get Iu/Iw & Vdc process for 1shunt
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_1shunt_current_get (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    uint16_t u2_temp_offset_count                  = 0U;
    uint16_t u2_addata[4]                          = {0U};
    int32_t  s4_Iac_ad[2]                          = {0};
    int32_t  s4_Iac_ad2                            = 0;
    int32_t  s4_addata[7]                          = {0};
    uint16_t u2_Iac_raw0                           = 0U;
    uint16_t u2_Iac_raw1                           = 0U;
    motor_hal_driver_cfg_t const    * p_cfg        = p_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extend_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    adc_instance_t const * p_adc_instance;

#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)

    /* g_adc_b module */
 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    p_adc_instance = p_cfg->p_adc_instance;
 #else
    p_adc_instance = p_extend_cfg->p_shared_cfg->p_adc_instance_first;
 #endif

    adc_b_fifo_read_t temp_fifo;

    /* Using ADC_B module */
    /* Get Vdc */
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_dc_ad_ch, &u2_addata[0]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_u_ad_ch, &u2_addata[1]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_v_ad_ch, &u2_addata[2]);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, p_extend_cfg->v_w_ad_ch, &u2_addata[3]);

    /* Get FIFO data */
    R_ADC_B_FifoRead(p_adc_instance->p_ctrl, ADC_GROUP_MASK_0, &temp_fifo);
    temp_fifo.fifo_data[0].data &= MOTOR_HAL_DRIVER_ADC_DATA_MASK;
    u2_Iac_raw0                  = (uint16_t) temp_fifo.fifo_data[0].data;
    temp_fifo.fifo_data[1].data &= MOTOR_HAL_DRIVER_ADC_DATA_MASK;
    u2_Iac_raw1                  = (uint16_t) temp_fifo.fifo_data[1].data;
#else                                  /* ADC_B_SUPPORTED == 0 */
    /* g_adc module */
    adc_instance_t const * p_adc2_instance;
    adc_status_t           temp_adc_status;

 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    p_adc_instance  = p_cfg->p_adc_instance;
    p_adc2_instance = p_cfg->p_adc2_instance;
 #else
    p_adc_instance  = p_extend_cfg->p_shared_cfg->p_adc_instance_first;
    p_adc2_instance = p_extend_cfg->p_shared_cfg->p_adc_instance_second;
 #endif

    /* Get double buffer data */
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, ADC_CHANNEL_DUPLEX_A, &u2_Iac_raw0);
    p_adc_instance->p_api->read(p_adc_instance->p_ctrl, ADC_CHANNEL_DUPLEX_B, &u2_Iac_raw1);

    /* Read A/D converted data */
    if (p_adc2_instance != NULL)
    {
        /* When an AD unit is used to read 1shunt current and vdc,
         * once close the unit and reopen with other configuration. */
        if (p_extend_cfg->i_u_ad_unit == p_extend_cfg->v_dc_ad_unit)
        {
            /* Close first ADC instance */
            p_adc_instance->p_api->close(p_adc_instance->p_ctrl);

            /* Open second ADC instance */
            p_adc2_instance->p_api->open(p_adc2_instance->p_ctrl, p_adc2_instance->p_cfg);
            p_adc2_instance->p_api->scanCfg(p_adc2_instance->p_ctrl, p_adc2_instance->p_channel_cfg);
        }

        p_adc2_instance->p_api->scanStart(p_adc2_instance->p_ctrl);

        p_adc2_instance->p_api->scanStatusGet(p_adc2_instance->p_ctrl, &temp_adc_status);
        while (ADC_STATE_SCAN_IN_PROGRESS == temp_adc_status.state)
        {
            /* wait A/D conversion finish */
            p_adc2_instance->p_api->scanStatusGet(p_adc2_instance->p_ctrl, &temp_adc_status);
        }

        p_adc2_instance->p_api->read(p_adc2_instance->p_ctrl, p_extend_cfg->v_dc_ad_ch, &u2_addata[0]);
        p_adc2_instance->p_api->read(p_adc2_instance->p_ctrl, p_extend_cfg->v_u_ad_ch, &u2_addata[1]);
        p_adc2_instance->p_api->read(p_adc2_instance->p_ctrl, p_extend_cfg->v_v_ad_ch, &u2_addata[2]);
        p_adc2_instance->p_api->read(p_adc2_instance->p_ctrl, p_extend_cfg->v_w_ad_ch, &u2_addata[3]);

        /* When an AD unit is used to read 1shunt current and vdc,
         * once close the unit and reopen with other configuration again. */
        if (p_extend_cfg->i_u_ad_unit == p_extend_cfg->v_dc_ad_unit)
        {
            /* Close second ADC instance */
            p_adc2_instance->p_api->close(p_adc2_instance->p_ctrl);

            /* Open & Start first ADC instance again */
            p_adc_instance->p_api->open(p_adc_instance->p_ctrl, p_adc_instance->p_cfg);
            p_adc_instance->p_api->scanCfg(p_adc_instance->p_ctrl, p_adc_instance->p_channel_cfg);
            p_adc_instance->p_api->calibrate(p_adc_instance->p_ctrl, p_adc_instance->p_cfg->p_extend);
            p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);
        }
    }
#endif                                 /* ADC_B_SUPPORTED == 0 */

    /* Get main line voltage */
    s4_addata[3] = (int32_t) u2_addata[0];
    s4_addata[3] = s4_addata[3] - (int32_t) (p_extend_cfg->ad_vdc_offset);
    s4_addata[4] = (int32_t) u2_addata[1];
    s4_addata[4] = s4_addata[4] - (int32_t) (p_extend_cfg->ad_voltage_offset);
    s4_addata[5] = (int32_t) u2_addata[2];
    s4_addata[5] = s4_addata[5] - (int32_t) (p_extend_cfg->ad_voltage_offset);
    s4_addata[6] = (int32_t) u2_addata[3];
    s4_addata[6] = s4_addata[6] - (int32_t) (p_extend_cfg->ad_voltage_offset);

    p_ctrl->v_dc_ad = CCAL_MUL((ccal_num_t) s4_addata[3], p_extend_cfg->sf_ad_vdc, 0);
    p_ctrl->v_u_ad  = CCAL_MUL((ccal_num_t) s4_addata[4], p_extend_cfg->sf_ad_voltage, 0);
    p_ctrl->v_v_ad  = CCAL_MUL((ccal_num_t) s4_addata[5], p_extend_cfg->sf_ad_voltage, 0);
    p_ctrl->v_w_ad  = CCAL_MUL((ccal_num_t) s4_addata[6], p_extend_cfg->sf_ad_voltage, 0);

    /* Translate double buffer A/D data to 3 phase currents */
    s4_Iac_ad[0] = (int32_t) u2_Iac_raw0;
    s4_Iac_ad[1] = (int32_t) u2_Iac_raw1;
    s4_Iac_ad[0] = s4_Iac_ad[0] - (int32_t) p_extend_cfg->ad_current_offset;
    s4_Iac_ad[1] = -(s4_Iac_ad[1] - (int32_t) p_extend_cfg->ad_current_offset);

    /* Measure current offset error */
    if (MOTOR_FUNDLIB_FLAG_SET == p_ctrl->active)
    {
        if (MOTOR_FUNDLIB_FLAG_CLEAR == p_ctrl->offset_calc_flag)
        {
            u2_temp_offset_count = (uint16_t) (1U << (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK));
            if (p_ctrl->offset_calc_times < u2_temp_offset_count)
            {
                p_ctrl->sum_i_u_ad += s4_Iac_ad[0];
                p_ctrl->sum_i_w_ad += s4_Iac_ad[1];
                p_ctrl->offset_calc_times++;
            }
            else
            {
                p_ctrl->offset_i_u = p_ctrl->sum_i_u_ad >>
                                     (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK);
                p_ctrl->offset_i_w = p_ctrl->sum_i_w_ad >>
                                     (p_ctrl->offset_calc_digit & MOTOR_HAL_DRIVER_SHIFT_MASK);
                p_ctrl->offset_calc_flag = MOTOR_FUNDLIB_FLAG_SET;

                /* Enable PWM output */
                rm_motor_hal_driver_output_enable(p_ctrl);
            }
        }
    }
    else
    {
        rm_motor_hal_driver_output_disable(p_ctrl);
    }

    /* Current offset error removal */
    if (MOTOR_FUNDLIB_FLAG_SET == p_ctrl->offset_calc_flag)
    {
        s4_Iac_ad[0] = s4_Iac_ad[0] - p_ctrl->offset_i_u;
        s4_Iac_ad[1] = s4_Iac_ad[1] - p_ctrl->offset_i_w;
    }

    s4_Iac_ad2 = -(s4_Iac_ad[0] + s4_Iac_ad[1]);

    switch (p_ctrl->min_phase)
    {
        case MOTOR_HAL_DRIVER_PHASE_U_PHASE:
        {
            s4_addata[0] = s4_Iac_ad[0];
            if (MOTOR_HAL_DRIVER_PHASE_V_PHASE == p_ctrl->mid_phase)
            {
                s4_addata[2] = s4_Iac_ad[1];
                s4_addata[1] = s4_Iac_ad2;
            }
            else
            {
                s4_addata[1] = s4_Iac_ad[1];
                s4_addata[2] = s4_Iac_ad2;
            }

            break;
        }

        case MOTOR_HAL_DRIVER_PHASE_V_PHASE:
        {
            s4_addata[1] = s4_Iac_ad[0];
            if (MOTOR_HAL_DRIVER_PHASE_W_PHASE == p_ctrl->mid_phase)
            {
                s4_addata[0] = s4_Iac_ad[1];
                s4_addata[2] = s4_Iac_ad2;
            }
            else
            {
                s4_addata[2] = s4_Iac_ad[1];
                s4_addata[0] = s4_Iac_ad2;
            }

            break;
        }

        case MOTOR_HAL_DRIVER_PHASE_W_PHASE:
        {
            s4_addata[2] = s4_Iac_ad[0];
            if (MOTOR_HAL_DRIVER_PHASE_U_PHASE == p_ctrl->mid_phase)
            {
                s4_addata[1] = s4_Iac_ad[1];
                s4_addata[0] = s4_Iac_ad2;
            }
            else
            {
                s4_addata[0] = s4_Iac_ad[1];
                s4_addata[1] = s4_Iac_ad2;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    p_ctrl->i_u_ad = CCAL_MUL((ccal_num_t) s4_addata[0], p_extend_cfg->sf_ad_current, 0);
    p_ctrl->i_v_ad = CCAL_MUL((ccal_num_t) s4_addata[1], p_extend_cfg->sf_ad_current, 0);
    p_ctrl->i_w_ad = CCAL_MUL((ccal_num_t) s4_addata[2], p_extend_cfg->sf_ad_current, 0);
}                                      /* End of function rm_motor_hal_driver_1shunt_current_get */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_modulation
 * Description   : Perform PWM modulation
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_modulation (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    ccal_num_t v_in[3]    = {CCAL_ZERO};
    ccal_num_t mod_out[3] = {CCAL_ZERO};

    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    p_ctrl->st_modulation.v_dc = p_ctrl->v_dc_ad;
    v_in[0] = p_ctrl->ref_u;
    v_in[1] = p_ctrl->ref_v;
    v_in[2] = p_ctrl->ref_w;

    rm_motor_hal_driver_modulation_run(&(p_ctrl->st_modulation),
                                       &(p_extended_cfg->mod_param),
                                       &(v_in[0]),
                                       &(mod_out[0]),
                                       &p_ctrl->maxvoltage_flag,
                                       p_ctrl->modulation_method);

    rm_motor_hal_driver_set_uvw_duty(p_ctrl, mod_out[0], mod_out[1], mod_out[2]);
}                                      /* End of function rm_motor_hal_driver_modulation */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_1shunt_modulation
 * Description   : Perform PWM modulation for 1shunt
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_1shunt_modulation (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    ccal_num_t v_in[3]        = {CCAL_ZERO};
    ccal_num_t mod_out[3]     = {CCAL_ZERO};
    int32_t    s4_ref_max1    = 0;
    int32_t    s4_ref_max2    = 0;
    int32_t    s4_ref_mid1    = 0;
    int32_t    s4_ref_mid2    = 0;
    int32_t    s4_ref_temp    = 0;
    int32_t    s4_raw_InvPeak = 0;
    int32_t    s4_ref_max     = 0;
    int32_t    s4_ref_mid     = 0;
    int32_t    s4_ref_min     = 0;
    int32_t    s4_AD1_trigger = 10;
    int32_t    s4_AD2_trigger = 10;
    int32_t    s4_mu_ref1     = 0;
    int32_t    s4_mu_ref2     = 0;
    int32_t    s4_mv_ref1     = 0;
    int32_t    s4_mv_ref2     = 0;
    int32_t    s4_mw_ref1     = 0;
    int32_t    s4_mw_ref2     = 0;

    three_phase_duty_cycle_t temp_duty;

    motor_hal_driver_phase_t max_phase;
    motor_hal_driver_phase_t temp_phase;
    uint32_t                 u4_temp_base   = p_ctrl->carrier_base;
    uint16_t                 u2_temp_deadt2 = p_ctrl->deadtime_count >> 1;

    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    three_phase_instance_t const    * p_three_phase  = p_ctrl->p_cfg->p_three_phase_instance;
    timer_instance_t const          * timer_u        =
        p_three_phase->p_cfg->p_timer_instance[p_extended_cfg->trigger_phase];
    timer_info_t temp_info;

    if (p_three_phase != NULL)
    {
        timer_u->p_api->infoGet(timer_u->p_ctrl, &temp_info);
        s4_raw_InvPeak = (int32_t) temp_info.period_counts;

        p_ctrl->st_modulation.v_dc = p_ctrl->v_dc_ad;
        v_in[0] = p_ctrl->ref_u;
        v_in[1] = p_ctrl->ref_v;
        v_in[2] = p_ctrl->ref_w;

        rm_motor_hal_driver_modulation_run(&(p_ctrl->st_modulation), &(p_extended_cfg->mod_param), &(v_in[0]),
                                           &(mod_out[0]), &p_ctrl->maxvoltage_flag, p_ctrl->modulation_method);

/* Duty shift and trigger timing for 1shunt */
        s4_ref_max = (int32_t) (u4_temp_base - (uint32_t) ((ccal_num_t) u4_temp_base * mod_out[0]) + u2_temp_deadt2);
        s4_ref_mid = (int32_t) (u4_temp_base - (uint32_t) ((ccal_num_t) u4_temp_base * mod_out[1]) + u2_temp_deadt2);
        s4_ref_min = (int32_t) (u4_temp_base - (uint32_t) ((ccal_num_t) u4_temp_base * mod_out[2]) + u2_temp_deadt2);

        max_phase         = MOTOR_HAL_DRIVER_PHASE_U_PHASE;
        p_ctrl->mid_phase = MOTOR_HAL_DRIVER_PHASE_V_PHASE;
        p_ctrl->min_phase = MOTOR_HAL_DRIVER_PHASE_W_PHASE;

        /* Judge the phase value */
        if (s4_ref_max < s4_ref_mid)
        {
            s4_ref_temp       = s4_ref_max;
            s4_ref_max        = s4_ref_mid;
            s4_ref_mid        = s4_ref_temp;
            temp_phase        = max_phase;
            max_phase         = p_ctrl->mid_phase;
            p_ctrl->mid_phase = temp_phase;
        }

        if (s4_ref_max < s4_ref_min)
        {
            s4_ref_temp       = s4_ref_max;
            s4_ref_max        = s4_ref_min;
            s4_ref_min        = s4_ref_temp;
            temp_phase        = max_phase;
            max_phase         = p_ctrl->min_phase;
            p_ctrl->min_phase = temp_phase;
        }

        if (s4_ref_mid < s4_ref_min)
        {
            s4_ref_temp       = s4_ref_mid;
            s4_ref_mid        = s4_ref_min;
            s4_ref_min        = s4_ref_temp;
            temp_phase        = p_ctrl->mid_phase;
            p_ctrl->mid_phase = p_ctrl->min_phase;
            p_ctrl->min_phase = temp_phase;
        }

        /* Adjust middle PWM timing */
        if ((s4_ref_mid - s4_ref_min) < p_extended_cfg->difference_minimum)
        {
            s4_ref_mid1 = s4_ref_min + p_extended_cfg->difference_minimum;
            if (s4_ref_mid1 > (s4_raw_InvPeak - 1))
            {
                s4_ref_mid1 = s4_raw_InvPeak - 1;
            }

            s4_ref_mid2 = s4_ref_mid - (s4_ref_mid1 - s4_ref_mid);
        }
        else
        {
            s4_ref_mid1 = s4_ref_mid;
            s4_ref_mid2 = s4_ref_mid;
        }

        /* Adjust maximum PWM timing */
        if ((s4_ref_max - s4_ref_mid1) < p_extended_cfg->difference_minimum)
        {
            s4_ref_max1 = s4_ref_mid1 + p_extended_cfg->difference_minimum;
            if (s4_ref_max1 > (s4_raw_InvPeak - 1))
            {
                s4_ref_max1 = s4_raw_InvPeak - 1;
            }

            s4_ref_max2 = s4_ref_max - (s4_ref_max1 - s4_ref_max);
        }
        else
        {
            s4_ref_max1 = s4_ref_max;
            s4_ref_max2 = s4_ref_max;
        }

        /* Shift limit */
        if (s4_ref_mid2 < 1)
        {
            s4_ref_mid2 = 1;
        }

        if (s4_ref_max2 < 1)
        {
            s4_ref_max2 = 1;
        }

        /* Set A/D trigger timing */
        s4_AD1_trigger = s4_ref_mid1 - (p_extended_cfg->difference_minimum - p_extended_cfg->adjust_adc_delay);
        s4_AD2_trigger = s4_ref_max1 - (p_extended_cfg->difference_minimum - p_extended_cfg->adjust_adc_delay);

        switch (p_ctrl->min_phase)
        {
            case MOTOR_HAL_DRIVER_PHASE_U_PHASE:
            {
                s4_mu_ref1 = s4_ref_min;
                s4_mu_ref2 = s4_ref_min;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_V_PHASE:
            {
                s4_mv_ref1 = s4_ref_min;
                s4_mv_ref2 = s4_ref_min;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_W_PHASE:
            {
                s4_mw_ref1 = s4_ref_min;
                s4_mw_ref2 = s4_ref_min;
                break;
            }

            default:
            {
                break;
            }
        }

        switch (p_ctrl->mid_phase)
        {
            case MOTOR_HAL_DRIVER_PHASE_U_PHASE:
            {
                s4_mu_ref1 = s4_ref_mid1;
                s4_mu_ref2 = s4_ref_mid2;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_V_PHASE:
            {
                s4_mv_ref1 = s4_ref_mid1;
                s4_mv_ref2 = s4_ref_mid2;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_W_PHASE:
            {
                s4_mw_ref1 = s4_ref_mid1;
                s4_mw_ref2 = s4_ref_mid2;
                break;
            }

            default:
            {
                break;
            }
        }

        switch (max_phase)
        {
            case MOTOR_HAL_DRIVER_PHASE_U_PHASE:
            {
                s4_mu_ref1 = s4_ref_max1;
                s4_mu_ref2 = s4_ref_max2;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_V_PHASE:
            {
                s4_mv_ref1 = s4_ref_max1;
                s4_mv_ref2 = s4_ref_max2;
                break;
            }

            case MOTOR_HAL_DRIVER_PHASE_W_PHASE:
            {
                s4_mw_ref1 = s4_ref_max1;
                s4_mw_ref2 = s4_ref_max2;
                break;
            }

            default:
            {
                break;
            }
        }

        temp_duty.duty[0]        = (uint32_t) s4_mu_ref1;
        temp_duty.duty[1]        = (uint32_t) s4_mv_ref1;
        temp_duty.duty[2]        = (uint32_t) s4_mw_ref1;
        temp_duty.duty_buffer[0] = (uint32_t) s4_mu_ref2;
        temp_duty.duty_buffer[1] = (uint32_t) s4_mv_ref2;
        temp_duty.duty_buffer[2] = (uint32_t) s4_mw_ref2;

        p_three_phase->p_api->dutyCycleSet(p_three_phase->p_ctrl, &temp_duty);

        R_GPT_AdcTriggerSet(timer_u->p_ctrl, GPT_ADC_COMPARE_MATCH_ADC_A, (uint32_t) s4_AD1_trigger);
        R_GPT_AdcTriggerSet(timer_u->p_ctrl, GPT_ADC_COMPARE_MATCH_ADC_B, (uint32_t) s4_AD2_trigger);
    }
}                                      /* End of function rm_motor_hal_driver_1shunt_modulation */

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_pin_cfg
 * Description  : Configure a pin
 * Arguments    : pin -
 *                    The pin
 *                cfg -
 *                    Configuration for the pin (PmnPFS register setting)
 * Return Value : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_pin_cfg (bsp_io_port_pin_t pin, uint32_t cfg)
{
    R_BSP_PinAccessEnable();
    R_BSP_PinCfg(pin, cfg);
    R_BSP_PinAccessDisable();
}                                      /* End of function rm_motor_hal_driver_pin_cfg() */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_output_enable
 * Description   : Enable PWM output
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_output_enable (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    if (1U == p_ctrl->port_enable_flag)
    {
        motor_hal_driver_cfg_t          * p_cfg         = (motor_hal_driver_cfg_t *) p_ctrl->p_cfg;
        motor_hal_driver_extended_cfg_t * p_extended    = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
        three_phase_instance_t const    * p_three_phase = p_cfg->p_three_phase_instance;
        timer_instance_t const          * p_u_phase_gpt = p_three_phase->p_cfg->p_timer_instance[0];
        timer_instance_t const          * p_v_phase_gpt = p_three_phase->p_cfg->p_timer_instance[1];
        timer_instance_t const          * p_w_phase_gpt = p_three_phase->p_cfg->p_timer_instance[2];

        p_ctrl->gtioca_low_cfg |= MOTOR_HAL_DRIVER_IO_PORT_PERIPHERAL_MASK;
        p_ctrl->gtiocb_low_cfg |= MOTOR_HAL_DRIVER_IO_PORT_PERIPHERAL_MASK;

        /* Set pin function */
        rm_motor_hal_driver_pin_cfg(p_extended->port_up, p_ctrl->gtioca_low_cfg);
        rm_motor_hal_driver_pin_cfg(p_extended->port_un, p_ctrl->gtiocb_low_cfg);
        rm_motor_hal_driver_pin_cfg(p_extended->port_vp, p_ctrl->gtioca_low_cfg);
        rm_motor_hal_driver_pin_cfg(p_extended->port_vn, p_ctrl->gtiocb_low_cfg);
        rm_motor_hal_driver_pin_cfg(p_extended->port_wp, p_ctrl->gtioca_low_cfg);
        rm_motor_hal_driver_pin_cfg(p_extended->port_wn, p_ctrl->gtiocb_low_cfg);

        /* PWM output enable */
        R_GPT_OutputEnable(p_u_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
        R_GPT_OutputEnable(p_v_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
        R_GPT_OutputEnable(p_w_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);

        p_ctrl->port_enable_flag = 0U;
    }
}                                      /* End of function rm_motor_hal_driver_output_enable */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_output_disable
 * Description   : Disable PWM output
 * Arguments     : p_ctrl - The pointer to the motor driver module instance
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_output_disable (motor_hal_driver_instance_ctrl_t * p_ctrl)
{
    motor_hal_driver_cfg_t          * p_cfg         = (motor_hal_driver_cfg_t *) p_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extended    = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
    three_phase_instance_t const    * p_three_phase = p_cfg->p_three_phase_instance;
    timer_instance_t const          * p_u_phase_gpt = p_three_phase->p_cfg->p_timer_instance[0];
    timer_instance_t const          * p_v_phase_gpt = p_three_phase->p_cfg->p_timer_instance[1];
    timer_instance_t const          * p_w_phase_gpt = p_three_phase->p_cfg->p_timer_instance[2];

    /* PWM output disable */
    R_GPT_OutputDisable(p_u_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputDisable(p_v_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputDisable(p_w_phase_gpt->p_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);

    p_ctrl->gtioca_low_cfg &= MOTOR_HAL_DRIVER_IO_PORT_GPIO_MASK;
    p_ctrl->gtiocb_low_cfg &= MOTOR_HAL_DRIVER_IO_PORT_GPIO_MASK;

    rm_motor_hal_driver_pin_cfg(p_extended->port_up, p_ctrl->gtioca_low_cfg);
    rm_motor_hal_driver_pin_cfg(p_extended->port_un, p_ctrl->gtiocb_low_cfg);
    rm_motor_hal_driver_pin_cfg(p_extended->port_vp, p_ctrl->gtioca_low_cfg);
    rm_motor_hal_driver_pin_cfg(p_extended->port_vn, p_ctrl->gtiocb_low_cfg);
    rm_motor_hal_driver_pin_cfg(p_extended->port_wp, p_ctrl->gtioca_low_cfg);
    rm_motor_hal_driver_pin_cfg(p_extended->port_wn, p_ctrl->gtiocb_low_cfg);

    p_ctrl->port_enable_flag = 1U;
}                                      /* End of function rm_motor_hal_driver_output_disable */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_cyclic
 * Description   : Cyclic process for driver accsess (Call at A/D conversion finish interrupt)
 * Arguments     : p_args - The pointer to arguments of A/D conversion finish intterupt callback
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_cyclic (adc_callback_args_t * p_args)
{
    motor_hal_driver_instance_ctrl_t * p_instance = (motor_hal_driver_instance_ctrl_t *) p_args->p_context;
    motor_hal_driver_callback_args_t   temp_args_t;

    /* Get A/D converted data (Phase current & main line voltage) */
    rm_motor_hal_driver_current_get(p_instance);

    if (MOTOR_HAL_DRIVER_OPEN == p_instance->open)
    {
        /* Perform current control process (if set) */
        if (NULL != p_instance->p_cfg->p_callback)
        {
            temp_args_t.event     = MOTOR_HAL_DRIVER_EVENT_CURRENT;
            temp_args_t.p_context = p_instance->p_cfg->p_context;
            (p_instance->p_cfg->p_callback)(&temp_args_t);
        }

        /* PWM modulation */
        if (MOTOR_FUNDLIB_FLAG_SET == p_instance->offset_calc_flag)
        {
            rm_motor_hal_driver_modulation(p_instance);
        }
    }
}                                      /* End of function rm_motor_hal_driver_cyclic */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_1shunt_cyclic
 * Description   : Cyclic process for driver accsess (Call at GPT underflow interrupt)
 * Arguments     : p_args - The pointer to arguments of timer intterupt callback
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_1shunt_cyclic (timer_callback_args_t * p_args)
{
    motor_hal_driver_instance_ctrl_t * p_instance = (motor_hal_driver_instance_ctrl_t *) p_args->p_context;
    motor_hal_driver_callback_args_t   temp_args_t;

    if (MOTOR_HAL_DRIVER_OPEN == p_instance->open)
    {
        if (TIMER_EVENT_CYCLE_END == p_args->event)
        {
            /* Get A/D converted data (Phase current & main line voltage) */
            rm_motor_hal_driver_1shunt_current_get(p_instance);

            /* Perform current control process (if set) */
            if (NULL != p_instance->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_HAL_DRIVER_EVENT_CURRENT;
                temp_args_t.p_context = p_instance->p_cfg->p_context;
                (p_instance->p_cfg->p_callback)(&temp_args_t);
            }

            /* PWM modulation */
            if (MOTOR_FUNDLIB_FLAG_SET == p_instance->offset_calc_flag)
            {
                rm_motor_hal_driver_1shunt_modulation(p_instance);
            }
            else
            {
                p_instance->ref_u = CCAL_ZERO;
                p_instance->ref_v = CCAL_ZERO;
                p_instance->ref_w = CCAL_ZERO;
                rm_motor_hal_driver_1shunt_modulation(p_instance);
            }
        }
    }
}                                      /* End of function rm_motor_hal_driver_1shunt_cyclic */

#if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 1)

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_shared_cyclic
 * Description   : Cyclic process by using shared instance for driver accsess (Call at A/D conversion finish interrupt)
 * Arguments     : p_args - The pointer to arguments of A/D conversion finish intterupt callback
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_shared_cyclic (adc_callback_args_t * p_args)
{
    motor_hal_driver_shared_instance_ctrl_t * p_instance =
        (motor_hal_driver_shared_instance_ctrl_t *) p_args->p_context;
    adc_callback_args_t                temp_args_t;
    motor_hal_driver_instance_ctrl_t * p_ctrl;

    uint8_t i;

    if (MOTOR_HAL_DRIVER_SHARED_ADC_OPEN == p_instance->open)
    {
        switch (p_args->event)
        {
            default:
            {
                break;
            }

            case ADC_EVENT_SCAN_COMPLETE:
            case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
            {
                for (i = 0; i < MOTOR_HAL_DRIVER_CFG_SUPPORT_MOTOR_NUM; i++)
                {
                    p_ctrl = (motor_hal_driver_instance_ctrl_t *) p_instance->p_context[i];

                    if (NULL == p_ctrl)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        motor_hal_driver_extended_cfg_t * p_extended_cfg =
                            (motor_hal_driver_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

                        /* Check scan complete ad channel */
                        if (p_args->group_mask & (1 << p_extended_cfg->adc_group))
                        {
                            temp_args_t = *p_args;

                            /* Set motor_hal_driver instance */
                            temp_args_t.p_context = p_instance->p_context[i];
                            rm_motor_hal_driver_cyclic(&temp_args_t);
                        }
                    }
                }

                break;
            }
        }
    }
}                                      /* End of function rm_motor_hal_driver_shared_cyclic */

#endif /* MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 1 */

#if (MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 1)

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_adc_b_open
 * Description   : Open operation with adc_b module
 * Arguments     : p_instance_ctrl - The pointer to instance control
 *               : p_cfg  - The pointer to configuration data
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_adc_b_open (motor_hal_driver_instance_ctrl_t   * p_instance_ctrl,
                                            motor_hal_driver_cfg_t const * const p_cfg)
{
 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
    if (p_cfg->p_adc_instance != NULL)
    {
        p_cfg->p_adc_instance->p_api->open(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_cfg);
        p_cfg->p_adc_instance->p_api->scanCfg(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_channel_cfg);

        if (p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT)
        {
            p_cfg->p_adc_instance->p_api->callbackSet(p_cfg->p_adc_instance->p_ctrl,
                                                      rm_motor_hal_driver_cyclic,
                                                      p_instance_ctrl,
                                                      &(p_instance_ctrl->adc_callback_args));
        }

        p_cfg->p_adc_instance->p_api->calibrate(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_cfg->p_extend);

        adc_status_t status = {.state = ADC_STATE_SCAN_IN_PROGRESS};
        while (ADC_STATE_IDLE != status.state)
        {
            p_cfg->p_adc_instance->p_api->scanStatusGet(p_cfg->p_adc_instance->p_ctrl, &status);
        }

        p_cfg->p_adc_instance->p_api->scanStart(p_cfg->p_adc_instance->p_ctrl);
    }

 #else                                 /* SUPPORT_SHARED_ADC == 1 */
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
    p_instance_ctrl->p_shared_instance_ctrl = p_extended_cfg->p_shared_cfg->p_shared_instance_ctrl;

    motor_hal_driver_shared_instance_ctrl_t * p_shared_ctrl = p_instance_ctrl->p_shared_instance_ctrl;

    /* After the second motor */
    if (p_shared_ctrl->registered_motor_count != 0)
    {
        /* Register driver module instance for callback */
        p_instance_ctrl->p_shared_instance_ctrl->p_context[p_shared_ctrl->registered_motor_count] = p_instance_ctrl;

        /* count up the number of registered motor */
        (p_shared_ctrl->registered_motor_count)++;
    }
    /* For the first motor */
    else
    {
        if (p_extended_cfg->p_shared_cfg->p_adc_instance_first != NULL)
        {
            adc_instance_t const * p_adc_instance;
            p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_first;

            FSP_CRITICAL_SECTION_DEFINE;
            FSP_CRITICAL_SECTION_ENTER;

            /* Open and setting ADC instance */
            p_adc_instance->p_api->open(p_adc_instance->p_ctrl, p_adc_instance->p_cfg);
            FSP_CRITICAL_SECTION_EXIT;
            p_adc_instance->p_api->scanCfg(p_adc_instance->p_ctrl, p_adc_instance->p_channel_cfg);

            if (p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT)
            {
                /* Register callback to ADC module */
                p_adc_instance->p_api->callbackSet(p_adc_instance->p_ctrl,
                                                   rm_motor_hal_driver_shared_cyclic,
                                                   p_instance_ctrl->p_shared_instance_ctrl,
                                                   &(p_instance_ctrl->adc_callback_args));
            }

            p_adc_instance->p_api->calibrate(p_adc_instance->p_ctrl, p_adc_instance->p_cfg->p_extend);

            adc_status_t status = {.state = ADC_STATE_SCAN_IN_PROGRESS};
            while (ADC_STATE_IDLE != status.state)
            {
                p_adc_instance->p_api->scanStatusGet(p_adc_instance->p_ctrl, &status);
            }

            /* Start ADC module */
            p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);

            /* Set ADC shared information */
            p_instance_ctrl->p_shared_instance_ctrl->open = MOTOR_HAL_DRIVER_SHARED_ADC_OPEN;
            p_instance_ctrl->p_shared_instance_ctrl->p_context[p_shared_ctrl->registered_motor_count] =
                p_instance_ctrl;
            (p_shared_ctrl->registered_motor_count)++;
        }
    }
 #endif                                /* SUPPORT_SHARED_ADC == 1 */
}                                      /* End of function rm_motor_hal_driver_adc_b_open() */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_adc_b_close
 * Description   : Close operation with adc_b module
 * Arguments     : p_instance_ctrl - The pointer to instance control
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_adc_b_close (motor_hal_driver_instance_ctrl_t * p_instance_ctrl)
{
    motor_hal_driver_cfg_t * p_cfg = (motor_hal_driver_cfg_t *) p_instance_ctrl->p_cfg;

 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    /* Close ADC module */
    if (p_cfg->p_adc_instance != NULL)
    {
        p_cfg->p_adc_instance->p_api->close(p_cfg->p_adc_instance->p_ctrl);
    }

 #else                                 /* SUPPORT_SHARED_ADC == 1 */
    adc_instance_t const * p_adc_instance;
    motor_hal_driver_shared_instance_ctrl_t * p_shared_ctrl  = p_instance_ctrl->p_shared_instance_ctrl;
    motor_hal_driver_extended_cfg_t         * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* When the first motor is closed, ADC module should be closed. */
    if (1 == p_shared_ctrl->registered_motor_count)
    {
        p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_first;
        p_adc_instance->p_api->close(p_adc_instance->p_ctrl);

        /* Clear context array */
        uint8_t i;
        for (i = 0; i < MOTOR_HAL_DRIVER_CFG_SUPPORT_MOTOR_NUM; i++)
        {
            p_instance_ctrl->p_shared_instance_ctrl->p_context[i] = NULL;
        }

        p_instance_ctrl->p_shared_instance_ctrl->open = 0U;
        p_shared_ctrl->registered_motor_count         = 0U;
    }
    else
    {
        /* Only declease motor count */
        p_shared_ctrl->registered_motor_count--;
    }
    FSP_CRITICAL_SECTION_EXIT;
 #endif                                /* SUPPORT_SHARED_ADC == 1 */
}

#else                                  /* MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 0 */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_adc_open
 * Description   : Open operation with adc_b module
 * Arguments     : p_instance_ctrl - The pointer to instance control
 *               : p_cfg  - The pointer to configuration data
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_adc_open (motor_hal_driver_instance_ctrl_t   * p_instance_ctrl,
                                          motor_hal_driver_cfg_t const * const p_cfg)
{
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;
 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    /* Start ADC module */
    /* For 1shunt, Vdc is need to read by ADC module #1. */
    if (MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT == p_extended_cfg->shunt)
    {
        /* When different AD units are used to read 1shunt current and vdc,
         * the unit for vdc should be open. */
        if (p_extended_cfg->i_u_ad_unit != p_extended_cfg->v_dc_ad_unit)
        {
            if (p_cfg->p_adc2_instance != NULL)
            {
                p_cfg->p_adc2_instance->p_api->open(p_cfg->p_adc2_instance->p_ctrl, p_cfg->p_adc2_instance->p_cfg);
                p_cfg->p_adc2_instance->p_api->scanCfg(p_cfg->p_adc2_instance->p_ctrl,
                                                       p_cfg->p_adc2_instance->p_channel_cfg);
            }
        }
    }

    if (p_cfg->p_adc_instance != NULL)
    {
        p_cfg->p_adc_instance->p_api->open(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_cfg);
        p_cfg->p_adc_instance->p_api->scanCfg(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_channel_cfg);
        p_cfg->p_adc_instance->p_api->calibrate(p_cfg->p_adc_instance->p_ctrl, p_cfg->p_adc_instance->p_cfg->p_extend);

        if (p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT)
        {
            p_cfg->p_adc_instance->p_api->callbackSet(p_cfg->p_adc_instance->p_ctrl,
                                                      rm_motor_hal_driver_cyclic,
                                                      p_instance_ctrl,
                                                      &(p_instance_ctrl->adc_callback_args));
        }

        p_cfg->p_adc_instance->p_api->scanStart(p_cfg->p_adc_instance->p_ctrl);
    }

 #else                                 /* SUPPORT_SHARED_ADC == 1 */
    p_instance_ctrl->p_shared_instance_ctrl = p_extended_cfg->p_shared_cfg->p_shared_instance_ctrl;

    motor_hal_driver_shared_instance_ctrl_t * p_shared_ctrl = p_instance_ctrl->p_shared_instance_ctrl;
    adc_instance_t const * p_adc_instance;

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* For the first motor */
    if (0 == p_shared_ctrl->registered_motor_count)
    {
        if (p_extended_cfg->p_shared_cfg->p_adc_instance_first != NULL)
        {
            p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_first;

            /* Open and setting ADC instance */
            p_adc_instance->p_api->open(p_adc_instance->p_ctrl, p_adc_instance->p_cfg);
            p_adc_instance->p_api->scanCfg(p_adc_instance->p_ctrl, p_adc_instance->p_channel_cfg);
            p_adc_instance->p_api->calibrate(p_adc_instance->p_ctrl, p_adc_instance->p_cfg->p_extend);

            if ((p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT) &&
                (MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_FIRST == p_extended_cfg->interrupt_adc))
            {
                /* Register callback to ADC module */
                p_adc_instance->p_api->callbackSet(p_adc_instance->p_ctrl,
                                                   rm_motor_hal_driver_cyclic,
                                                   p_instance_ctrl,
                                                   &(p_instance_ctrl->adc_callback_args));
            }

            /* Start ADC module */
            p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);

            /* Set ADC shared information */
            p_instance_ctrl->p_shared_instance_ctrl->open = MOTOR_HAL_DRIVER_SHARED_ADC_OPEN;
            p_instance_ctrl->p_shared_instance_ctrl->p_context[p_shared_ctrl->registered_motor_count] =
                p_instance_ctrl;
            (p_shared_ctrl->registered_motor_count)++;
        }

        if ((MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT == p_extended_cfg->shunt) &&
            (p_extended_cfg->i_u_ad_unit == p_extended_cfg->v_dc_ad_unit))
        {
            /* When 1shunt current and Vdc are read with same ADC channel,
             * second ADC module should not be opened. */
        }
        /* In other case, second ADC module is opened here. */
        else
        {
            if (p_extended_cfg->p_shared_cfg->p_adc_instance_second != NULL)
            {
                p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_second;

                if (p_adc_instance != NULL)
                {
                    p_adc_instance->p_api->open(p_adc_instance->p_ctrl, p_adc_instance->p_cfg);
                    p_adc_instance->p_api->scanCfg(p_adc_instance->p_ctrl, p_adc_instance->p_channel_cfg);
                    p_adc_instance->p_api->calibrate(p_adc_instance->p_ctrl, p_adc_instance->p_cfg->p_extend);

                    if ((p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT) &&
                        (MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_SECOND == p_extended_cfg->interrupt_adc))
                    {
                        /* Register callback to ADC module */
                        p_adc_instance->p_api->callbackSet(p_adc_instance->p_ctrl,
                                                           rm_motor_hal_driver_cyclic,
                                                           p_instance_ctrl,
                                                           &(p_instance_ctrl->adc_callback_args));
                    }

                    /* Not 1 shunt, start 2nd adc here */
                    if (p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT)
                    {
                        p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);
                    }
                }
            }
        }
    }
    /* After the second motor */
    else
    {
        if (p_extended_cfg->shunt != MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT)
        {
            if (MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_FIRST == p_extended_cfg->interrupt_adc)
            {
                p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_first;

                p_adc_instance->p_api->scanStop(p_adc_instance->p_ctrl);

                /* Register callback to ADC module */
                p_adc_instance->p_api->callbackSet(p_adc_instance->p_ctrl,
                                                   rm_motor_hal_driver_cyclic,
                                                   p_instance_ctrl,
                                                   &(p_instance_ctrl->adc_callback_args));
                p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);
            }
            else if (MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_SECOND == p_extended_cfg->interrupt_adc)
            {
                p_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_second;

                p_adc_instance->p_api->scanStop(p_adc_instance->p_ctrl);

                /* Register callback to ADC module */
                p_adc_instance->p_api->callbackSet(p_adc_instance->p_ctrl,
                                                   rm_motor_hal_driver_cyclic,
                                                   p_instance_ctrl,
                                                   &(p_instance_ctrl->adc_callback_args));
                p_adc_instance->p_api->scanStart(p_adc_instance->p_ctrl);
            }
            else
            {
                /* Do nothing */
            }
        }

        /* Register driver module instance for callback */
        p_instance_ctrl->p_shared_instance_ctrl->p_context[p_shared_ctrl->registered_motor_count] = p_instance_ctrl;

        /* count up the number of registered motor */
        (p_shared_ctrl->registered_motor_count)++;
    }
    FSP_CRITICAL_SECTION_EXIT;
 #endif                                /* SUPPORT_SHARED_ADC == 1 */
}                                      /* End of function rm_motor_hal_driver_adc_open() */

/***********************************************************************************************************************
 * Function Name : rm_motor_hal_driver_adc_close
 * Description   : Close operation with adc_b module
 * Arguments     : p_instance_ctrl - The pointer to instance control
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_hal_driver_adc_close (motor_hal_driver_instance_ctrl_t * p_instance_ctrl)
{
    motor_hal_driver_cfg_t          * p_cfg          = (motor_hal_driver_cfg_t *) p_instance_ctrl->p_cfg;
    motor_hal_driver_extended_cfg_t * p_extended_cfg = (motor_hal_driver_extended_cfg_t *) p_cfg->p_extend;

 #if (MOTOR_HAL_DRIVER_CFG_SUPPORT_SHARED_ADC == 0) /* Original ADC module support */
    /* Close ADC module */
    if (p_cfg->p_adc_instance != NULL)
    {
        p_cfg->p_adc_instance->p_api->close(p_cfg->p_adc_instance->p_ctrl);
    }

    if (MOTOR_HAL_DRIVER_SHUNT_TYPE_1_SHUNT == p_extended_cfg->shunt)
    {
        if (p_cfg->p_adc2_instance != NULL)
        {
            p_cfg->p_adc2_instance->p_api->close(p_cfg->p_adc2_instance->p_ctrl);
        }
    }

 #else                                 /* SUPPORT_SHARED_ADC == 1 */
    p_instance_ctrl->p_shared_instance_ctrl = p_extended_cfg->p_shared_cfg->p_shared_instance_ctrl;
    motor_hal_driver_shared_instance_ctrl_t * p_shared_ctrl = p_instance_ctrl->p_shared_instance_ctrl;

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* When the first motor is closed, all ADC modules should be closed. */
    if (1U == p_shared_ctrl->registered_motor_count)
    {
        adc_instance_t const * p_temp_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_first;

        if (p_temp_adc_instance != NULL)
        {
            /* Close ADC module */
            p_temp_adc_instance->p_api->close(p_temp_adc_instance->p_ctrl);
        }

        p_temp_adc_instance = p_extended_cfg->p_shared_cfg->p_adc_instance_second;

        if (p_temp_adc_instance != NULL)
        {
            /* Close second ADC module */
            p_temp_adc_instance->p_api->close(p_temp_adc_instance->p_ctrl);
        }

        /* Clear context array */
        uint8_t i;
        for (i = 0; i < MOTOR_HAL_DRIVER_CFG_SUPPORT_MOTOR_NUM; i++)
        {
            p_instance_ctrl->p_shared_instance_ctrl->p_context[i] = NULL;
        }

        p_instance_ctrl->p_shared_instance_ctrl->open = 0U;
        p_shared_ctrl->registered_motor_count         = 0U;
    }
    else
    {
        /* Only declease motor count */
        p_shared_ctrl->registered_motor_count--;
    }
    FSP_CRITICAL_SECTION_EXIT;
 #endif                                /* SUPPORT_SHARED_ADC == 1 */
}

#endif                                 /* MOTOR_HAL_DRIVER_CFG_ADC_B_SUPPORTED == 0 */
