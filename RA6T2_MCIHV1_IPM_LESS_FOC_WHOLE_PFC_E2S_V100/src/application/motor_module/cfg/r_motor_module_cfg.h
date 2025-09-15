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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_motor_module_cfg.h
* Description  : Compile-time configurations
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
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
#define     MOTOR_MCU_CFG_CARRIER_FREQ          (20.0f)                          /* Carrier wave frequency [kHz] */
#define     MOTOR_MCU_CFG_INTR_DECIMATION       (0)                             /* Interrupt skipping number */
#define     MOTOR_MCU_CFG_AD_FREQ               (60.0f)                         /* A/D frequency [MHz] */
#define     MOTOR_MCU_CFG_AD_SAMPLING_CYCLE     (2.0f * (7.25f + 120.0f))        /* twice the (Scan start processing time(tD) + sample-and-hold processing time(tSPLSH)) [cycle] */
#define     MOTOR_MCU_CFG_AD12BIT_DATA          (4095.0f)                       /* A/D 12Bit data */
#define     MOTOR_MCU_CFG_ADC_OFFSET            (0x7FF)                         /* A/D offset */

#if defined(BSP_FEATURE_TFU_SUPPORTED)
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

#define     MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT    (1.7f)

#define     MOTOR_COMMON_CFG_IA_MAX_CALC_MULT           (MTR_SQRT_3)


/*
 * for Current Module(current_rx)
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
#define     CURRENT_CFG_PERIOD_MAG_VALUE        (0.5f)                               /* Period magnification value for coordinate transformation */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VD    (INVERTER_CFG_INPUT_V * 0.5f)        /* Current PI integral term limit for vd */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VQ    (INVERTER_CFG_INPUT_V * 0.5f)        /* Current PI integral term limit for vq */

#define     CURRENT_CFG_OMEGA                   (150.0f)                             /* natural frequency for current loop */
#define     CURRENT_CFG_ZETA                    (1.0f)                               /* damping ratio for current loop */

#define     CURRENT_CFG_E_OBS_OMEGA             (900.0f)                             /* Natural frequency of BEMF observer */
#define     CURRENT_CFG_E_OBS_ZETA              (1.0f)                               /* Damping ratio of BEMF observer */

#define     CURRENT_CFG_ESTLOW_PULSEVOLT         (200.0f)                            /* Natural frequency of PLL Speed estimate loop */
#define     CURRENT_CFG_ESTLOW_PULSEVOLT_RUNNING (120.0f)                            /* Natural frequency of PLL Speed estimate loop */
#define     CURRENT_CFG_ESTLOW_ESTTIME           (0.3f/MOTOR_COMMON_CTRL_PERIOD)     /* Position convergence period time counter [200ms] */
#define     CURRENT_CFG_ESTLOW_ESTTIME_OVER      (0.4f/MOTOR_COMMON_CTRL_PERIOD)     /* Position convergence period+judgment period */

/* judgment time counter [300ms] */
#define     CURRENT_CFG_PLL_ESTLOW_OMEGA         (20.0f)                             /* Natural frequency of PLL Speed estimate loop */
#define     CURRENT_CFG_PLL_ESTLOW_ZETA          (1.0f)                              /* Damping ratio of PLL Speed estimate loop */

/* Carrier frequency :8kHz,4kHz,2kHz <abzenc debug> */
#define     CURRENT_CFG_PLL_EST_OMEGA            (20.0f)                             /* Natural frequency of PLL Speed estimate loop */
#define     CURRENT_CFG_PLL_EST_ZETA             (1.0f)                              /* Damping ratio of PLL Speed estimate loop */

#define     CURRENT_CFG_ESTLOW_PULSEFREQ_BOOT    (8)                                 /* Pulse injection frequency 1/f at the boot mode */
#define     CURRENT_CFG_ESTLOW_PULSEFREQ_DRIVE   (6)                                 /* Pulse injection frequency 1/f at the drive */

#define     CURRENT_CFG_ESTLOW_PF_THRESHOLD                  (2.5f)                  /* Threshold of polarity */
#define     CURRENT_CFG_ESTLOW_CONVERGENCE_COUNT             (10)                    /* Magnetic pole position estimation convergence judgment counter*/
#define     CURRENT_CFG_ESTLOW_STATE_TRANS_WAIT_SLOW_TO_HIGH (100)                   /* Slow to high speed command value judgment counter */
#define     CURRENT_CFG_ESTLOW_STATE_TRANS_WAIT_HIGH_TO_SLOW (100)                   /* High to slow speed command value judgment counter*/

#define     CURRENT_CFG_TRQVIB_OUTPUT_GAIN      (0.001f)                             /* Output gain of Torque vibration compensation */
#define     CURRENT_CFG_TRQVIB_TIMELEAP         (0.0f)                               /* Time Leap[rad] of Torque vibration compensation */
#define     CURRENT_CFG_TRQVIB_LPF_GAIN         (0.0005f)                            /* LPF gain of Torque vibration compensation */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_2   (1.0f)                               /* Input weight 2 of Repetitive table */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_1   (0.0f)                               /* Input weight 1 of Repetitive table */
#define     CURRENT_CFG_TRQVIB_INPUT_WEIGHT_0   (0.0f)                               /* Input weight 0 of Repetitive table */
#define     CURRENT_CFG_STALL_D_HPF_GAIN        (0.00025f)                           /* LPF gain of Torque vibration compensation for d-axis */
#define     CURRENT_CFG_STALL_Q_HPF_GAIN        (0.00025f)                           /* LPF gain of Torque vibration compensation for q-axis */
#define     CURRENT_CFG_STALL_THRESHOLD_LEVEL   (5.0f)                               /* LPF gain of Torque vibration compensation */
#define     CURRENT_CFG_STALL_THRESHOLD_TIME    (0.1f)                               /* LPF gain of Torque vibration compensation */


/*
 * for Speed Module(speed_rx)
 */
/* Defines whether use flux-weakening in FOC */
#define     SPEED_CFG_FLUX_WEAKENING            (MTR_ENABLE)

/* Defines whether use Speed observer in FOC */
#define     SPEED_CFG_OBSERVER                  (MTR_ENABLE)

/* Defines whether use MTPA in FOC */
#define     SPEED_CFG_MTPA                      (MTR_ENABLE)

#define     SPEED_CFG_CTRL_PERIOD               (0.0005f)       /* control period for speed loop */
#define     SPEED_CFG_OMEGA                     (2.0f)          /* natural frequency for speed loop */
#define     SPEED_CFG_ZETA                      (1.0f)          /* damping ratio for speed loop */
#define     SPEED_CFG_LPF_OMEGA                 (25.0f)         /* natural frequency for speed LPF */
#define     SPEED_CFG_SPEED_LIMIT_RPM           (13000.0f)       /* over speed limit [rpm] (mechanical angle) */

#define     SPEED_CFG_SPEED_LIMIT_MARGIN        (1.1f)            /* over speed limit margin (+10%) */
#define     SPEED_CFG_SPEED_LIMIT_RAD           (SPEED_CFG_SPEED_LIMIT_RPM * SPEED_CFG_SPEED_LIMIT_MARGIN * MOTOR_CFG_POLE_PAIRS * MTR_RPM2RAD)
                                                                  /* over speed limit [rad] (electrical angle) */
#define     SPEED_CFG_RATE_LIMIT_RPM            (3000.0f)        /* Rate limit of speed change [rpm/s] */
#define     SPEED_CFG_RATE_LIMIT_RPM1           (3000.0f)        /* Rate limit of speed change [rpm/s], initial */
#define     SPEED_CFG_RATE_LIMIT_RPM2           (3000.0f)
#define     SPEED_CFG_RATE_LIMIT_RPM3           (3000.0f)


#define     SPEED_CFG_SOB_OMEGA                 (7.5f)           /* natural frequency for speed observer */
#define     SPEED_CFG_SOB_OUTLIM_START_RPM      (25.0f)           /* limit start speed for speed observer */
#define     SPEED_CFG_SOB_OUTLIM_END_RPM        (30.0f)           /* limit end speed for speed observer */

/*
 * for sensorless vector Manager(sensorless_vector_rx)
 */
#define     SENSORLESS_VECTOR_FLY_START_CURRENT_TH                      (2.0f)
                                                            /* Current threshold of Short-circuit ON [A] */
#define     SENSORLESS_VECTOR_FLY_START_OVER_TIME_SEC                   (0.005f)
                                                            /* Detection excess time [s]  (Temporarily change it from 0.0005 to 0.005) */
#define     SENSORLESS_VECTOR_FLY_START_OFF_TIME_SEC                    (0.0005f)
                                                            /* Short-circuit OFF time [s] */
#define     SENSORLESS_VECTOR_FLY_START_ACTIVE_BRAKE_TIME_SEC           (1.0f)
                                                            /* Active braking time [s] */
#define     SENSORLESS_VECTOR_FLY_START_RESTART_SPEED_LIMIT             (600.0f)
                                                            /* Restart judgment speed reference value [r/min] */


/*
 * for Driver Module(driver_rx)
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
#define     MOTOR_COMMON_PI_INTEGRAL_LIMIT_IQ   (MOTOR_CFG_NOMINAL_CURRENT_RMS *  MTR_SQRT_3)
                                                        /* speed PI integral term limit for iq */
#define     MOTOR_COMMON_LIMIT_IQ               (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_3)
                                                        /* speed PI limit for iq */
#define     MOTOR_COMMON_CURRENT_OBS_LIMIT      (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_3 * 3.0f)
                                                        /* Integration limit value of voltage disturbance estimate */
#define     MOTOR_COMMON_OVERCURRENT_LIMIT      (MOTOR_CFG_NOMINAL_CURRENT_RMS * MTR_SQRT_2 * MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT)
                                                        /* Over current limit [A] */
#define     MOTOR_ANGEST_THRESHOLD              (0.5f * (MTR_TWOPI / 2.0f) / 180.0f)
                                                        /* Threshold of angle estimation [rad] */
#define     MOTOR_PF_THRESHOLD                  (10.0f) /* Threshold of polarity */

#endif /* R_MOTOR_MODULE_CFG_H */
