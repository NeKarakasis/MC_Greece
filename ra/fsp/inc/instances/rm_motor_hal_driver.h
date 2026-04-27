/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup MOTOR_HAL_DRIVER
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_HAL_DRIVER_H
#define RM_MOTOR_HAL_DRIVER_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#ifndef CCAL_FLOAT
 #define CCAL_FLOAT
#endif
#include "rm_motor_shared_ccal_types.h"
#include "rm_motor_shared_ccal_macros.h"

#include "bsp_api.h"

#include "rm_motor_hal_driver_cfg.h"
#include "rm_motor_hal_driver_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Modulation type selection */
typedef enum  e_motor_hal_driver_modulation_method
{
    MOTOR_HAL_DRIVER_MODULATION_METHOD_SPWM = 0, ///< Sinusoidal pulse-width-modulation
    MOTOR_HAL_DRIVER_MODULATION_METHOD_SVPWM,    ///< Space vector pulse-width-modulation
    MOTOR_HAL_DRIVER_MODULATION_METHOD_DPWM,     ///< Discontinuous pulse-width-modulation
} motor_hal_driver_modulation_method_t;

/** Modulation instance block */
typedef struct st_motor_hal_driver_modulation
{
    ccal_num_t v_dc;                   ///< Main line voltage (Vdc) [V]
    ccal_num_t voltage_error_ratio;    ///< The voltage error ratio (VoltageError/Vdc)
    ccal_num_t max_duty;               ///< Maximum duty cycle
    ccal_num_t min_duty;               ///< Minimum duty cycle
    ccal_num_t neutral_duty;           ///< Duty cycle that represents 0[V]
    uint8_t    saturation_flag;        ///< Saturation flag
} motor_hal_driver_modulation_t;

/** Support two ADC instance valid for adc */
typedef enum e_motor_hal_driver_select_adc_instance
{
    MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_FIRST = 0, ///< Use first ADC instance
    MOTOR_HAL_DRIVER_SELECT_ADC_INSTANCE_SECOND,    ///< Use second ADC instances
} motor_hal_driver_select_adc_instance_t;

/** For 1shunt phase detection */
typedef enum  e_motor_hal_driver_phase
{
    MOTOR_HAL_DRIVER_PHASE_U_PHASE = 0, ///< Use U-phase for 1shunt detection
    MOTOR_HAL_DRIVER_PHASE_V_PHASE,     ///< Use V-phase for 1shunt detection
    MOTOR_HAL_DRIVER_PHASE_W_PHASE,     ///< Use W-phase for 1shunt detection
} motor_hal_driver_phase_t;

/** For multiple motor */
typedef struct st_motor_hal_driver_shared_instance_ctrl
{
    uint32_t     open;                                              ///< Motor_hal_driver open flag
    uint8_t      registered_motor_count;                            ///< Registered motor counts
    void const * p_context[MOTOR_HAL_DRIVER_CFG_SUPPORT_MOTOR_NUM]; ///< Each instance pointer for multiple motors
} motor_hal_driver_shared_instance_ctrl_t;

/** For multiple motor */
typedef struct st_motor_hal_driver_extended_shared_cfg
{
    adc_instance_t const * p_adc_instance_first;                            ///< first ADC instance
    adc_instance_t const * p_adc_instance_second;                           ///< second ADC instance

    motor_hal_driver_shared_instance_ctrl_t * const p_shared_instance_ctrl; ///< Control instance for multiple motor
} motor_hal_driver_extended_shared_cfg_t;

/** Extended configuration for motor_hal_driver */
typedef struct st_motor_hal_driver_extended_cfg
{
    uint16_t   pwm_timer_freq;                                    ///< PWM timer frequency [MHz]
    ccal_num_t pwm_carrier_freq;                                  ///< PWM carrier frequency [kHz]
    uint32_t   carrier_base;                                      ///< PWM carrier raw count
    uint16_t   deadtime;                                          ///< PWM deadtime raw count

    ccal_num_t current_range;                                     ///< A/D current measure range (max current) [A]
    ccal_num_t voltage_range;                                     ///< A/D voltage measure range (max phase voltage) [V]
    ccal_num_t v_dc_range;                                        ///< A/D main line voltage measure range (max voltage) [V]
    ccal_num_t sf_ad_current;                                     ///< conversion A/D to current value
    ccal_num_t sf_ad_vdc;                                         ///< conversion A/D to vdc value
    ccal_num_t sf_ad_voltage;                                     ///< conversion A/D to voltage value
    uint16_t   ad_resolution;                                     ///< A/D resolution
    uint16_t   ad_current_offset;                                 ///< A/D current offset (Center value)
    int16_t    ad_voltage_offset;                                 ///< A/D voltage offset (Error correction value)
    int16_t    ad_vdc_offset;                                     ///< A/D vdc offset (Center value or Error correction value)
    ccal_num_t ad_voltage_conversion;                             ///< A/D conversion level

    uint16_t offset_calc_count;                                   ///< Calculation counts for current offset

    /* for bootstrap */
    uint16_t charge_bootstrap_time;                               ///< Charge time for bootstrap circuit (counts)

    motor_hal_driver_modulation_method_t modulation_method;       ///< Modulation method

    /* PWM output port */
    bsp_io_port_pin_t port_up;                                    ///< PWM output port UP
    bsp_io_port_pin_t port_un;                                    ///< PWM output port UN
    bsp_io_port_pin_t port_vp;                                    ///< PWM output port VP
    bsp_io_port_pin_t port_vn;                                    ///< PWM output port VN
    bsp_io_port_pin_t port_wp;                                    ///< PWM output port WP
    bsp_io_port_pin_t port_wn;                                    ///< PWM output port WN

    /* Shunt Type */
    motor_hal_driver_shunt_type_t shunt;                          ///< Selection of shunt type

    /* For 1shunt */
    int32_t difference_minimum;                                   ///< Minimum difference of PWM duty
    int32_t adjust_adc_delay;                                     ///< Adjustment delay for A/D conversion
    motor_hal_driver_phase_t trigger_phase;                       ///< trigger phase of interrupt to get 1shunt current

    /* For multiple motor */
    adc_group_mask_t adc_group;                                   ///< Used ADC scan group only valid for adc_b

    /* ADC channel assignment */
    adc_channel_t i_u_ad_ch;                                      ///< A/D Channel for U Phase Current
    adc_channel_t i_v_ad_ch;                                      ///< A/D Channel for V Phase Current
    adc_channel_t i_w_ad_ch;                                      ///< A/D Channel for W Phase Current
    adc_channel_t v_dc_ad_ch;                                     ///< A/D Channel for Main Line Voltage
    adc_channel_t v_u_ad_ch;                                      ///< A/D channel for U phase voltage
    adc_channel_t v_v_ad_ch;                                      ///< A/D channel for V phase voltage
    adc_channel_t v_w_ad_ch;                                      ///< A/D channel for W phase voltage
    uint8_t       i_u_ad_unit;                                    ///< Used A/D unit number for U phase current
    uint8_t       i_v_ad_unit;                                    ///< Used A/D unit number for V phase current
    uint8_t       i_w_ad_unit;                                    ///< Used A/D unit number for W phase current
    uint8_t       v_dc_ad_unit;                                   ///< Used A/D unit number for main line voltage
    uint8_t       v_u_ad_unit;                                    ///< Used A/D unit number for U phase voltage
    uint8_t       v_v_ad_unit;                                    ///< Used A/D unit number for V phase voltage
    uint8_t       v_w_ad_unit;                                    ///< Used A/D unit number for W phase voltage

    motor_hal_driver_modulation_t mod_param;                      ///< Modulation structure

    motor_hal_driver_select_adc_instance_t         interrupt_adc; ///< Select which interrupt to use
    motor_hal_driver_extended_shared_cfg_t const * p_shared_cfg;  ///< Shared extended config
} motor_hal_driver_extended_cfg_t;

/** Motor_hal_driver instance control block */
typedef struct st_motor_hal_driver_instance_ctrl
{
    uint32_t open;                                                    ///< Motor_hal_driver open flag

    uint32_t carrier_base;                                            ///< PWM carrier base counts
    uint16_t deadtime_count;                                          ///< Deadtime counts

    ccal_num_t i_u_ad;                                                ///< U phase current [A]
    ccal_num_t i_v_ad;                                                ///< V phase current [A]
    ccal_num_t i_w_ad;                                                ///< W phase current [A]
    ccal_num_t v_dc_ad;                                               ///< Main line voltage [V]

    ccal_num_t v_u_ad;                                                ///< U phase voltage (V)
    ccal_num_t v_v_ad;                                                ///< V phase voltage (V)
    ccal_num_t v_w_ad;                                                ///< W phase voltage (V)

    ccal_num_t ref_u;                                                 ///< Calculated U Phase output voltage [V]
    ccal_num_t ref_v;                                                 ///< Calculated V Phase output voltage [V]
    ccal_num_t ref_w;                                                 ///< Calculated W Phase output voltage [V]

    /* for current offset calculation */
    uint8_t  offset_calc_flag;                                        ///< The flag represents that the offset measurement is finished
    uint16_t offset_calc_times;                                       ///< Calculation current times for current offset
    uint16_t offset_calc_digit;                                       ///< Calculation reference level for current offset
    int32_t  offset_i_u;                                              ///< U phase current offset correction value [LSB]
    int32_t  offset_i_v;                                              ///< V phase current offset correction value [LSB]
    int32_t  offset_i_w;                                              ///< W phase current offset correction value [LSB]
    int32_t  sum_i_u_ad;                                              ///< U phase current summation value to calculate offset [LSB]
    int32_t  sum_i_v_ad;                                              ///< V phase current summation value to calculate offset [LSB]
    int32_t  sum_i_w_ad;                                              ///< W phase current summation value to calculate offset [LSB]
    uint8_t  active;                                                  ///< Active/inactive flags for hal_driver

    /* for 1shunt current calculation */
    motor_hal_driver_phase_t min_phase;                               ///< Minimum phase information to calculate 1shunt current
    motor_hal_driver_phase_t mid_phase;                               ///< Middle phase information to calculate 1shunt current

    /* for bootstrap */
    uint8_t  charge_bootstrap_flag;                                   ///< Charge flag of bootstrap circuit
    uint16_t charge_bootstrap_cnt;                                    ///< The number of samples for charging bootstrap circuit

    /* for modulation */
    uint8_t modulation_method;                                        ///< Modulation method
    uint8_t maxvoltage_flag;                                          ///< Phase of maximum voltage in DPWM method

    /* Port configuration */
    uint32_t gtioca_low_cfg;                                          ///< I/O port "Low" config for gtioca
    uint32_t gtiocb_low_cfg;                                          ///< I/O port "Low" config for gtioca
    uint8_t  port_enable_flag;                                        ///< The flag represents that PWM port should be enabled.

    motor_hal_driver_modulation_t  st_modulation;                     ///< Modulation structure
    motor_hal_driver_cfg_t const * p_cfg;                             ///< Motor Driver configuration Structure

    /* For ADC callback */
    adc_callback_args_t adc_callback_args;                            ///< For call ADC callbackSet function

    /* For GPT(Timer) callback */
    timer_callback_args_t timer_callback_args;                        ///< For call GPT(Timer) callbackSet function

    /* Shared ADC */
    motor_hal_driver_shared_instance_ctrl_t * p_shared_instance_ctrl; ///< Control instance for multiple motor
} motor_hal_driver_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const motor_hal_driver_api_t g_motor_hal_driver_on_motor_hal_driver;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_HAL_DRIVER_Open(motor_hal_driver_ctrl_t * const p_ctrl, motor_hal_driver_cfg_t const * const p_cfg);

fsp_err_t RM_MOTOR_HAL_DRIVER_Close(motor_hal_driver_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_HAL_DRIVER_Reset(motor_hal_driver_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_HAL_DRIVER_PhaseVoltageSet(motor_hal_driver_ctrl_t * const p_ctrl,
                                              ccal_num_t const                u_voltage,
                                              ccal_num_t const                v_voltage,
                                              ccal_num_t const                w_voltage);

fsp_err_t RM_MOTOR_HAL_DRIVER_Activate(motor_hal_driver_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_HAL_DRIVER_Deactivate(motor_hal_driver_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_HAL_DRIVER_CurrentGet(motor_hal_driver_ctrl_t * const            p_ctrl,
                                         motor_hal_driver_current_dataset_t * const p_current_dataset);

fsp_err_t RM_MOTOR_HAL_DRIVER_OutputVoltageGet(motor_hal_driver_ctrl_t * const            p_ctrl,
                                               motor_hal_driver_voltage_dataset_t * const p_voltage_dataset);

fsp_err_t RM_MOTOR_HAL_DRIVER_FlagCurrentOffsetGet(motor_hal_driver_ctrl_t * const p_ctrl,
                                                   uint8_t * const                 p_offset_flag);

fsp_err_t RM_MOTOR_HAL_DRIVER_FlagBootstrapGet(motor_hal_driver_ctrl_t * const p_ctrl,
                                               uint8_t * const                 p_bootstrap_flag);

fsp_err_t RM_MOTOR_HAL_DRIVER_CurrentOffsetRestart(motor_hal_driver_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_HAL_DRIVER_ParameterUpdate(motor_hal_driver_ctrl_t * const      p_ctrl,
                                              motor_hal_driver_cfg_t const * const p_cfg);

fsp_err_t RM_MOTOR_HAL_DRIVER_ChargeBootstrap(motor_hal_driver_ctrl_t * const p_ctrl);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_HAL_DRIVER_H

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_HAL_DRIVER)
 **********************************************************************************************************************/
