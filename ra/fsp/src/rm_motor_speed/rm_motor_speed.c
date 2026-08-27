/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdint.h>
#include "rm_motor_speed.h"
#include "rm_motor_speed_library.h"
#include "bsp_api.h"
#include "bsp_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define     MOTOR_SPEED_OPEN                               (('M' << 24U) | ('T' << 16U) | ('S' << 8U) | ('P' << 0U))

#define     MOTOR_SPEED_FLAG_CLEAR                         (0)                         /* For flag clear */
#define     MOTOR_SPEED_FLAG_SET                           (1)                         /* For flag set */

#define     MOTOR_SPEED_MULTIPLE_2                         (2.0F)
#define     MOTOR_SPEED_TWOPI                              (2.0F * 3.1415926535F)
#define     MOTOR_SPEED_TWOPI_60                           (MOTOR_SPEED_TWOPI / 60.0F) /* To translate rpm => rad/s */
#define     MOTOR_SPEED_DIV_8BIT                           (1.0F / 256.0F)
#define     MOTOR_SPEED_RAD_TRANS                          (3.1415926535F / 180.0F)
#define     MOTOR_SPEED_ROOT3                              (1.7320508F)

/* Speed reference status */
#define     MOTOR_SPEED_SPEED_ZERO_CONST                   (0)
#define     MOTOR_SPEED_POSITION_CONTROL                   (1)
#define     MOTOR_SPEED_SPEED_CHANGE                       (2)
#define     MOTOR_SPEED_OPEN_LOOP_INDUCTION                (3)

/* Id reference status */
#define     MOTOR_SPEED_ID_UP                              (0)
#define     MOTOR_SPEED_ID_CONST                           (1)
#define     MOTOR_SPEED_ID_DOWN                            (2)
#define     MOTOR_SPEED_ID_ZERO_CONST                      (3)
#define     MOTOR_SPEED_ID_FLUXWKN                         (4)
#define     MOTOR_SPEED_ID_OPENLOOP                        (5)

/* Iq reference status */
#define     MOTOR_SPEED_IQ_ZERO_CONST                      (0)
#define     MOTOR_SPEED_IQ_SPEED_PI_OUTPUT                 (1)
#define     MOTOR_SPEED_IQ_AUTO_ADJ                        (2)
#define     MOTOR_SPEED_IQ_DOWN                            (3)

#define     MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_90DEG       (1)
#define     MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_0DEG        (2)
#define     MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN         (3)
#define     MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_OPENLOOP    (4)

/* Torque mode sub states */
#define MOTOR_SPEED_TORQUE_STATE_TORQUE       (0U)
#define MOTOR_SPEED_TORQUE_STATE_SPEED_LIMIT  (1U)


#ifndef MOTOR_SPEED_ERROR_RETURN

 #define    MOTOR_SPEED_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
void rm_motor_speed_cyclic(timer_callback_args_t * p_args);

static void  rm_motor_speed_set_param_ref_speed(motor_speed_instance_ctrl_t * p_ctrl, float f_ref_speed_rpm);
static float rm_motor_speed_speed_rate_limit(motor_speed_instance_ctrl_t * p_ctrl);

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 0)
static float rm_motor_speed_set_iq_ref(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_id_ref(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_speed_ref(motor_speed_instance_ctrl_t * p_ctrl);

static float rm_motor_speed_set_iq_ref_hall(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_id_ref_hall(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_speed_ref_hall(motor_speed_instance_ctrl_t * p_ctrl);

#endif

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
static float rm_motor_speed_set_iq_ref_encoder(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_id_ref_encoder(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_speed_ref_encoder(motor_speed_instance_ctrl_t * p_ctrl);

static float rm_motor_speed_set_id_ref_induction(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_set_speed_ref_induction(motor_speed_instance_ctrl_t * p_ctrl);

#endif
static float rm_motor_speed_speed_pi(motor_speed_instance_ctrl_t * p_ctrl, float f_speed_rad);
static float rm_motor_speed_pi_control(motor_speed_pi_params_t * pi_ctrl);
static void  rm_motor_speed_first_order_lpf_init(motor_speed_lpf_t * st_lpf);
static void  rm_motor_speed_first_order_lpf_gain_calc(motor_speed_lpf_t * st_lpf, float f_omega, float f_ctrl_period);

/* Torque mode functions */
static float rm_motor_speed_torque_to_iq(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_get_startup_current(motor_speed_instance_ctrl_t * p_ctrl);
static float rm_motor_speed_torque_control(motor_speed_instance_ctrl_t * p_ctrl);
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
const motor_speed_api_t g_motor_speed_on_motor_speed =
{
    .open                 = RM_MOTOR_SPEED_Open,
    .close                = RM_MOTOR_SPEED_Close,
    .reset                = RM_MOTOR_SPEED_Reset,
    .run                  = RM_MOTOR_SPEED_Run,
    .speedReferenceSet    = RM_MOTOR_SPEED_SpeedReferenceSet,
    .torqueReferenceSet   = RM_MOTOR_SPEED_TorqueReferenceSet,
    .controlModeSet       = RM_MOTOR_SPEED_ControlModeSet,
    .positionReferenceSet = RM_MOTOR_SPEED_PositionReferenceSet,
    .parameterSet         = RM_MOTOR_SPEED_ParameterSet,
    .speedControl         = RM_MOTOR_SPEED_SpeedControl,
    .parameterGet         = RM_MOTOR_SPEED_ParameterGet,
    .parameterUpdate      = RM_MOTOR_SPEED_ParameterUpdate,
};

motor_speed_control_mode_t e_control_mode;

uint8_t u1_torque_state;

float f_torque_ref_mnm;
/*******************************************************************************************************************//**
 * @addtogroup MOTOR_SPEED
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Opens and configures the Motor Speed Module. Implements @ref motor_speed_api_t::open.
 *
 * @retval FSP_SUCCESS              Motor Speed Module successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 * @retval FSP_ERR_INVALID_ARGUMENT Configuration parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_Open (motor_speed_ctrl_t * const p_ctrl, motor_speed_cfg_t const * const p_cfg)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
#endif
    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_cfg->p_extend;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_extended_cfg);

    FSP_ERROR_RETURN(MOTOR_SPEED_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);

    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_speed_ctrl_period, FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_limit_speed_change, FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_maximum_speed_rpm, FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(((MOTOR_SPEED_CONTROL_TYPE_ENCODER != p_extended_cfg->control_type) ||
                      (NULL != p_cfg->p_position_instance)),
                     FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(((MOTOR_SPEED_CONTROL_TYPE_SENSORLESS != p_extended_cfg->control_type) ||
                      (NULL == p_cfg->p_position_instance)),
                     FSP_ERR_INVALID_ARGUMENT);
#endif

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->u1_active                 = MOTOR_SPEED_FLAG_CLEAR;
    p_instance_ctrl->u1_state_speed_ref        = MOTOR_SPEED_SPEED_ZERO_CONST;
    p_instance_ctrl->st_input.u1_flag_get_iref = MOTOR_SPEED_FLAG_CLEAR;
    p_instance_ctrl->u1_state_id_ref           = MOTOR_SPEED_ID_UP;
    p_instance_ctrl->u1_state_iq_ref           = MOTOR_SPEED_IQ_ZERO_CONST;
    p_instance_ctrl->e_control_mode            = MOTOR_SPEED_CONTROL_MODE_SPEED;
    p_instance_ctrl->u1_torque_state           = MOTOR_SPEED_TORQUE_STATE_TORQUE;


    p_instance_ctrl->f_rpm2rad = (float) (p_extended_cfg->mtr_param.u2_mtr_pp) * MOTOR_SPEED_TWOPI_60;
    p_instance_ctrl->f_torque_ref_mnm          = 0.0F;
    p_instance_ctrl->f_id_ref             = 0.0F;
    p_instance_ctrl->f_ref_speed_rad_ctrl = 0.0F;
    p_instance_ctrl->st_input.f_speed_rad = 0.0F;
    p_instance_ctrl->f_speed_lpf_rad      = 0.0F;
    p_instance_ctrl->u1_flag_down_to_ol   = MOTOR_SPEED_FLAG_CLEAR;

    p_instance_ctrl->pi_param.f_ilimit = p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3;
    rm_motor_speed_pi_gain_calc(&(p_extended_cfg->mtr_param),
                                &(p_extended_cfg->d_param),
                                &(p_instance_ctrl->pi_param),
                                p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_first_order_lpf_init(&(p_instance_ctrl->st_phase_err_lpf));
    rm_motor_speed_first_order_lpf_gain_calc(&(p_instance_ctrl->st_phase_err_lpf),
                                             p_extended_cfg->d_param.f_phase_err_lpf_cut_freq,
                                             p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_first_order_lpf_init(&(p_instance_ctrl->st_speed_lpf));
    rm_motor_speed_first_order_lpf_gain_calc(&(p_instance_ctrl->st_speed_lpf),
                                             p_extended_cfg->f_omega_t,
                                             p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_fluxwkn_init(&(p_instance_ctrl->st_flxwkn), p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3, 1.0F,
                                &(p_extended_cfg->mtr_param));
    p_instance_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;

    rm_motor_speed_opl_damp_init(&(p_instance_ctrl->openloop_sub), p_extended_cfg->f_ol_fb_speed_limit_rate);
    rm_motor_speed_opl_damp_r_set_gain(&(p_instance_ctrl->openloop_sub),
                                       p_extended_cfg->mtr_param.u2_mtr_pp,
                                       p_extended_cfg->mtr_param.f4_mtr_m,
                                       p_extended_cfg->mtr_param.f4_mtr_j,
                                       p_extended_cfg->d_param.f_ol_damping_zeta,
                                       p_extended_cfg->d_param.f_ed_hpf_omega,
                                       p_extended_cfg->ol_param.f4_ol_id_ref,
                                       p_extended_cfg->ol_param.f4_id_down_speed_rpm * MOTOR_SPEED_TWOPI_60,
                                       p_extended_cfg->f_speed_ctrl_period);

    /* Speed Observer */
    if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
    {
        if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
        {
            rm_motor_speed_observer_init(&(p_instance_ctrl->st_observer));
            rm_motor_speed_observer_gain_calc(&(p_instance_ctrl->st_observer),
                                              &(p_extended_cfg->d_param),
                                              p_extended_cfg->f_speed_ctrl_period);
        }
        else
        {
            rm_motor_speed_disturbance_observer_init(&(p_instance_ctrl->st_disturbance_observer),
                                                     p_extended_cfg->f_natural_frequency,
                                                     p_extended_cfg->f_speed_ctrl_period,
                                                     p_extended_cfg->mtr_param.f4_mtr_j);
        }
    }

    /* Set used timer instance */
    if (p_cfg->p_timer_instance != NULL)
    {
        p_cfg->p_timer_instance->p_api->open(p_cfg->p_timer_instance->p_ctrl, p_cfg->p_timer_instance->p_cfg);
        p_cfg->p_timer_instance->p_api->start(p_cfg->p_timer_instance->p_ctrl);
    }

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)

    /* Set used motor position instance */
    if (p_cfg->p_position_instance != NULL)
    {
        p_instance_ctrl->p_position_instance = p_cfg->p_position_instance;
        p_cfg->p_position_instance->p_api->open(p_cfg->p_position_instance->p_ctrl, p_cfg->p_position_instance->p_cfg);
    }
#endif
    p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_INIT;

    /* Mark driver as open */
    p_instance_ctrl->open = MOTOR_SPEED_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Disables specified Motor Speed Module. Implements @ref motor_speed_api_t::close.
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_Close (motor_speed_ctrl_t * const p_ctrl)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_speed_cfg_t          * p_cfg          = (motor_speed_cfg_t *) p_instance_ctrl->p_cfg;
    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    p_instance_ctrl->u1_active = MOTOR_SPEED_FLAG_CLEAR;
    p_instance_ctrl->e_status  = MOTOR_SPEED_CTRL_STATUS_INIT;

    if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
    {
        if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
        {
            rm_motor_speed_observer_reset(&(p_instance_ctrl->st_observer));
        }
        else
        {
            rm_motor_speed_disturbance_observer_reset(&(p_instance_ctrl->st_disturbance_observer));
        }
    }

    /* Close used timer instance */
    if (p_cfg->p_timer_instance != NULL)
    {
        p_cfg->p_timer_instance->p_api->close(p_cfg->p_timer_instance->p_ctrl);
    }

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)

    /* Close used motor position instance */
    if (p_cfg->p_position_instance != NULL)
    {
        p_cfg->p_position_instance->p_api->close(p_cfg->p_position_instance->p_ctrl);
    }
#endif

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Reset the variables of Motor Speed Module. Implements @ref motor_speed_api_t::reset
 *
 * @retval FSP_SUCCESS              Successfully reset.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_Reset (motor_speed_ctrl_t * const p_ctrl)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    p_instance_ctrl->u1_active                 = MOTOR_SPEED_FLAG_CLEAR;
    p_instance_ctrl->u1_state_speed_ref        = MOTOR_SPEED_SPEED_ZERO_CONST;
    p_instance_ctrl->st_input.u1_flag_get_iref = MOTOR_SPEED_FLAG_CLEAR;
    p_instance_ctrl->u1_state_id_ref           = MOTOR_SPEED_ID_UP;
    p_instance_ctrl->u1_state_iq_ref           = MOTOR_SPEED_IQ_ZERO_CONST;
    p_instance_ctrl->e_control_mode            = MOTOR_SPEED_CONTROL_MODE_SPEED;
    p_instance_ctrl->u1_torque_state           = MOTOR_SPEED_TORQUE_STATE_TORQUE;

    p_instance_ctrl->f_torque_ref_mnm     = 0.0F;

    p_instance_ctrl->f_id_ref             = 0.0F;
    p_instance_ctrl->f_ref_speed_rad_ctrl = 0.0F;
    p_instance_ctrl->st_input.f_speed_rad = 0.0F;
    p_instance_ctrl->f_speed_lpf_rad      = 0.0F;

    p_instance_ctrl->st_phase_err_lpf.f_pre_output = 0.0F;
    p_instance_ctrl->st_phase_err_lpf.f_pre_input  = 0.0F;

    p_instance_ctrl->st_speed_lpf.f_pre_output = 0.0F;
    p_instance_ctrl->st_speed_lpf.f_pre_input  = 0.0F;

    /* Reset PI parameters */
    p_instance_ctrl->pi_param.f_err  = 0.0F;
    p_instance_ctrl->pi_param.f_refi = 0.0F;

    p_instance_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;
    rm_motor_speed_fluxwkn_reset(&(p_instance_ctrl->st_flxwkn));

    rm_motor_speed_opl_damp_reset(&(p_instance_ctrl->openloop_sub));

    if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
    {
        if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
        {
            rm_motor_speed_observer_reset(&(p_instance_ctrl->st_observer));
        }
        else
        {
            rm_motor_speed_disturbance_observer_reset(&(p_instance_ctrl->st_disturbance_observer));
        }
    }

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
    if (p_instance_ctrl->p_position_instance != NULL)
    {
        p_instance_ctrl->p_position_instance->p_api->reset(p_instance_ctrl->p_position_instance->p_ctrl);

        p_instance_ctrl->p_position_instance->p_api->controlModeSet(p_instance_ctrl->p_position_instance->p_ctrl,
                                                                    MOTOR_POSITION_CTRL_MODE_CONSTANT);
    }
#endif

    p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_INIT;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Run(Start) the Motor Speed Control. Implements @ref motor_speed_api_t::run
 *
 * @retval FSP_SUCCESS              Successfully start.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_Run (motor_speed_ctrl_t * const p_ctrl)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->u1_active = MOTOR_SPEED_FLAG_SET;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set Speed Reference Data. Implements @ref motor_speed_api_t::speedReferenceSet
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_SpeedReferenceSet (motor_speed_ctrl_t * const p_ctrl, float const speed_reference_rpm)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    rm_motor_speed_set_param_ref_speed(p_instance_ctrl, speed_reference_rpm);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set torque reference data.
 *
 * @param[in]  p_ctrl                Pointer to control structure.
 * @param[in]  torque_reference_mnm  Torque reference [mNm].
 *
 * @retval FSP_SUCCESS       Torque reference successfully set.
 * @retval FSP_ERR_ASSERTION Null pointer.
 * @retval FSP_ERR_NOT_OPEN  Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_TorqueReferenceSet (
    motor_speed_ctrl_t * const p_ctrl,
    float const torque_reference_mnm)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl =
        (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);

    MOTOR_SPEED_ERROR_RETURN(
        MOTOR_SPEED_OPEN == p_instance_ctrl->open,
        FSP_ERR_NOT_OPEN);
#endif

    /* Torque direction changed */
    if (((torque_reference_mnm > 0.0F) &&
         (p_instance_ctrl->f_torque_ref_mnm < 0.0F)) || ((torque_reference_mnm < 0.0F) &&
         (p_instance_ctrl->f_torque_ref_mnm > 0.0F)))
    {
        p_instance_ctrl->u1_torque_state = MOTOR_SPEED_TORQUE_STATE_TORQUE;
    }

    p_instance_ctrl->f_torque_ref_mnm = torque_reference_mnm;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set motor control mode.
 *
 * @param[in]  p_ctrl  Pointer to control structure.
 * @param[in]  mode    Motor control mode.
 *
 * @retval FSP_SUCCESS              Control mode successfully set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid control mode.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_ControlModeSet (motor_speed_ctrl_t * const p_ctrl,
                                          motor_speed_control_mode_t const mode)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl =
        (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);

    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open,
                             FSP_ERR_NOT_OPEN);

    MOTOR_SPEED_ERROR_RETURN(
        (MOTOR_SPEED_CONTROL_MODE_SPEED == mode) ||
        (MOTOR_SPEED_CONTROL_MODE_TORQUE == mode),
        FSP_ERR_INVALID_ARGUMENT);
#endif

    if (p_instance_ctrl->e_control_mode != mode)
    {
        /*
         * Reset torque supervisor whenever operating mode changes.
         */
        p_instance_ctrl->u1_torque_state =
            MOTOR_SPEED_TORQUE_STATE_TORQUE;

        /*
         * Torque -> Speed mode:
         * initialize speed PI close to current operating point.
         */
        if (MOTOR_SPEED_CONTROL_MODE_SPEED == mode)
        {
            p_instance_ctrl->f_ref_speed_rad_ctrl =
                p_instance_ctrl->f_speed_lpf_rad;

            p_instance_ctrl->pi_param.f_refi =
                p_instance_ctrl->f_iq_ref;
        }

        p_instance_ctrl->e_control_mode = mode;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set Position Reference Data. Implements @ref motor_speed_api_t::positionReferenceSet
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Input structure pointer is NULL.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_PositionReferenceSet (motor_speed_ctrl_t * const                p_ctrl,
                                               motor_speed_position_data_t const * const p_position_data)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_SPEED_ERROR_RETURN(NULL != p_position_data, FSP_ERR_INVALID_ARGUMENT);
#endif

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
    motor_position_instance_t const * p_position = p_instance_ctrl->p_position_instance;
#endif

    p_instance_ctrl->st_position_data = *p_position_data;

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
    if (p_position != NULL)
    {
        p_position->p_api->positionReferenceSet(p_position->p_ctrl,
                                                p_instance_ctrl->st_position_data.position_reference_degree);
    }
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Set Input parameters. Implements @ref motor_speed_api_t::parameterSet
 *
 * @retval FSP_SUCCESS              Successfully data is set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Output pointer is NULL.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_ParameterSet (motor_speed_ctrl_t * const p_ctrl, motor_speed_input_t const * const p_st_input)

{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_SPEED_ERROR_RETURN(NULL != p_st_input, FSP_ERR_INVALID_ARGUMENT);
#endif

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
    motor_position_instance_t const * p_position = p_instance_ctrl->p_position_instance;

    if (p_position != NULL)
    {
        p_position->p_api->positionSet(p_position->p_ctrl, p_st_input->f_position_rad);
    }
#endif

    p_instance_ctrl->st_input = *p_st_input;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Calculates the d/q-axis current reference.(Main process of Speed Control) Implements @ref motor_speed_api_t::speedControl
 *
 * @retval FSP_SUCCESS              Successful data calculation.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_SpeedControl (motor_speed_ctrl_t * const p_ctrl)
{
#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 0)
    float f4_temp0 = 0.0F;
#endif
    float f4_idq_ref[2] = {0.0F};
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)
    motor_speed_callback_args_t       temp_args_t;
    motor_position_instance_t const * p_position = p_instance_ctrl->p_position_instance;

    if (MOTOR_SPEED_CONTROL_TYPE_ENCODER == p_extended_cfg->control_type)
    {
        /* Encoder Process */
        /***** Id, Iq, speed reference setting *****/
        if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->st_input.u1_flag_get_iref)
        {
            if (MOTOR_SPEED_CTRL_STATUS_INIT == p_instance_ctrl->e_status)
            {
                // Adjustment mode is MOTOR_ENCODER_CALCULATE_ANGLE_ADJUST_HALL
                if (1U == p_instance_ctrl->st_input.u1_adjust_mode)
                {
                    /* Invoke the encoder function if it is set. */
                    if (NULL != p_instance_ctrl->p_cfg->p_callback)
                    {
                        temp_args_t.event     = MOTOR_SPEED_EVENT_ENCODER_CYCLIC;
                        temp_args_t.p_context = p_instance_ctrl->p_cfg->p_context;
                        (p_instance_ctrl->p_cfg->p_callback)(&temp_args_t);
                        p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_RUN;
                    }
                }
                else if (0U == p_instance_ctrl->st_input.u1_adjust_mode)
                {
                    p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_BOOT;
                }
                else
                {
                }
            }
            else
            {
                if (MOTOR_SPEED_STEP_ENABLE == p_instance_ctrl->st_position_data.e_step_mode)
                {
                    if (p_position != NULL)
                    {
                        p_position->p_api->controlModeSet(p_position->p_ctrl, MOTOR_POSITION_CTRL_MODE_STEP);
                    }
                }
                else
                {
                    if (MOTOR_SPEED_CTRL_STATUS_RUN == p_instance_ctrl->e_status)
                    {
                        if (p_position != NULL)
                        {
                            p_position->p_api->controlModeSet(p_position->p_ctrl, MOTOR_POSITION_CTRL_MODE_TRAPEZOID);
                        }
                    }
                }

                /* Position Control */
                if (p_position != NULL)
                {
                    p_position->p_api->positionControl(p_position->p_ctrl);
                }

                p_instance_ctrl->f_ref_speed_rad_ctrl = rm_motor_speed_set_speed_ref_encoder(p_instance_ctrl);
                f4_idq_ref[1] = rm_motor_speed_set_iq_ref_encoder(p_instance_ctrl);
                f4_idq_ref[0] = rm_motor_speed_set_id_ref_encoder(p_instance_ctrl);

                if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_instance_ctrl->st_input.u1_adjust_status)
                {
                    p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_RUN;
                }
            }

            /* Flux weakening */
            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* This function will over-write the dq-axis current command */
                if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->u1_enable_flux_weakning)
                {
                    rm_motor_speed_fluxwkn_set_vamax(&(p_instance_ctrl->st_flxwkn), p_instance_ctrl->st_input.f_vamax);

                    if ((0.0F < p_instance_ctrl->st_input.f_speed_rad) ||
                        (0.0F > p_instance_ctrl->st_input.f_speed_rad))
                    {
                        rm_motor_speed_fluxwkn_run(&(p_instance_ctrl->st_flxwkn),
                                                   p_instance_ctrl->st_input.f_speed_rad,
                                                   &(p_instance_ctrl->st_input.f_id),
                                                   &(f4_idq_ref[0]));
                    }

                    if (f4_idq_ref[0] > 0.0F)
                    {
                        f4_idq_ref[0] = 0.0F;
                    }
                }
            }
        }
    }
    else if (MOTOR_SPEED_CONTROL_TYPE_INDUCTION == p_extended_cfg->control_type)
    {
        /* Induction sensor process */
        /***** Id, Iq, speed reference setting *****/
        if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->st_input.u1_flag_get_iref)
        {
            if (MOTOR_SPEED_CTRL_STATUS_INIT == p_instance_ctrl->e_status)
            {
                p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_BOOT;
            }
            else
            {
                if (MOTOR_SPEED_CTRL_STATUS_RUN == p_instance_ctrl->e_status)
                {
                    if (MOTOR_SPEED_STEP_ENABLE == p_instance_ctrl->st_position_data.e_step_mode)
                    {
                        if (p_position != NULL)
                        {
                            p_position->p_api->controlModeSet(p_position->p_ctrl, MOTOR_POSITION_CTRL_MODE_STEP);
                        }
                    }
                    else
                    {
                        if (p_position != NULL)
                        {
                            p_position->p_api->controlModeSet(p_position->p_ctrl, MOTOR_POSITION_CTRL_MODE_TRAPEZOID);
                        }
                    }
                }

                /* Position Control */
                if (p_position != NULL)
                {
                    p_position->p_api->positionControl(p_position->p_ctrl);
                }

                p_instance_ctrl->f_ref_speed_rad_ctrl = rm_motor_speed_set_speed_ref_induction(p_instance_ctrl);
                f4_idq_ref[1] = rm_motor_speed_set_iq_ref_encoder(p_instance_ctrl);
                f4_idq_ref[0] = rm_motor_speed_set_id_ref_induction(p_instance_ctrl);

                if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_instance_ctrl->st_input.u1_adjust_status)
                {
                    p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_RUN;
                }
            }

            /* Flux weakening */
            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* This function will over-write the dq-axis current command */
                if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->u1_enable_flux_weakning)
                {
                    rm_motor_speed_fluxwkn_set_vamax(&(p_instance_ctrl->st_flxwkn), p_instance_ctrl->st_input.f_vamax);

                    if ((0.0F < p_instance_ctrl->st_input.f_speed_rad) ||
                        (0.0F > p_instance_ctrl->st_input.f_speed_rad))
                    {
                        rm_motor_speed_fluxwkn_run(&(p_instance_ctrl->st_flxwkn),
                                                   p_instance_ctrl->st_input.f_speed_rad,
                                                   &(p_instance_ctrl->st_input.f_id),
                                                   &(f4_idq_ref[0]));
                    }

                    if (f4_idq_ref[0] > 0.0F)
                    {
                        f4_idq_ref[0] = 0.0F;
                    }
                }
            }
        }
    }
    else
    {
        /* Do nothing */
    }

#else

    /* Sensorless Process */
    if (MOTOR_SPEED_CONTROL_TYPE_SENSORLESS == p_extended_cfg->control_type)
    {
        /***** Sensor-less to open-loop *****/
        /* Filter for phase error */
        p_instance_ctrl->f_phase_err_rad_lpf =
            rm_motor_speed_first_order_lpf(&(p_instance_ctrl->st_phase_err_lpf),
                                           p_instance_ctrl->st_input.f_phase_err_rad);

        /* LPF for speed */
        p_instance_ctrl->f_speed_lpf_rad =
            rm_motor_speed_first_order_lpf(&(p_instance_ctrl->st_speed_lpf), p_instance_ctrl->st_input.f_speed_rad);

        if (MOTOR_SPEED_IQ_SPEED_PI_OUTPUT == p_instance_ctrl->u1_state_iq_ref)
        {
            /* f4_temp0 : The absolute value of speed command [rad/s] */
            f4_temp0 = fabsf(p_instance_ctrl->f_speed_lpf_rad);
            if (f4_temp0 < (p_extended_cfg->ol_param.f4_id_up_speed_rpm) * MOTOR_SPEED_TWOPI_60)
            {
                p_instance_ctrl->u1_flag_down_to_ol   = MOTOR_SPEED_FLAG_SET;
                p_instance_ctrl->f_ref_speed_rad_ctrl = p_instance_ctrl->f_speed_lpf_rad;
            }
        }

        /***** Id, Iq, speed reference setting *****/
        if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->st_input.u1_flag_get_iref)
        {
            p_instance_ctrl->f_ref_speed_rad_ctrl = rm_motor_speed_set_speed_ref(p_instance_ctrl);
            f4_idq_ref[1] = rm_motor_speed_set_iq_ref(p_instance_ctrl);
            f4_idq_ref[0] = rm_motor_speed_set_id_ref(p_instance_ctrl);
        }

        p_instance_ctrl->u1_flag_down_to_ol = MOTOR_SPEED_FLAG_CLEAR;

        if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
        {
            /* This function will over-write the dq-axis current command */
            if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->u1_enable_flux_weakning)
            {
                rm_motor_speed_fluxwkn_set_vamax(&(p_instance_ctrl->st_flxwkn), p_instance_ctrl->st_input.f_vamax);
                rm_motor_speed_fluxwkn_run(&(p_instance_ctrl->st_flxwkn), p_instance_ctrl->f_speed_lpf_rad,
                                           &(p_instance_ctrl->st_input.f_id), &(f4_idq_ref[0]));
            }
        }
    }
    /* Hall process */
    else if (MOTOR_SPEED_CONTROL_TYPE_HALL == p_extended_cfg->control_type)
    {
        if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->st_input.u1_flag_get_iref)
        {
            if (MOTOR_SPEED_CTRL_STATUS_INIT == p_instance_ctrl->e_status)
            {
                p_instance_ctrl->e_status = MOTOR_SPEED_CTRL_STATUS_RUN;
            }
            else
            {
                p_instance_ctrl->f_ref_speed_rad_ctrl = rm_motor_speed_set_speed_ref_hall(p_instance_ctrl);
                f4_idq_ref[1] = rm_motor_speed_set_iq_ref_hall(p_instance_ctrl);
                f4_idq_ref[0] = rm_motor_speed_set_id_ref_hall(p_instance_ctrl);
            }

            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* This function will over-write the dq-axis current command */
                if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->u1_enable_flux_weakning)
                {
                    rm_motor_speed_fluxwkn_set_vamax(&(p_instance_ctrl->st_flxwkn), p_instance_ctrl->st_input.f_vamax);

                    if ((0.0F < p_instance_ctrl->st_input.f_speed_rad) ||
                        (0.0F > p_instance_ctrl->st_input.f_speed_rad))
                    {
                        rm_motor_speed_fluxwkn_run(&(p_instance_ctrl->st_flxwkn),
                                                   p_instance_ctrl->st_input.f_speed_rad,
                                                   &(p_instance_ctrl->st_input.f_id),
                                                   &(f4_idq_ref[0]));
                    }

                    if (f4_idq_ref[0] > 0.0F)
                    {
                        f4_idq_ref[0] = 0.0F;
                    }
                }
            }
        }
    }
    else
    {
        /* Do nothing */
    }
#endif

    p_instance_ctrl->f_id_ref = f4_idq_ref[0];
    p_instance_ctrl->f_iq_ref = f4_idq_ref[1];

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Get Speed Control Parameters. Implements @ref motor_speed_api_t::parameterGet
 *
 * @retval FSP_SUCCESS              Successfully the flag is gotten.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Output pointer is NULL.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_ParameterGet (motor_speed_ctrl_t * const p_ctrl, motor_speed_output_t * const p_st_output)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    MOTOR_SPEED_ERROR_RETURN(NULL != p_st_output, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_st_output->f_id_ref             = p_instance_ctrl->f_id_ref;
    p_st_output->f_iq_ref             = p_instance_ctrl->f_iq_ref;
    p_st_output->f_ref_speed_rad_ctrl = p_instance_ctrl->f_ref_speed_rad_ctrl;
    p_st_output->f_damp_comp_speed    = p_instance_ctrl->f_damp_comp_speed;

    p_st_output->u1_flag_pi = MOTOR_SPEED_FLAG_CLEAR;
    if (MOTOR_SPEED_IQ_SPEED_PI_OUTPUT == p_instance_ctrl->u1_state_iq_ref)
    {
        p_st_output->u1_flag_pi = MOTOR_SPEED_FLAG_SET;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Update the parameters of Speed Control Calculation. Implements @ref motor_speed_api_t::parameterUpdate
 *
 * @retval FSP_SUCCESS              Successfully data was updated.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Configuration parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SPEED_ParameterUpdate (motor_speed_ctrl_t * const p_ctrl, motor_speed_cfg_t const * const p_cfg)
{
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_ctrl;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_cfg);
    MOTOR_SPEED_ERROR_RETURN(MOTOR_SPEED_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_cfg->p_extend;

#if MOTOR_SPEED_CFG_PARAM_CHECKING_ENABLE
    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_speed_ctrl_period, FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_limit_speed_change, FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(0.0F <= p_extended_cfg->f_maximum_speed_rpm, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->f_rpm2rad = (float) (p_extended_cfg->mtr_param.u2_mtr_pp) * MOTOR_SPEED_TWOPI_60;

    p_instance_ctrl->pi_param.f_ilimit = p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3;
    rm_motor_speed_pi_gain_calc(&(p_extended_cfg->mtr_param),
                                &(p_extended_cfg->d_param),
                                &(p_instance_ctrl->pi_param),
                                p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_first_order_lpf_gain_calc(&(p_instance_ctrl->st_phase_err_lpf),
                                             p_extended_cfg->d_param.f_phase_err_lpf_cut_freq,
                                             p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_first_order_lpf_gain_calc(&(p_instance_ctrl->st_speed_lpf),
                                             p_extended_cfg->f_omega_t,
                                             p_extended_cfg->f_speed_ctrl_period);

    rm_motor_speed_fluxwkn_init(&(p_instance_ctrl->st_flxwkn), p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3, 1.0F,
                                &(p_extended_cfg->mtr_param));
    p_instance_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;

    rm_motor_speed_opl_damp_init(&(p_instance_ctrl->openloop_sub), p_extended_cfg->f_ol_fb_speed_limit_rate);
    rm_motor_speed_opl_damp_r_set_gain(&(p_instance_ctrl->openloop_sub),
                                       p_extended_cfg->mtr_param.u2_mtr_pp,
                                       p_extended_cfg->mtr_param.f4_mtr_m,
                                       p_extended_cfg->mtr_param.f4_mtr_j,
                                       p_extended_cfg->d_param.f_ol_damping_zeta,
                                       p_extended_cfg->d_param.f_ed_hpf_omega,
                                       p_extended_cfg->ol_param.f4_ol_id_ref,
                                       p_extended_cfg->ol_param.f4_id_down_speed_rpm * MOTOR_SPEED_TWOPI_60,
                                       p_extended_cfg->f_speed_ctrl_period);

    if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
    {
        if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
        {
            rm_motor_speed_observer_gain_calc(&(p_instance_ctrl->st_observer),
                                              &(p_extended_cfg->d_param),
                                              p_extended_cfg->f_speed_ctrl_period);
        }
        else
        {
            rm_motor_speed_disturbance_observer_init(&(p_instance_ctrl->st_disturbance_observer),
                                                     p_extended_cfg->f_natural_frequency,
                                                     p_extended_cfg->f_speed_ctrl_period,
                                                     p_extended_cfg->mtr_param.f4_mtr_j);
        }
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_SPEED)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_cyclic
 * Description   : Cyclic process of Speed Control (Call at timer interrupt)
 * Arguments     : p_args - The pointer to used arguments
 * Return Value  : None
 **********************************************************************************************************************/
void rm_motor_speed_cyclic (timer_callback_args_t * p_args)
{
    motor_speed_instance_t      * p_instance      = (motor_speed_instance_t *) p_args->p_context;
    motor_speed_instance_ctrl_t * p_instance_ctrl = (motor_speed_instance_ctrl_t *) p_instance->p_ctrl;
    motor_speed_callback_args_t   temp_args_t;

    /* Invoke the callback function if it is set. */
    if (NULL != p_instance->p_cfg->p_callback)
    {
        temp_args_t.event     = MOTOR_SPEED_EVENT_FORWARD;
        temp_args_t.p_context = p_instance->p_cfg->p_context;
        (p_instance->p_cfg->p_callback)(&temp_args_t);
    }

    /* if automatic input data is set, perform automatic set input */
    if (NULL != p_instance->p_cfg->st_input)
    {
        p_instance->p_api->parameterSet(p_instance->p_ctrl, p_instance->p_cfg->st_input);
    }

    /* Perform Speed Control Process */
    if (MOTOR_SPEED_FLAG_SET == p_instance_ctrl->u1_active)
    {
        p_instance->p_api->speedControl(p_instance->p_ctrl);
    }

    /* if automatic output data is set, perform automatic get output */
    if (NULL != p_instance->p_cfg->st_output)
    {
        p_instance->p_api->parameterGet(p_instance->p_ctrl, p_instance->p_cfg->st_output);
    }

    /* Invoke the callback function if it is set. */
    if (NULL != p_instance->p_cfg->p_callback)
    {
        temp_args_t.event     = MOTOR_SPEED_EVENT_BACKWARD;
        temp_args_t.p_context = p_instance->p_cfg->p_context;
        (p_instance->p_cfg->p_callback)(&temp_args_t);
    }
}

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_param_ref_speed
 * Description   : Sets parameter for reference speed
 * Arguments     : p_ctrl              - The pointer to speed control instance
 *                 f_ref_speed_rpm     - reference speed [rpm]
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_speed_set_param_ref_speed (motor_speed_instance_ctrl_t * p_ctrl, float f_ref_speed_rpm)
{
    /* [rpm]->[rad/s] */
    p_ctrl->f_ref_speed_rad = f_ref_speed_rpm * p_ctrl->f_rpm2rad;
}                                      /* End of function rm_motor_speed_set_param_ref_speed */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_speed_rate_limit
 * Description   : Limits the rate of change of speed reference
 * Arguments     : p_ctrl              - The pointer to speed control instance
 * Return Value  : Limited speed reference
 **********************************************************************************************************************/
static float rm_motor_speed_speed_rate_limit (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_temp0;
    float f4_temp1;
    float f4_speed_ref_calc_rad;

#if 0
    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    f4_temp0              = p_ctrl->f_ref_speed_rad - p_ctrl->f_ref_speed_rad_ctrl;
    f4_temp1              = fminf((p_extended_cfg->f_limit_speed_change) * MOTOR_SPEED_TWOPI_60, fabsf(f4_temp0));
    f4_speed_ref_calc_rad = p_ctrl->f_ref_speed_rad_ctrl + copysignf(f4_temp1, f4_temp0);
#endif
    float f_target_speed_rad;
    motor_speed_extended_cfg_t * p_extended_cfg =
            (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    f_target_speed_rad = p_ctrl->f_ref_speed_rad;

    if (MOTOR_SPEED_CONTROL_MODE_TORQUE ==
        p_ctrl->e_control_mode)
    {
        if (p_ctrl->f_torque_ref_mnm < 0.0F)
        {
            f_target_speed_rad =
                -fabsf(p_ctrl->f_ref_speed_rad);
        }
        else
        {
            f_target_speed_rad =
                fabsf(p_ctrl->f_ref_speed_rad);
        }
    }

    f4_temp0 =
        f_target_speed_rad -
        p_ctrl->f_ref_speed_rad_ctrl;

    f4_temp1 =
        fminf(
            p_extended_cfg->f_limit_speed_change *
            MOTOR_SPEED_TWOPI_60,
            fabsf(f4_temp0));

    f4_speed_ref_calc_rad =
        p_ctrl->f_ref_speed_rad_ctrl +
        copysignf(f4_temp1, f4_temp0);
    return f4_speed_ref_calc_rad;
}                                      /* End of function rm_motor_speed_speed_rate_limit */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_speed_pi
 * Description   : Speed PI control
 * Arguments     : p_ctrl              - The pointer to speed control instance
 *                 f4_speed_rad        - The electrical speed [rad/s]
 * Return Value  : The Iq reference
 **********************************************************************************************************************/
static float rm_motor_speed_speed_pi (motor_speed_instance_ctrl_t * p_ctrl, float f_speed_rad)
{
    float f4_iq_ref_calc;

    p_ctrl->pi_param.f_err = p_ctrl->f_ref_speed_rad_ctrl - f_speed_rad;
    f4_iq_ref_calc         = rm_motor_speed_pi_control(&(p_ctrl->pi_param));

    return f4_iq_ref_calc;
}                                      /* End of function rm_motor_speed_speed_pi */

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 0)

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_speed_ref
 * Description   : Updates the speed reference
 * Arguments     : p_ctrl - The pointer to the FOC data instance
 * Return Value  : Speed reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_speed_ref (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_speed_rad_ref_buff = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    switch (p_ctrl->u1_state_speed_ref)
    {
        case MOTOR_SPEED_SPEED_ZERO_CONST:
        {
            f4_speed_rad_ref_buff = 0.0F;
            if ((MOTOR_SPEED_ID_CONST == p_ctrl->u1_state_id_ref) ||
                (MOTOR_SPEED_ID_ZERO_CONST == p_ctrl->u1_state_id_ref))
            {
                p_ctrl->u1_state_speed_ref = MOTOR_SPEED_SPEED_CHANGE;
            }

            break;
        }

        case MOTOR_SPEED_SPEED_CHANGE:
        {
            f4_speed_rad_ref_buff = rm_motor_speed_speed_rate_limit(p_ctrl);
            break;
        }

        default:
        {
            /* Do noting */
            break;
        }
    }

    /* Speed reference limit */
    f4_speed_rad_ref_buff = rm_motor_speed_limitfabs(f4_speed_rad_ref_buff,
                                                     (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60);

    /* Return speed reference */
    return f4_speed_rad_ref_buff;
}                                      /* End of function rm_motor_speed_set_speed_ref */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_speed_ref_hall
 * Description   : Updates the speed reference
 * Arguments     : p_ctrl - The pointer to the FOC data instance
 * Return Value  : Speed reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_speed_ref_hall (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_speed_ref = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    switch (p_ctrl->u1_state_speed_ref)
    {
        case MOTOR_SPEED_SPEED_ZERO_CONST:
        {
            /* Automatically change to PI control */
            p_ctrl->u1_state_speed_ref = MOTOR_SPEED_SPEED_CHANGE;

            break;
        }

        case MOTOR_SPEED_SPEED_CHANGE:
        {
            f4_speed_ref = rm_motor_speed_speed_rate_limit(p_ctrl);
            break;
        }

        default:
        {
            /* Do Nothing */
            break;
        }
    }

    /* speed reference limit */
    f4_speed_ref = rm_motor_speed_limitfabs(f4_speed_ref, (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60);

    /* return speed reference */
    return f4_speed_ref;
}

#endif

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_speed_ref_encoder
 * Description   : Updates the speed reference
 * Arguments     : p_ctrl - The pointer to the FOC data instance
 * Return Value  : Speed reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_speed_ref_encoder (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_speed_ref_calc_rad = 0.0F;

    motor_speed_extended_cfg_t      * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_position_instance_t const * p_position     = p_ctrl->p_position_instance;

    switch (p_ctrl->u1_state_speed_ref)
    {
        case MOTOR_SPEED_SPEED_ZERO_CONST:
        {
            /* speed must be zero while encoder angle adjustment */

            if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
            {
                /* check control loop mode */
                if (MOTOR_SPEED_LOOP_MODE_SPEED == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_SPEED_CHANGE;
                }
                else if (MOTOR_SPEED_LOOP_MODE_POSITION == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_POSITION_CONTROL;
                }
                else
                {
                    /* Do Nothing */
                }
            }

            break;
        }

        case MOTOR_SPEED_POSITION_CONTROL:
        {
            /* check control method mode */
            if (MOTOR_SPEED_METHOD_PID == p_extended_cfg->u1_ctrl_method)
            {
                if (p_position != NULL)
                {
                    p_position->p_api->speedReferencePControlGet(p_position->p_ctrl, &f4_speed_ref_calc_rad);
                }
            }
            else if (MOTOR_SPEED_METHOD_IPD == p_extended_cfg->u1_ctrl_method)
            {
                if (p_position != NULL)
                {
                    p_position->p_api->speedReferenceIpdControlGet(p_position->p_ctrl,
                                                                   (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60,
                                                                   &f4_speed_ref_calc_rad);
                }
            }
            else
            {
                /* Do Nothing */
            }

            /* Speed Reference Profiling mode */
            if ((MOTOR_SPEED_STEP_DISABLE == p_ctrl->st_position_data.e_step_mode) &&
                (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status))
            {
                if (p_position != NULL)
                {
                    /* speed feed-forward control */
                    p_position->p_api->speedReferenceFeedforwardGet(p_position->p_ctrl, &f4_speed_ref_calc_rad);
                }
            }

            break;
        }

        case MOTOR_SPEED_SPEED_CHANGE:
        {
            f4_speed_ref_calc_rad = rm_motor_speed_speed_rate_limit(p_ctrl);
            break;
        }

        default:
        {
            /* Do Nothing */
            break;
        }
    }

    /* speed reference limit */
    f4_speed_ref_calc_rad = rm_motor_speed_limitfabs(f4_speed_ref_calc_rad,
                                                     (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60);

    /* return speed reference */
    return f4_speed_ref_calc_rad;
}

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_speed_ref_induction
 * Description   : Updates the speed reference
 * Arguments     : p_ctrl - The pointer to the FOC data instance
 * Return Value  : Speed reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_speed_ref_induction (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_speed_ref_calc_rad = 0.0F;

    motor_speed_extended_cfg_t      * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_position_instance_t const * p_position     = p_ctrl->p_position_instance;

    switch (p_ctrl->u1_state_speed_ref)
    {
        case MOTOR_SPEED_SPEED_ZERO_CONST:
        {
            if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_OPENLOOP == p_ctrl->st_input.u1_adjust_status)
            {
                p_ctrl->u1_state_speed_ref = MOTOR_SPEED_OPEN_LOOP_INDUCTION;
            }
            /* speed must be zero while rotor angle adjustment */
            else if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
            {
                /* check control loop mode */
                if (MOTOR_SPEED_LOOP_MODE_SPEED == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_SPEED_CHANGE;
                }
                else if (MOTOR_SPEED_LOOP_MODE_POSITION == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_POSITION_CONTROL;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /* Do nothing */
            }

            break;
        }

        case MOTOR_SPEED_POSITION_CONTROL:
        {
            /* check control method mode */
            if (MOTOR_SPEED_METHOD_PID == p_extended_cfg->u1_ctrl_method)
            {
                if (p_position != NULL)
                {
                    p_position->p_api->speedReferencePControlGet(p_position->p_ctrl, &f4_speed_ref_calc_rad);
                }
            }
            else if (MOTOR_SPEED_METHOD_IPD == p_extended_cfg->u1_ctrl_method)
            {
                if (p_position != NULL)
                {
                    p_position->p_api->speedReferenceIpdControlGet(p_position->p_ctrl,
                                                                   (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60,
                                                                   &f4_speed_ref_calc_rad);
                }
            }
            else
            {
                /* Do Nothing */
            }

            /* Speed Reference Profiling mode */
            if ((MOTOR_SPEED_STEP_DISABLE == p_ctrl->st_position_data.e_step_mode) &&
                (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status))
            {
                if (p_position != NULL)
                {
                    /* speed feed-forward control */
                    p_position->p_api->speedReferenceFeedforwardGet(p_position->p_ctrl, &f4_speed_ref_calc_rad);
                }
            }

            break;
        }

        case MOTOR_SPEED_SPEED_CHANGE:
        {
            f4_speed_ref_calc_rad = rm_motor_speed_speed_rate_limit(p_ctrl);
            break;
        }

        case MOTOR_SPEED_OPEN_LOOP_INDUCTION:
        {
            f4_speed_ref_calc_rad = p_ctrl->st_input.f_openloop_speed * p_ctrl->f_rpm2rad;

            if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
            {
                /* check control loop mode */
                if (MOTOR_SPEED_LOOP_MODE_SPEED == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_SPEED_CHANGE;
                }
                else if (MOTOR_SPEED_LOOP_MODE_POSITION == p_ctrl->st_position_data.e_loop_mode)
                {
                    p_ctrl->u1_state_speed_ref = MOTOR_SPEED_POSITION_CONTROL;
                }
                else
                {
                    /* Do Nothing */
                }
            }

            break;
        }

        default:
        {
            /* Do Nothing */
            break;
        }
    }

    /* speed reference limit */
    f4_speed_ref_calc_rad = rm_motor_speed_limitfabs(f4_speed_ref_calc_rad,
                                                     (p_extended_cfg->f_maximum_speed_rpm) * MOTOR_SPEED_TWOPI_60);

    /* return speed reference */
    return f4_speed_ref_calc_rad;
}

#endif

/**********************************************************************************************************************
 * Set Current Reference
 **********************************************************************************************************************/

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 0)

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_iq_ref
 * Description   : Updates the q-axis current reference
 * Arguments     : p_ctrl - The pointer to the ctrl instance
 * Return Value  : Iq reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_iq_ref (motor_speed_instance_ctrl_t * p_ctrl)
{
    float   f4_iq_ref_buff = 0.0F;
    float   f4_temp0       = 0.0F;
    uint8_t u1_temp0       = MOTOR_SPEED_FLAG_CLEAR;

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_FLAG_SET == p_ctrl->u1_flag_down_to_ol)
    {
        p_ctrl->u1_state_iq_ref   = MOTOR_SPEED_IQ_DOWN;
        p_ctrl->f_ol_iq_down_step = (p_extended_cfg->ol_param.f4_ol_iq_down_step_ratio) * MOTOR_SPEED_DIV_8BIT *
                                    p_ctrl->f_iq_ref;
    }

    switch (p_ctrl->u1_state_iq_ref)
    {
        case MOTOR_SPEED_IQ_ZERO_CONST:
        {
            /* Iq command will always be zero in this state */
            f4_iq_ref_buff = 0.0F;

            f4_temp0 = fabsf(p_ctrl->f_ref_speed_rad_ctrl);

            if (MOTOR_SPEED_OPENLOOP_DAMPING_ENABLE == p_extended_cfg->u1_openloop_damping)
            {
                /* Open loop damping control*/
                p_ctrl->f_damp_comp_speed =
                    rm_motor_speed_opl_damp_ctrl(&(p_ctrl->openloop_sub),
                                                 p_ctrl->st_input.f_ed,
                                                 p_ctrl->f_ref_speed_rad_ctrl);
            }

            if (f4_temp0 >= (p_extended_cfg->ol_param.f4_id_down_speed_rpm) * MOTOR_SPEED_TWOPI_60)
            {
                /* State transient to next state */
                if (MOTOR_SPEED_LESS_SWITCH_ENABLE == p_extended_cfg->u1_less_switch)
                {
                    p_ctrl->u1_state_iq_ref      = MOTOR_SPEED_IQ_AUTO_ADJ;
                    p_ctrl->f_opl_torque_current =
                        rm_motor_speed_opl2less_torque_current_calc(
                            &p_extended_cfg->mtr_param,
                            p_extended_cfg->ol_param.f4_opl2less_sw_time,
                            rm_motor_speed_get_startup_current(p_ctrl),
                            p_ctrl->f_phase_err_rad_lpf);
                }
                else
                {
                    p_ctrl->u1_state_iq_ref = MOTOR_SPEED_IQ_SPEED_PI_OUTPUT;
                }

                /* Openloop Damping is active */
                if (MOTOR_SPEED_FLAG_SET == p_extended_cfg->u1_openloop_damping)
                {
                    /* Open-loop damping control reset */
                    rm_motor_speed_opl_damp_reset(&(p_ctrl->openloop_sub));
                    p_ctrl->f_damp_comp_speed = 0.0F;
                }
            }

            break;
        }

        case MOTOR_SPEED_IQ_AUTO_ADJ:
        {
            /* Iq auto adjustment */
            f4_iq_ref_buff = rm_motor_speed_opl2less_iq_calc(p_ctrl->st_input.f_ed,
                                                             p_ctrl->st_input.f_eq,
                                                             p_ctrl->f_id_ref,
                                                             p_ctrl->f_opl_torque_current,
                                                             p_ctrl->st_input.f_phase_err_rad);

            /* Limit Iq reference */
            f4_iq_ref_buff = rm_motor_speed_limitfabs(f4_iq_ref_buff, p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3);

            if (0.0F <= p_ctrl->f_speed_lpf_rad)
            {
                u1_temp0 =
                    (p_ctrl->st_input.f_phase_err_rad <=
                     (p_extended_cfg->ol_param.f4_switch_phase_err_rad) * MOTOR_SPEED_RAD_TRANS);
            }
            else if (0.0F > p_ctrl->f_speed_lpf_rad)
            {
                u1_temp0 =
                    (p_ctrl->st_input.f_phase_err_rad >=
                     -(p_extended_cfg->ol_param.f4_switch_phase_err_rad) * MOTOR_SPEED_RAD_TRANS);
            }
            else
            {
                /* Do nothing */
            }

            if (MOTOR_SPEED_FLAG_SET == u1_temp0)
            {
                p_ctrl->u1_state_iq_ref      = MOTOR_SPEED_IQ_SPEED_PI_OUTPUT;
                p_ctrl->f_init_phase_err_rad = 0.0F;

                /* Preset for suppress speed hunting */
                p_ctrl->f_ref_speed_rad_ctrl = p_ctrl->f_speed_lpf_rad;

                p_ctrl->pi_param.f_refi = f4_iq_ref_buff;
            }

            break;
        }

        case MOTOR_SPEED_IQ_SPEED_PI_OUTPUT:
        {
            if (MOTOR_SPEED_CONTROL_MODE_TORQUE ==
                p_ctrl->e_control_mode)
            {
                f4_iq_ref_buff =
                    rm_motor_speed_torque_control(p_ctrl);
            }
            else
            {
                /* Existing speed control */
                f4_iq_ref_buff =
                    rm_motor_speed_speed_pi(
                        p_ctrl,
                        p_ctrl->st_input.f_speed_rad);

                f4_iq_ref_buff =
                    rm_motor_speed_limitfabs(
                        f4_iq_ref_buff,
                        p_extended_cfg->f_iq_limit *
                        MOTOR_SPEED_ROOT3);
            }

            break;
        }

        case MOTOR_SPEED_IQ_DOWN:
        {
            f4_iq_ref_buff = p_ctrl->f_iq_ref - p_ctrl->f_ol_iq_down_step;
            f4_temp0       = p_ctrl->f_ol_iq_down_step * f4_iq_ref_buff; /* Check sign */
            if (f4_temp0 <= 0.0F)
            {
                f4_iq_ref_buff          = 0.0F;
                p_ctrl->u1_state_iq_ref = MOTOR_SPEED_IQ_ZERO_CONST;
            }

            break;
        }

        default:
        {
            /* Do noting */
            break;
        }
    }

    /* Return iq reference */
    return f4_iq_ref_buff;
}                                      /* End of function rm_motor_speed_set_iq_ref */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_id_ref
 * Description   : Updates the d-axis current reference
 * Arguments     : p_ctrl - The pointer of speed control instance
 * Return Value  : Id reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_id_ref (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_temp0       = 0.0F;
    float f4_id_ref_buff = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_FLAG_SET == p_ctrl->u1_flag_down_to_ol)
    {
        p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_UP;
    }

    if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
    {
        /* Flux-weakening process should be ignored unless d-axis current command in certain state */
        p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;
    }

    switch (p_ctrl->u1_state_id_ref)
    {
        case MOTOR_SPEED_ID_UP:
        {
            float f_startup_current;

            f_startup_current = rm_motor_speed_get_startup_current(p_ctrl);

            f4_id_ref_buff = p_ctrl->f_id_ref + p_extended_cfg->ol_param.f4_ol_id_up_step;

            if (f4_id_ref_buff >= f_startup_current)
            {
                f4_id_ref_buff = f_startup_current;

                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_CONST;
            }

            break;
        }

        case MOTOR_SPEED_ID_CONST:
        {
            if (MOTOR_SPEED_CONTROL_MODE_TORQUE ==
                p_ctrl->e_control_mode)
            {
                f4_id_ref_buff =
                    rm_motor_speed_get_startup_current(p_ctrl);
            }
            else
            {
                /* Original SPEED mode */
                f4_id_ref_buff = p_ctrl->f_id_ref;
            }

            f4_temp0       = fabsf(p_ctrl->f_ref_speed_rad_ctrl);
            if (f4_temp0 >= (p_extended_cfg->ol_param.f4_id_down_speed_rpm) * MOTOR_SPEED_TWOPI_60)
            {
                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_DOWN;
            }

            break;
        }

        case MOTOR_SPEED_ID_DOWN:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref - p_extended_cfg->ol_param.f4_ol_id_down_step;
            if (0.0F >= f4_id_ref_buff)
            {
                f4_id_ref_buff          = 0.0F;
                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
            }

            break;
        }

        case MOTOR_SPEED_ID_ZERO_CONST:
        case MOTOR_SPEED_ID_FLUXWKN:
        {
            f4_id_ref_buff = 0.0F;
            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* Enables the Flux-weakening when the open-loop has completely ended */
                p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_SET;
                if (1 == rm_motor_speed_fluxwkn_check_bypass(&(p_ctrl->st_flxwkn)))
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_FLUXWKN;
                }
                else
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
            }

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    /* Return id reference */
    return f4_id_ref_buff;
}                                      /* End of function rm_motor_speed_set_id_ref */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_iq_ref_hall
 * Description   : Updates the q-axis current reference with hall sensors
 * Arguments     : p_ctrl - The pointer to the ctrl instance
 * Return Value  : Iq reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_iq_ref_hall (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_iq_ref_calc    = 0.0F;
    float f4_temp_speed_rad = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
    {
        if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
        {
            f4_temp_speed_rad = rm_motor_speed_observer(&p_ctrl->st_observer,
                                                        &p_extended_cfg->mtr_param,
                                                        p_ctrl->f_iq_ref,
                                                        p_ctrl->st_input.f_speed_rad);
        }
        else
        {
            float f4_torque_num         = 0.0F;
            float f_temp_motor_polepair = (float) p_extended_cfg->mtr_param.u2_mtr_pp;
            f4_torque_num = ((float) p_extended_cfg->mtr_param.u2_mtr_pp * p_extended_cfg->mtr_param.f4_mtr_m) *
                            p_ctrl->f_iq_ref;
            f4_temp_speed_rad =
                rm_motor_speed_disturbance_observer_run(&p_ctrl->st_disturbance_observer,
                                                        f4_torque_num,
                                                        p_ctrl->st_input.f_speed_rad / f_temp_motor_polepair);
            f4_temp_speed_rad *= f_temp_motor_polepair;
        }
    }
    else
    {
        f4_temp_speed_rad = p_ctrl->st_input.f_speed_rad;
    }

    f4_iq_ref_calc = rm_motor_speed_speed_pi(p_ctrl, f4_temp_speed_rad);

    /*** iq reference limit ***/
    f4_iq_ref_calc = rm_motor_speed_limitfabs(f4_iq_ref_calc, p_extended_cfg->f_iq_limit);

    /* return iq reference */
    return f4_iq_ref_calc;
}                                      /* End of function rm_motor_speed_set_iq_ref_hall */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_id_ref_hall
 * Description   : Updates the d-axis current reference with hall sensors
 * Arguments     : p_ctrl - The pointer of speed control instance
 * Return Value  : Id reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_id_ref_hall (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_id_ref_buff = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
    {
        /* Flux-weakening process should be ignored unless d-axis current command in certain state */
        p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;
    }

    switch (p_ctrl->u1_state_id_ref)
    {
        case MOTOR_SPEED_ID_UP:
        {
            p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;

            break;
        }

        case MOTOR_SPEED_ID_CONST:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref;

            p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;

            break;
        }

        case MOTOR_SPEED_ID_ZERO_CONST:
        case MOTOR_SPEED_ID_FLUXWKN:
        {
            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* Enables the Flux-weakening when the open-loop has completely ended */
                p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_SET;
                if (1 == rm_motor_speed_fluxwkn_check_bypass(&(p_ctrl->st_flxwkn)))
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_FLUXWKN;
                }
                else
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
            }

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    /* Return id reference */
    return f4_id_ref_buff;
}                                      /* End of function rm_motor_speed_set_id_ref_hall */

#endif

#if (MOTOR_SPEED_CFG_POSITION_SUPPORTED == 1)

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_iq_ref_encoder
 * Description   : Updates the q-axis current reference with Encoder
 * Arguments     : p_ctrl - The pointer to the ctrl instance
 * Return Value  : Iq reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_iq_ref_encoder (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_iq_ref_calc    = 0.0F;
    float f4_temp_speed_rad = 0.0F;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    motor_position_instance_t const * p_position = p_ctrl->p_position_instance;

    if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
    {
        if (MOTOR_SPEED_OBSERVER_SWITCH_ENABLE == p_extended_cfg->u1_observer_swtich)
        {
            if (MOTOR_SPEED_OBSERVER_SELECT_NORMAL == p_extended_cfg->observer_select)
            {
                f4_temp_speed_rad = rm_motor_speed_observer(&p_ctrl->st_observer,
                                                            &p_extended_cfg->mtr_param,
                                                            p_ctrl->f_iq_ref,
                                                            p_ctrl->st_input.f_speed_rad);
            }
            else
            {
                float f4_torque_num         = 0.0F;
                float f_temp_motor_polepair = (float) p_extended_cfg->mtr_param.u2_mtr_pp;
                f4_torque_num = ((float) p_extended_cfg->mtr_param.u2_mtr_pp * p_extended_cfg->mtr_param.f4_mtr_m) *
                                p_ctrl->f_iq_ref;
                f4_temp_speed_rad =
                    rm_motor_speed_disturbance_observer_run(&p_ctrl->st_disturbance_observer,
                                                            f4_torque_num,
                                                            p_ctrl->st_input.f_speed_rad / f_temp_motor_polepair);
                f4_temp_speed_rad *= f_temp_motor_polepair;
            }
        }
        else
        {
            f4_temp_speed_rad = p_ctrl->st_input.f_speed_rad;
        }

        /*** speed PI control ***/
        if (MOTOR_SPEED_METHOD_PID == p_extended_cfg->u1_ctrl_method)
        {
            f4_iq_ref_calc = rm_motor_speed_speed_pi(p_ctrl, f4_temp_speed_rad);
        }
        else if (MOTOR_SPEED_METHOD_IPD == p_extended_cfg->u1_ctrl_method)
        {
            if (p_position != NULL)
            {
                p_position->p_api->ipdSpeedPControl(p_position->p_ctrl,
                                                    p_ctrl->f_ref_speed_rad_ctrl,
                                                    f4_temp_speed_rad,
                                                    &f4_iq_ref_calc);
            }
        }
        else
        {
            /* Do Nothing */
        }

        /*** iq reference limit ***/
        f4_iq_ref_calc = rm_motor_speed_limitfabs(f4_iq_ref_calc, p_extended_cfg->f_iq_limit);
    }

    /* return iq reference */
    return f4_iq_ref_calc;
}                                      /* End of function rm_motor_speed_set_iq_ref_encoder */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_id_ref_encoder
 * Description   : Updates the d-axis current reference with Encoder
 * Arguments     : p_ctrl - The pointer of speed control instance
 * Return Value  : Id reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_id_ref_encoder (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_id_ref_buff = 0.0F;
    motor_speed_callback_args_t temp_args_t;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
    {
        /* Flux-weakening process should be ignored unless d-axis current command in certain state */
        p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;
    }

    switch (p_ctrl->u1_state_id_ref)
    {
        case MOTOR_SPEED_ID_UP:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref + p_extended_cfg->ol_param.f4_ol_id_up_step;
            if (f4_id_ref_buff >= p_extended_cfg->ol_param.f4_ol_id_ref)
            {
                f4_id_ref_buff          = p_extended_cfg->ol_param.f4_ol_id_ref;
                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_CONST;
            }

            break;
        }

        case MOTOR_SPEED_ID_CONST:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref;

            if (1U == p_ctrl->st_input.u1_adjust_count_full)
            {
                f4_id_ref_buff = 0.0F;

                /* angle adjusted to 0 degree */

                if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_0DEG == p_ctrl->st_input.u1_adjust_status)
                {
                    /* repeat soft start */
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_UP;
                }
                /* angle adjusted to Finish */

                else if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
                {
                    /* id mode transition to zero constant mode */
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
                else
                {
                    /* Do Nothing */
                }
            }

            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_SPEED_EVENT_ENCODER_ADJUST;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        case MOTOR_SPEED_ID_ZERO_CONST:
        case MOTOR_SPEED_ID_FLUXWKN:
        {
            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* Enables the Flux-weakening when the open-loop has completely ended */
                p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_SET;
                if (1 == rm_motor_speed_fluxwkn_check_bypass(&(p_ctrl->st_flxwkn)))
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_FLUXWKN;
                }
                else
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
            }

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    /* Return id reference */
    return f4_id_ref_buff;
}                                      /* End of function rm_motor_speed_set_id_ref_encoder */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_set_id_ref_induction
 * Description   : Updates the d-axis current reference with induction sensor
 * Arguments     : p_ctrl - The pointer of speed control instance
 * Return Value  : Id reference
 **********************************************************************************************************************/
static float rm_motor_speed_set_id_ref_induction (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f4_id_ref_buff = 0.0F;
    motor_speed_callback_args_t temp_args_t;

    motor_speed_extended_cfg_t * p_extended_cfg = (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
    {
        /* Flux-weakening process should be ignored unless d-axis current command in certain state */
        p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_CLEAR;
    }

    switch (p_ctrl->u1_state_id_ref)
    {
        case MOTOR_SPEED_ID_UP:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref + p_extended_cfg->ol_param.f4_ol_id_up_step;
            if (f4_id_ref_buff >= p_extended_cfg->ol_param.f4_ol_id_ref)
            {
                f4_id_ref_buff          = p_extended_cfg->ol_param.f4_ol_id_ref;
                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_CONST;
            }

            break;
        }

        case MOTOR_SPEED_ID_CONST:
        {
            f4_id_ref_buff = p_ctrl->f_id_ref;

            if (1U == p_ctrl->st_input.u1_adjust_count_full)
            {
                f4_id_ref_buff = 0.0F;

                if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_OPENLOOP == p_ctrl->st_input.u1_adjust_status)
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_OPENLOOP;
                }
                /* angle adjusted to 0 degree */
                else if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_0DEG == p_ctrl->st_input.u1_adjust_status)
                {
                    /* repeat soft start */
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_UP;
                    f4_id_ref_buff          = 0.0F;
                }
                /* angle adjusted to Finish */
                else if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
                {
                    /* id mode transition to zero constant mode */
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
                else
                {
                    /* Do Nothing */
                }
            }

            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_SPEED_EVENT_ENCODER_ADJUST;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        case MOTOR_SPEED_ID_ZERO_CONST:
        case MOTOR_SPEED_ID_FLUXWKN:
        {
            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_SPEED_EVENT_ENCODER_CYCLIC;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            if (MOTOR_SPEED_FLUX_WEAKEN_ENABLE == p_extended_cfg->u1_flux_weakening)
            {
                /* Enables the Flux-weakening when the open-loop has completely ended */
                p_ctrl->u1_enable_flux_weakning = MOTOR_SPEED_FLAG_SET;
                if (1 == rm_motor_speed_fluxwkn_check_bypass(&(p_ctrl->st_flxwkn)))
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_FLUXWKN;
                }
                else
                {
                    p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
                }
            }

            break;
        }

        case MOTOR_SPEED_ID_OPENLOOP:
        {
            f4_id_ref_buff = p_ctrl->st_input.f_openloop_id_ref;
            if (MOTOR_SPEED_CALCULATE_ANGLE_ADJUST_FIN == p_ctrl->st_input.u1_adjust_status)
            {
                /* id mode transition to zero constant mode */
                p_ctrl->u1_state_id_ref = MOTOR_SPEED_ID_ZERO_CONST;
            }

            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_SPEED_EVENT_ENCODER_CYCLIC;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    /* Return id reference */
    return f4_id_ref_buff;
}                                      /* End of function rm_motor_speed_set_id_ref_induction */

#endif

/**********************************************************************************************************************
 * Low Pass Filter for Speed
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_first_order_lpf_init
 * Description   : Initialize First Order LPF
 * Arguments     : st_lpf - First order LPF structure (pointer)
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_speed_first_order_lpf_init (motor_speed_lpf_t * st_lpf)
{
    st_lpf->f_pre_output = 0.0F;
    st_lpf->f_pre_input  = 0.0F;
    st_lpf->f_omega_t    = 0.0F;
    st_lpf->f_gain_ka    = 0.0F;
    st_lpf->f_gain_kb    = 0.0F;
}                                      /* End of function rm_motor_speed_first_order_lpf_init */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_first_order_lpf_gain_calc
 * Description   : Calculate the gain of First Order LPF
 * Arguments     : st_lpf          - First order LPF structure (pointer)
 *                 f4_omega        - Natural frequency
 *                 f4_ctrl_period  - Control period
 * Return Value  : None
 **********************************************************************************************************************/
static void rm_motor_speed_first_order_lpf_gain_calc (motor_speed_lpf_t * st_lpf, float f_omega, float f_ctrl_period)
{
    st_lpf->f_omega_t = (MOTOR_SPEED_TWOPI * f_omega) * f_ctrl_period;
    st_lpf->f_gain_ka = (MOTOR_SPEED_MULTIPLE_2 - st_lpf->f_omega_t) /
                        (st_lpf->f_omega_t + MOTOR_SPEED_MULTIPLE_2);
    st_lpf->f_gain_kb = st_lpf->f_omega_t / (st_lpf->f_omega_t + MOTOR_SPEED_MULTIPLE_2);
}                                      /* End of function rm_motor_speed_first_order_lpf_gain_calc */

/***********************************************************************************************************************
 * Function Name : rm_motor_speed_pi_control
 * Description   : PI control
 * Arguments     : pi_ctrl - The pointer to the PI control structure
 * Return Value  : PI control output value
 **********************************************************************************************************************/
static float rm_motor_speed_pi_control (motor_speed_pi_params_t * pi_ctrl)
{
    float f4_err;
    float f4_kp;
    float f4_ki;
    float f4_refp;
    float f4_refi;
    float f4_ref;
    float f4_ilimit;

    f4_err    = pi_ctrl->f_err;
    f4_kp     = pi_ctrl->f_kp;
    f4_ki     = pi_ctrl->f_ki;
    f4_refi   = pi_ctrl->f_refi;
    f4_ilimit = pi_ctrl->f_ilimit;

    f4_refp  = f4_err * f4_kp;         /* Proportional part */
    f4_refi += (f4_err * f4_ki);       /* Integral part */

    /*** Integral part limit ***/
    f4_refi         = rm_motor_speed_limitfabs(f4_refi, f4_ilimit);
    pi_ctrl->f_refi = f4_refi;

    f4_ref = f4_refp + f4_refi;        /* PI output */

    return f4_ref;
}


static float rm_motor_speed_torque_to_iq (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f_torque_nm;
    float f_torque_constant;
    float f_iq_ref;

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    f_torque_nm = p_ctrl->f_torque_ref_mnm * 0.001F;

    f_torque_constant =
        (float) p_extended_cfg->mtr_param.u2_mtr_pp *
        p_extended_cfg->mtr_param.f4_mtr_m;

    if (fabsf(f_torque_constant) > 0.000001F)
    {
        f_iq_ref = f_torque_nm / f_torque_constant;
    }
    else
    {
        f_iq_ref = 0.0F;
    }

    f_iq_ref =
        rm_motor_speed_limitfabs(
            f_iq_ref,
            p_extended_cfg->f_iq_limit * MOTOR_SPEED_ROOT3);

    return f_iq_ref;
}


static float rm_motor_speed_get_startup_current (motor_speed_instance_ctrl_t * p_ctrl)
{
    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (MOTOR_SPEED_CONTROL_MODE_TORQUE == p_ctrl->e_control_mode)
    {
        return fabsf(rm_motor_speed_torque_to_iq(p_ctrl));
    }

    return p_extended_cfg->ol_param.f4_ol_id_ref;
}


static float rm_motor_speed_torque_control (motor_speed_instance_ctrl_t * p_ctrl)
{
    float f_iq_torque;
    float f_iq_speed;
    float f_speed_limit_rad;
    float f_speed_ref_rad;
    float f_speed_error;
    float f_direction;

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    f_iq_torque = rm_motor_speed_torque_to_iq(p_ctrl);

    /* Zero torque */
    if (fabsf(f_iq_torque) < 0.000001F)
    {
        p_ctrl->u1_torque_state =
            MOTOR_SPEED_TORQUE_STATE_TORQUE;

        return 0.0F;
    }

    f_direction =
        (f_iq_torque >= 0.0F) ? 1.0F : -1.0F;

    f_speed_limit_rad =
        fabsf(p_ctrl->f_ref_speed_rad);

    f_speed_ref_rad =
        f_direction * f_speed_limit_rad;

    switch (p_ctrl->u1_torque_state)
    {
        case MOTOR_SPEED_TORQUE_STATE_TORQUE:
        {
            f_iq_speed = f_iq_torque;

            /*
             * Enter speed limiting only when the speed
             * in the commanded direction reaches the limit.
             */
            if ((f_direction * p_ctrl->f_speed_lpf_rad) >=
                f_speed_limit_rad)
            {
                p_ctrl->f_ref_speed_rad_ctrl =
                    f_speed_ref_rad;

                f_speed_error =
                    f_speed_ref_rad -
                    p_ctrl->st_input.f_speed_rad;

                /*
                 * Bumpless transfer.
                 *
                 * Next PI execution does:
                 *
                 * I += Ki * error
                 * out = Kp * error + I
                 *
                 * Therefore initialize I so that:
                 *
                 * out = iq_torque
                 */
                p_ctrl->pi_param.f_refi =
                    f_iq_torque -
                    ((p_ctrl->pi_param.f_kp +
                      p_ctrl->pi_param.f_ki) *
                     f_speed_error);

                p_ctrl->pi_param.f_refi =
                    rm_motor_speed_limitfabs(
                        p_ctrl->pi_param.f_refi,
                        p_ctrl->pi_param.f_ilimit);

                p_ctrl->u1_torque_state =
                    MOTOR_SPEED_TORQUE_STATE_SPEED_LIMIT;

                f_iq_speed =
                    rm_motor_speed_speed_pi(
                        p_ctrl,
                        p_ctrl->st_input.f_speed_rad);
            }
            p_ctrl->f_final_iq_torque = f_iq_speed;
            break;
        }

        case MOTOR_SPEED_TORQUE_STATE_SPEED_LIMIT:
        {
            p_ctrl->f_ref_speed_rad_ctrl =
                f_speed_ref_rad;

            f_iq_speed =
                rm_motor_speed_speed_pi(
                    p_ctrl,
                    p_ctrl->st_input.f_speed_rad);

            f_iq_speed =
                rm_motor_speed_limitfabs(
                    f_iq_speed,
                    p_extended_cfg->f_iq_limit *
                    MOTOR_SPEED_ROOT3);

            /*
             * Speed controller is no longer restricting
             * requested torque.
             *
             * Multiplying by direction makes this work
             * for both positive and negative torque.
             */
            if ((f_direction * f_iq_speed) >=
                (f_direction * f_iq_torque))
            {
                f_iq_speed = f_iq_torque;

                p_ctrl->u1_torque_state =
                    MOTOR_SPEED_TORQUE_STATE_TORQUE;
            }
            p_ctrl->f_final_iq_torque = f_iq_speed;
            break;
        }

        default:
        {
            p_ctrl->u1_torque_state =
                MOTOR_SPEED_TORQUE_STATE_TORQUE;

            f_iq_speed = f_iq_torque;
            p_ctrl->f_final_iq_torque = f_iq_speed;
            break;
        }
    }
    return f_iq_speed;
}
/* End of function rm_motor_speed_pi_control */
