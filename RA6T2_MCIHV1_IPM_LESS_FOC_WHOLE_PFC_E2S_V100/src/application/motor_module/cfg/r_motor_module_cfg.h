/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_motor_module_cfg.h
* Description  : Compile-time configurations
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_MODULE_CFG_H
#define R_MOTOR_MODULE_CFG_H

#include "r_motor_common.h"
#include "r_motor_targetmotor_cfg.h"
#include "r_motor_inverter_cfg.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/**
 * Middle ware Layer Configurations
 */

/*
 * Software system configurations
 */
/*
 * for MCU
 */
#define     MOTOR_MCU_CFG_PWM_TIMER_FREQ        (120.0f)                        /* PWM timer frequency [MHz] */
#define     MOTOR_MCU_CFG_CARRIER_FREQ          (16.0f)                          /* Carrier wave frequency [kHz] */
#define     MOTOR_MCU_CFG_INTR_DECIMATION       (0)                             /* Interrupt skipping number */
#define     MOTOR_MCU_CFG_AD_FREQ               (60.0f)                         /* A/D frequency [MHz] */
#define     MOTOR_MCU_CFG_AD_SAMPLING_CYCLE     (2.0f * (7.25f + 63.0f))        /* twice the (Scan start processing time(tD) + sample-and-hold processing time(tSPLSH)) [cycle] */
#define     MOTOR_MCU_CFG_AD12BIT_DATA          (4095.0f)                       /* A/D 12Bit data */
#define     MOTOR_MCU_CFG_ADC_OFFSET            (1890.0f)                         /* A/D offset */

#if (defined(__TFU) || defined(BSP_FEATURE_TFU_SUPPORTED))
#define     MOTOR_MCU_CFG_TFU_OPTIMIZE          (MTR_ENABLE)                    /* TFU Optimization */
#else
#define     MOTOR_MCU_CFG_TFU_OPTIMIZE          (MTR_DISABLE)
#endif


/*
 * Motor control common configurations
 */
#define MOTOR_TYPE_BLDC     /* The motor used is BLDC. */

/* Select control loop mode
* - MOTOR_LOOP_SPEED(default)
*/
#define     MOTOR_COMMON_CFG_LOOP_MODE                  (MOTOR_LOOP_SPEED)

#define     MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT    (1.75f)

#define     MOTOR_COMMON_CFG_IA_MAX_CALC_MULT           (MTR_SQRT_3)


/*
 * for Current Module(current)
 */
/* Defines whether use voltage error compensation in FOC */
#define     CURRENT_CFG_VOLT_ERR_COMP           (MTR_DISABLE)

/* Defines whether use stall detection in FOC */
#define     CURRENT_CFG_STALL_DETECTION         (MTR_DISABLE)

/* Defines whether use flying start in FOC */
#define     CURRENT_CFG_FLYING_START            (MTR_DISABLE)

/* Select modulation method
* - MOD_METHOD_SPWM
* - MOD_METHOD_SVPWM(default)
*/
#define     CURRENT_CFG_MODULATION_METHOD       (MOD_METHOD_SVPWM)

#define     CURRENT_CFG_OFFSET_CALC_TIME        (512.0f)                             /* Current offset calculation time */
#define     CURRENT_CFG_CHARGE_BOOTSTRAP_TIME   (144.0f)                             /* Charge time for bootstrap circuit */
#define     CURRENT_CFG_PERIOD_MAG_VALUE        (0.5f)                               /* Period magnification value for coordinate transformation */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VD    (INVERTER_CFG_INPUT_V * 0.5f)        /* Current PI integral term limit for vd */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VQ    (INVERTER_CFG_INPUT_V * 0.5f)        /* Current PI integral term limit for vq */
#define     CURRENT_CFG_OMEGA                   (100.0f)                             /* natural frequency for current loop */
#define     CURRENT_CFG_ZETA                    (1.0f)                               /* damping ratio for current loop */
#define     CURRENT_CFG_REF_ID_OPENLOOP         (2.5f)                               /* id reference when low speed [A] */
#define     CURRENT_CFG_ID_UP_STEP_TIME         (2560.0f)                            /* Time to increase id */
#define     CURRENT_CFG_ID_DOWN_STEP_TIME       (500.0f)                             /* Time to decrease id */
#define     CURRENT_CFG_IQ_DOWN_STEP_TIME_INV   (1.0f / CURRENT_CFG_ID_UP_STEP_TIME) /* Inverse number of time to decrease iq */
#define     CURRENT_CFG_E_OBS_OMEGA             (500.0f)                             /* Natural frequency of BEMF observer */
#define     CURRENT_CFG_E_OBS_ZETA              (1.0f)                               /* Damping ratio of BEMF observer */
#define     CURRENT_CFG_PLL_EST_OMEGA           (20.0f)                              /* Natural frequency of PLL Speed estimate loop */
#define     CURRENT_CFG_PLL_EST_ZETA            (1.0f)                               /* Damping ratio of PLL Speed estimate loop */
#define     CURRENT_CFG_TRQVIB_TARGET_2F        (MTR_FLG_SET)                        /* Wheter to include 2F for Target Vibration Component */
#define     CURRENT_CFG_TRQVIB_COMP_MODE        (TRQCOMP_MODE_PAT)                   /* Generation method of Compensation signal, LUT: 0, PAT: 1 */
#define     CURRENT_CFG_TRQVIB_OUTPUT_GAIN_1F   (0.005f)                             /* Output gain of Torque vibration compensation for 1f */
#define     CURRENT_CFG_TRQVIB_OUTPUT_GAIN_2F   (0.005f)                             /* Output gain of Torque vibration compensation for 2f */
#define     CURRENT_CFG_TRQVIB_TIMELEAP_1F      (0.0f)                               /* Time Leap[rad] of Torque vibration compensation for 1f */
#define     CURRENT_CFG_TRQVIB_TIMELEAP_2F      (4.0f)                               /* Time Leap[rad] of Torque vibration compensation for 2f*/
#define     CURRENT_CFG_TRQVIB_TF_LPF_OMEGA     (0.6f)                               /* Cutt-off frequency for LPF in TF [Hz] */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_2   (1.0f)                               /* Input weight 2 of Repetitive table */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_1   (0.0f)                               /* Input weight 1 of Repetitive table */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_0   (0.0f)                               /* Input weight 0 of Repetitive table */
#define     CURRENT_CFG_TRQVIB_SUPP_TH_1F       (0.05f)                              /* The threshold of learning off for 1f : The ratio of amplitude before and after suppression */
#define     CURRENT_CFG_TRQVIB_SUPP_TH_2F       (0.1f)                               /* The threshold of learning off for 2f : The ratio of amplitude before and after suppression */
#define     CURRENT_CFG_TRQVIB_ABNORMAL_TH_1F   (0.9f)                               /* The threshold of learning off for 1f : The output abnormality of Tracking filter */
#define     CURRENT_CFG_TRQVIB_ABNORMAL_TH_2F   (0.9f)                               /* The threshold of learning off for 2f : The output abnormality of Tracking filter */
#define     CURRENT_CFG_STALL_D_HPF_GAIN        (0.00025f)                           /* HPF gain of Stall detection for d-axis */
#define     CURRENT_CFG_STALL_Q_HPF_GAIN        (0.00025f)                           /* HPF gain of Stall detection for q-axis */
#define     CURRENT_CFG_STALL_THRESHOLD_LEVEL   (5.0f)                               /* Threshold of stall detection level */
#define     CURRENT_CFG_STALL_THRESHOLD_TIME    (0.1f)                               /* Time of stall detection */


/*
 * for Speed Module(speed)
 */
/* Defines whether use flux-weakening in FOC */
#define     SPEED_CFG_FLUX_WEAKENING            (MTR_ENABLE)

/* Defines whether use MTPA in FOC */
#define     SPEED_CFG_MTPA                      (MTR_DISABLE)


/* Defines whether perform soft switching between open-loop and vector control */
#define     SPEED_CFG_LESS_SWITCH               (MTR_ENABLE)

/* Defines whether use the open-loop damping control */
#define     SPEED_CFG_OPENLOOP_DAMPING          (MTR_ENABLE)

#define     SPEED_CFG_CTRL_PERIOD               (0.0005f)       /* control period for speed loop */
#define     SPEED_CFG_OMEGA                     (2.0)//(0.5f)          /* natural frequency for speed loop */
#define     SPEED_CFG_ZETA                      (1.0f)          /* damping ratio for speed loop */
#define     SPEED_CFG_LPF_OMEGA                 (25.0f)         /* natural frequency for speed LPF */
#define     SPEED_CFG_SPEED_LIMIT_RPM           (13000.0f)       /* over speed limit [rpm] (mechanical angle) */
#define     SPEED_CFG_RATE_LIMIT_RPM            (400.0f)        /* Rate limit of speed change [rpm/s] */
#define     SPEED_OPL2LESS_SWITCH_TIME          (0.08f)//(0.0625f)       /* Time[s] to switch open-loop to sensor-less */
#define     SPEED_OPL_DAMP_ED_HPF_OMEGA         (2.5f)          /* HPF cutoff frequency for ed [Hz] */
#define     SPEED_OPL_DAMP_ZETA                 (1.0f)          /* Damping ratio of open-loop damping control */
#define     SPEED_OPL_DAMP_FB_SPEED_LIMIT_RATE  (0.5f)          /* Rate of reference speed for feedback speed limiter */


/*
 * for sensorless vector Manager(sensorless_vector)
 */
#define     SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM                         (500.0f)
                                                            /* Speed to start decreasing id [rpm] */
#define     SENSORLESS_VECTOR_ID_UP_SPEED_RPM                           (400.0f)
                                                            /* Speed to start increasing id [rpm] */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_DEG             (10.0f)
                                                            /* Phase error[deg] to decide sensor-less switch timing */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_RAD             ((SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_DEG * 3.14159265f) / 180.0f)
                                                            /* Phase error[rad] to decide sensor-less switch timing */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_LPF_CUT_FREQ    (10.0f)
                                                            /* Cut off frequency[Hz] of phase error LPF */
#define     SENSORLESS_VECTOR_FLY_START_CURRENT_TH                      (2.0f)
                                                            /* Current threshold of Short-circuit ON [A] */
#define     SENSORLESS_VECTOR_FLY_START_OVER_TIME_SEC                   (0.0025f)
                                                            /* Detection excess time [s] */
#define     SENSORLESS_VECTOR_FLY_START_OFF_TIME_SEC                    (0.001f)
                                                            /* Short-circuit OFF time [s] */
#define     SENSORLESS_VECTOR_FLY_START_ACTIVE_BRAKE_TIME_SEC           (1.0f)
                                                            /* Active braking time [s] */
#define     SENSORLESS_VECTOR_FLY_START_RESTART_SPEED_LIMIT             (660.0f)
                                                            /* Restart judgment speed reference value [r/min] */


/*
 * for Driver Module(driver)
 */
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_START        (R_Config_MOTOR_StartTimerCtrl)
                                                        /* The StartTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_STOP         (R_Config_MOTOR_StopTimerCtrl)
                                                        /* The StopTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_ADC_DATA_GET            (R_Config_MOTOR_AdcGetConvVal)
                                                        /* The AdcGetConvVal function of motor component */
#define     DRIVER_CFG_FUNC_DUTY_SET                (R_Config_MOTOR_UpdDuty)
                                                        /* The UpdDuty function of motor component */
#define     DRIVER_CFG_FUNC_ZERO_DUTY_SET           (R_Config_MOTOR_UpdZeroDuty)
                                                        /* The UpdZeroDuty function of motor component */
#define     DRIVER_CFG_FUNC_COMPARE_DUTY_SET        (R_Config_MOTOR_UpdCompareDuty)
                                                        /* The UpdCompareDuty function of motor component */


/*
 * other
 */
#define     MOTOR_COMMON_AD_SAMPLING_TIME       (MOTOR_MCU_CFG_AD_SAMPLING_CYCLE / MOTOR_MCU_CFG_AD_FREQ)
                                                        /* A/D sampling time [us] */
#define     MOTOR_COMMON_CARRIER_SET_BASE       ((uint16_t)(MOTOR_MCU_CFG_PWM_TIMER_FREQ * 1000 / MOTOR_MCU_CFG_CARRIER_FREQ / 2))
#define     MOTOR_COMMON_DEADTIME_SET           ((uint16_t)(INVERTER_CFG_DEADTIME * MOTOR_MCU_CFG_PWM_TIMER_FREQ))
                                                        /* Setting value of deadtime */
#define     MOTOR_COMMON_CTRL_PERIOD            ((MOTOR_MCU_CFG_INTR_DECIMATION + 1) / (MOTOR_MCU_CFG_CARRIER_FREQ * 1000))
                                                        /* Control period */
/****** Macro of current control*****/
#define     MOTOR_COMMON_ID_UP_STEP_RATE        (CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_UP_STEP_TIME)
                                                        /* addition value of id */
#define     MOTOR_COMMON_ID_DOWN_STEP_RATE      (CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_DOWN_STEP_TIME)
                                                        /* subtraction value of id */
#define     MOTOR_COMMON_PI_INTEGRAL_LIMIT_IQ   (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_3)
                                                        /* speed PI integral term limit for iq */
#define     MOTOR_COMMON_LIMIT_IQ               (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_3)
                                                        /* speed PI limit for iq */
#define     MOTOR_COMMON_CURRENT_OBS_LIMIT      (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_3 * 3.0f)
                                                        /* Integration limit value of voltage disturbance estimate */
#define     MOTOR_COMMON_OVERCURRENT_LIMIT      (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_2 * MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT)
                                                        /* Over current limit [A] */

#endif /* R_MOTOR_MODULE_CFG_H */
