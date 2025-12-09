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
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
*         : 01.03.2024 1.20
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

//#define MOTOR_SHUNT_TYPE_1_SHUNT     /* Only use V phase current */
#define MOTOR_SHUNT_TYPE_2_SHUNT      /* Use U and W phase current */

/*
 * Software system configurations
 */
/*
 * for MCU
 */
#define     MOTOR_MCU_CFG_PWM_TIMER_FREQ        (32.0f)             /* PWM timer frequency [MHz] */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     MOTOR_MCU_CFG_CARRIER_FREQ          (5.0f)              /* Carrier wave frequency [kHz] */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     MOTOR_MCU_CFG_CARRIER_FREQ          (6.0f)             /* Carrier wave frequency [kHz] */
#endif
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     MOTOR_MCU_CFG_INTR_DECIMATION       (0)                 /* Interrupt skipping number */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     MOTOR_MCU_CFG_INTR_DECIMATION       (0)                 /* Interrupt skipping number */
#endif
#define     MOTOR_MCU_CFG_AD_FREQ               (32.0f)             /* A/D frequency [MHz] */
#define     MOTOR_MCU_CFG_AD_SAMPLING_CYCLE     (8 + 39)            /* Scan start processing time(tD) + sample-and-hold processing time(tSPLSH) [cycle] */
#define     MOTOR_MCU_CFG_AD12BIT_DATA          (4095.0f)           /* A/D 12Bit data */
#define     MOTOR_MCU_CFG_ADC_OFFSET            (0xA3A)//(0x7FF)             /* A/D offset */

#if defined(__TFU)
#define     MOTOR_MCU_CFG_TFU_OPTIMIZE          (MTR_ENABLE)        /* TFU Optimization */
#else
#define     MOTOR_MCU_CFG_TFU_OPTIMIZE          (MTR_DISABLE)
#endif


/*
 * Motor control common configurations
 */
#define MOTOR_TYPE_BLDC     /* The motor used is BLDC. */
//#define MOTOR_TYPE_STM      /* The motor used is STM. */

/* Select control loop mode
* - MOTOR_LOOP_SPEED(default)
*/
#define     MOTOR_COMMON_CFG_LOOP_MODE                  (MOTOR_LOOP_SPEED)

#define     MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT    (2.0f)

#if defined(MOTOR_TYPE_BLDC)
#define     MOTOR_COMMON_CFG_IA_MAX_CALC_MULT           (MTR_SQRT_3)
#elif defined(MOTOR_TYPE_STM)
#define     MOTOR_COMMON_CFG_IA_MAX_CALC_MULT           (MTR_SQRT_2)
#endif


/*
 * for Current Module(current_rx)
 */
/* Defines whether use voltage error compensation in FOC */
#define     CURRENT_CFG_VOLT_ERR_COMP           (MTR_DISABLE)

/* Select modulation method
* - MOD_METHOD_SPWM
* - MOD_METHOD_SVPWM(default)
*/
#define     CURRENT_CFG_MODULATION_METHOD       (MOD_METHOD_SVPWM)

#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     CURRENT_CFG_OFFSET_CALC_TIME        (128.0f)                            /* Current offset calculation time */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     CURRENT_CFG_OFFSET_CALC_TIME        (256.0f)                            /* Current offset calculation time */
#endif
#define     CURRENT_CFG_PERIOD_MAG_VALUE        (1.0f)                              /* Period magnification value for coordinate transformation */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VD    (INVERTER_CFG_INPUT_V * 0.5f)       /* Current PI integral term limit for vd */
#define     CURRENT_CFG_PI_INTEGRAL_LIMIT_VQ    (INVERTER_CFG_INPUT_V * 0.5f)       /* Current PI integral term limit for vq */
#define		CURRENT_CFG_LPF_OMEGA				(2.f * 3.141592f * 2000.f)			/* Cut off frequency of 1st order LPF for the currents*/
#define		CURRENT_CFG_CTRL_PERIOD				(1.0F/(MOTOR_MCU_CFG_CARRIER_FREQ * 1000.0f)) /* Period of the current control loop, it needs for LPF filter for currents */
#define     CURRENT_CFG_OMEGA                   (300.0f)                            /* natural frequency for current loop */ //50
#define     CURRENT_CFG_ZETA                    (1.0f)                              /* damping ratio for current loop */
#define     CIRC_CURRENT_CFG_REF_ID_OPENLOOP    (0.7f)                              /* id reference when low speed [A] for the circulation motor */
#define     DRY_CURRENT_CFG_REF_ID_OPENLOOP     (0.3f)								/* id reference when low speed [A] for the dry motor */
#define		CURRENT_CFG_REF_ID_OPENLOOP(ch) 		(float)(((ch)==0 )?   (CIRC_CURRENT_CFG_REF_ID_OPENLOOP) : (DRY_CURRENT_CFG_REF_ID_OPENLOOP) )
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     CURRENT_CFG_ID_UP_STEP_TIME         (640.0f)                            /* Time to increase id */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     CURRENT_CFG_ID_UP_STEP_TIME         (1250.0f)                           /* Time to increase id */ //1250
#endif
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     CURRENT_CFG_ID_DOWN_STEP_TIME       (125.0f)                            /* Time to decrease id */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     CURRENT_CFG_ID_DOWN_STEP_TIME       (2000.0f)                            /* Time to decrease id */  //1000 or 2000
#endif
#define     CURRENT_CFG_IQ_DOWN_STEP_TIME_INV   (1.0f /CURRENT_CFG_ID_UP_STEP_TIME) /* Inverse number of time to decrease iq */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     CURRENT_CFG_E_OBS_OMEGA             (550.0f)                            /* Natural frequency of BEMF observer */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)

#define     CIRC_CURRENT_CFG_E_OBS_OMEGA         (400.0f)                            /* Natural frequency of BEMF observer */ //100


#define     DRY_CURRENT_CFG_E_OBS_OMEGA          (300.0f)


#define     CURRENT_CFG_E_OBS_OMEGA(ch) 			(float)(((ch)==0 )?   (CIRC_CURRENT_CFG_E_OBS_OMEGA) : (DRY_CURRENT_CFG_E_OBS_OMEGA) )

#endif
#define     CURRENT_CFG_E_OBS_ZETA              	(1.0f)                              /* Damping ratio of BEMF observer */

#define     CIRC_CURRENT_CFG_PLL_EST_OMEGA      	(25.0f)                             /* Natural frequency of PLL Speed estimate loop */ //10

#define     DRY_CURRENT_CFG_PLL_EST_OMEGA           (10.0f)                             /* Natural frequency of PLL Speed estimate loop */ //10

#define     CURRENT_CFG_PLL_EST_OMEGA(ch)           	(float)(((ch)==0 )?   (CIRC_CURRENT_CFG_PLL_EST_OMEGA) : (DRY_CURRENT_CFG_PLL_EST_OMEGA) )                             /* Natural frequency of PLL Speed estimate loop */ //10

#define     CURRENT_CFG_PLL_EST_ZETA            (1.0f)                              /* Damping ratio of PLL Speed estimate loop */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     CURRENT_CFG_MIN_DIFFERENCE_DUTY     (192)                               /* Minimum difference of PWM duty */
#define     CURRENT_CFG_ADJUST_ADC_DELAY        (64)                                /* Adjustment delay for A/D conversion */

/* Current driver settings */
#define     CURRENT_CFG_FUNC_DUTY_SET           (R_Config_MTU3_MTU4_1ShuntUpdDuty)  /* Duty set function */
#define     CURRENT_CFG_FUNC_ADC_TRIGGER_SET    (R_Config_MTU3_MTU4_AdcTriggerSet)  /* AD double trigger timing set function */
#define     CURRENT_CFG_FUNC_PWM_MAX_COUNT_GET  (R_Config_MTU3_MTU4_MaxCountGet)    /* PWM max count get function */
#endif


/*
 * for Speed Module(speed_rx)
 */
/* Defines whether use flux-weakening in FOC */
#define     SPEED_CFG_FLUX_WEAKENING            (MTR_DISABLE)

/* Defines whether use MTPA in FOC */
#define     SPEED_CFG_MTPA                      (MTR_DISABLE)

/* Defines whether perform soft switching between open-loop and vector control */
#define     SPEED_CFG_LESS_SWITCH               (MTR_ENABLE)

/* Defines whether use the open-loop damping control */
#define     SPEED_CFG_OPENLOOP_DAMPING          (MTR_DISABLE)

#define     SPEED_CFG_CTRL_PERIOD               (0.001f)        /* control period for speed loop */

#define     CIRC_SPEED_CFG_OMEGA                     (0.4f)          /* natural frequency for speed loop */ //1
#define     DRY_SPEED_CFG_OMEGA                     (0.03f)

#define     SPEED_CFG_OMEGA(ch)                     (float)(((ch)==0 )?   (CIRC_SPEED_CFG_OMEGA) : (DRY_SPEED_CFG_OMEGA) )

#define     SPEED_CFG_ZETA                      (1.0f)          /* damping ratio for speed loop */
#define     SPEED_CFG_LPF_OMEGA                 (10.0f)         /* natural frequency for speed LPF */
#define     SPEED_CFG_SPEED_LIMIT_RPM           (6000.0f)       /* over speed limit [rpm] (mechanical angle) */
#define     SPEED_CFG_RATE_LIMIT_RPM            (500.0f)       /* Rate limit of speed change [rpm/s] */
#define     SPEED_OPL2LESS_SWITCH_TIME          (1.5f)        /* Time[s] to switch open-loop to sensor-less */ //0.8
#define     SPEED_OPL_DAMP_ED_HPF_OMEGA         (2.5f)          /* HPF cutoff frequency for ed [Hz] */
#define     SPEED_OPL_DAMP_ZETA                 (1.0f)          /* Damping ratio of open-loop damping control */
#define     SPEED_OPL_DAMP_FB_SPEED_LIMIT_RATE  (0.2f)          /* Rate of reference speed for feedback speed limiter */


/*
 * for sensorless vector Manager(sensorless_vector_rx)
 */
#define     CIRC_SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM                    (800.0f) //800
#define     DRY_SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM                     (1500.f)

#define		SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM(ch)						(float)(((ch)==0 )?   (CIRC_SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM) : (DRY_SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM) )

#define     SENSORLESS_VECTOR_ID_UP_SPEED_RPM                           (500.0f)
                                                            /* Speed to start increasing id [rpm] */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_DEG             (3.0f)
                                                            /* Phase error[deg] to decide sensor-less switch timing */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_RAD             ((SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_DEG * 3.14159265f) / 180.0f)
                                                            /* Phase error[rad] to decide sensor-less switch timing */
#define     SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_LPF_CUT_FREQ    (10.0f)
                                                            /* Cut off frequency[Hz] of phase error LPF */


/*
 * for Driver Module(driver_rx)
 */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_START    (R_Config_MTU3_MTU4_StartTimerCtrl)
                                                        /* The StartTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_STOP     (R_Config_MTU3_MTU4_StopTimerCtrl)
                                                        /* The StopTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_ADC_DATA_GET        (R_Config_MTU3_MTU4_1ShuntAdcGetConvVal)
                                                        /* The AdcGetConvVal function of motor component */
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_START    (R_Config_MOTOR_StartTimerCtrl)
                                                        /* The StartTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_PWM_OUTPUT_STOP     (R_Config_MOTOR_StopTimerCtrl)
                                                        /* The StopTimerCtrl function of motor component */
#define     DRIVER_CFG_FUNC_ADC_DATA_GET        (R_Config_MOTOR_AdcGetConvVal)
                                                        /* The AdcGetConvVal function of motor component */
#define     DRIVER_CFG_FUNC_DUTY_SET            (R_Config_MOTOR_UpdDuty)
                                                        /* The UpdDuty function of motor component */
#endif
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
#define     MOTOR_COMMON_ID_UP_STEP_RATE(ch)		(CURRENT_CFG_REF_ID_OPENLOOP(ch) / CURRENT_CFG_ID_UP_STEP_TIME)

/*
#define     CIRC_MOTOR_COMMON_ID_UP_STEP_RATE       (CIRC_CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_UP_STEP_TIME)
                                                         addition value of id
#define     DRY_MOTOR_COMMON_ID_UP_STEP_RATE        (DRY_CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_UP_STEP_TIME)
*/
                                                        /* addition value of id */
#define     MOTOR_COMMON_ID_DOWN_STEP_RATE(ch)		(CURRENT_CFG_REF_ID_OPENLOOP(ch) / CURRENT_CFG_ID_DOWN_STEP_TIME)

#define     CIRC_MOTOR_COMMON_ID_DOWN_STEP_RATE     (CIRC_CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_DOWN_STEP_TIME)
                                                        /* subtraction value of id */
#define     DRY_MOTOR_COMMON_ID_DOWN_STEP_RATE      (DRY_CURRENT_CFG_REF_ID_OPENLOOP / CURRENT_CFG_ID_DOWN_STEP_TIME)
                                                        /* subtraction value of id */
#define     MOTOR_COMMON_PI_INTEGRAL_LIMIT_IQ(ch)   (MOTOR_CFG_NOMINAL_CURRENT_RMS(ch) * MTR_SQRT_3)
                                                        /* speed PI integral term limit for iq */
#define     MOTOR_COMMON_LIMIT_IQ(ch)               (MOTOR_CFG_NOMINAL_CURRENT_RMS(ch) * MTR_SQRT_3)
                                                        /* speed PI limit for iq */
#define     MOTOR_COMMON_CURRENT_OBS_LIMIT(ch)      (MOTOR_CFG_NOMINAL_CURRENT_RMS(ch) * MTR_SQRT_3 * 3.0f)
                                                        /* Integration limit value of voltage disturbance estimate */
#define     MOTOR_COMMON_OVERCURRENT_LIMIT(ch)      (MOTOR_CFG_NOMINAL_CURRENT_RMS(ch) * MTR_SQRT_2 * MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT)
                                                        /* Over current limit [A] */
#endif /* R_MOTOR_MODULE_CFG_H */
