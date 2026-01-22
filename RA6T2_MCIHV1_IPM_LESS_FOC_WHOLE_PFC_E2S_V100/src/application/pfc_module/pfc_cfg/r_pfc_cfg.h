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
* File Name    : r_pfc_cfg.h
* Description  : Compile-time configurations
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_CFG_H
#define R_PFC_CFG_H

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/**
 * Middle ware Layer Configurations
 */
/*
 * for MCU
 */
#define PFC_MCU_CFG_PWM_TIMER_FREQ        (120.0f)            /* PWM timer frequency [MHz] */
#define PFC_MCU_CFG_CARRIER_FREQ          (32.0f)             /* Carrier wave frequency [kHz] */
#define PFC_MCU_CFG_INTR_DECIMATION       (0)                 /* Interrupt skipping number */
#define PFC_MCU_CFG_SAMP_PERIOD           ((PFC_MCU_CFG_INTR_DECIMATION + 1) / (PFC_MCU_CFG_CARRIER_FREQ * 1000))
                                                              /* Sampling period [s] */
#define PFC_MCU_CFG_AD12BIT_DATA          (4095.0f)           /* A/D 12Bit data */
#define PFC_MCU_CFG_ADC_OFFSET            (0x7FF)             /* A/D offset */

/* Target Inverter definitions */
#define PFC_CFG_ADC_REF_VOLTAGE           (3.3f)              /* AD reference voltage */
#define PFC_CFG_SHUNT_RESIST              (0.005f)            /* Resistance of shunt resistor [ohm] */
#define PFC_CFG_AC_VOLTAGE_GAIN           (426.5319149f)      /* AC voltage measurement gain */
#define PFC_CFG_BUS_VOLTAGE_GAIN          (174.9130435f)      /* Bus voltage measurement gain */
#define PFC_CFG_CURRENT_AMP_GAIN          (8.333333333f)      /* Current amplifier gain */
#define PFC_CFG_INPUT_VOLTAGE_OFFSET      (0)                 /* Input AC voltage measurement offset */
#define PFC_CFG_BUS_VOLTAGE_OFFSET        (0)                 /* Bus voltage measurement offset */
#define PFC_CFG_CURRENT_OFFSET            (0)                 /* PFC current measurement offset */

/*
 * PFC control common configurations
 */
#define VAC_FREQ                (50.0f)            /* Select 50Hz or 60Hz as AC power supply frequency */

/*
 * for PFC Control Module(pfc_ctrl)
 */
#define DATA_ARR_SIZE           (320)              /* 50Hz:320, 60Hz:267 */

/* Set Vdc reference */
#define VDC_TARGET_VALUE        (390.0f)                             /* The target value of bus voltage [V] */
#define VDC_UP_STEP_RATE        (420.0f * PFC_MCU_CFG_SAMP_PERIOD)   /* Addition value of Vdc */

/* Power limit */
#define PFC_OUT_MAX_POWER       (1000.0f)          /* PFC maximum output power [W] */
#define PFC_OUT_MIN_POWER       (500.0f)           /* PFC minimum output power [W] */
#define PFC_OUT_POWER_COEF      (1.4f)             /* Output power adjustment coefficient (between 1.0 and 1.4) */

/* Automatic voltage regulator */
#define PFC_AVR_KP              (32.9f)            /* The natural frequency is 12Hz */
#define PFC_AVR_KI              (0.003f)
#define PFC_AVR_LIMIT           (500.0f)           /* Default initial value of PI output limit */

/* Automatic current regulator */
#define PFC_ACR_KP              (0.019f)           /* The natural frequency is 1500Hz */
#define PFC_ACR_KI              (0.003f)
#define PFC_ACR_LIMIT           (1.0f)             /* Default initial value of PI output limit */

/* Voltage FF compensation */
#define PFC_AVR_FF_COMP_MIN_LIMIT       (10.0f)    /* Vrms under limit value */

/* Duty ratio FF compensation */
#define PFC_ACR_FF_COMP_MIN_LIMIT       (10.0f)    /* Vdc under limit value */
#define PFC_ACR_DUTY_FF_CTRL_TIME       (0.1f)     /* soft start time for duty FF compensation */
#define PFC_ACR_DUTY_FF_CTRL_CNT_LIMIT  (PFC_ACR_DUTY_FF_CTRL_TIME / PFC_MCU_CFG_SAMP_PERIOD)
                                                   /* The maximum counts for soft start time for duty FF compensation */
#define PFC_ACR_DUTY_FF_COMP_COEF       (0.85f)    /* Compensation coefficient for duty FF compensation */

/* DUTY LIMIT */
#define PFC_DUTY_MAX            (0.968f)           /* minimum off 1usec */
#define PFC_DUTY_MIN            (0.003f)           /* minimum on 0.09375usec */

/*
 * for PFC Manager(pfc_systask)
 */
/* RELAY CONTROL */
#define RELAY_ON_VAC_RMS_MIN    (78.0f)            /* Minimum VAC rms value for relay ON */
#define RELAY_OFF_VAC_RMS_MIN   (70.0f)            /* Minimum VAC rms value for relay OFF */
#define RELAY_ON_DIV_MIN        (10.0f)            /* Minimum voltage deviation for relay ON */
#define RELAY_OFF_DIV_MAX       (100.0f)           /* Maximum voltage deviation for relay OFF */
#define RELAY_ON_DELAY_TIME     (0.1f)             /* Waiting time for relay ON (100 [ms]) */
#define RELAY_OFF_DELAY_TIME    (0.03f)            /* Waiting time for relay OFF (30 [ms]) */
#define RELAY_ON_DELAY_CNT      (RELAY_ON_DELAY_TIME / PFC_MCU_CFG_SAMP_PERIOD)
                                                   /* Waiting time count value for relay ON [count] */
#define RELAY_OFF_DELAY_CNT     (RELAY_OFF_DELAY_TIME / PFC_MCU_CFG_SAMP_PERIOD)
                                                   /* Waiting time count value for relay OFF [count] */

/* PFC Error detected Level */
#define VAC_OVER_VOLTAGE            (388.0f)       /* Input AC over voltage limit [V] */
#define VDC_OVER_VOLTAGE            (450.0f)       /* Bus over voltage limit [V] */
#define VDC_UNDER_VOLTAGE           (80.0f)        /* Bus under voltage limit [V] */
#define ICHP_OVER_CURRENT           (38.0f)        /* PFC over current limit [A] */
#define PFC_OVERHEAT_DETECT_LEVEL   (PFC_LOW)      /* Port level when over heating error is detected */

/* Input voltage dip detection */
#define PFC_VAC_DIP_DETECT_LEVEL    (70.0f)        /* Detection level for input voltage dip */

/* Notch filter */
#define VDC_NOTCH_FILTER_CUT_FREQ   (VAC_FREQ * 2.0f)  /* Cut off frequency[Hz] of VDC notch filter */
#define VDC_NOTCH_FILTER_D          (0.01f)            /* Notch depth of VDC notch filter */
#define VDC_NOTCH_FILTER_ZETA       (0.05f)            /* Bandwidth of VDC notch filter */

/* LPF */
#define VAC_LPF_CUT_FREQ            (2000.0f)          /* Cut off frequency[Hz] of VAC LPF */
#define VDC_LPF_CUT_FREQ            (800.0f)           /* Cut off frequency[Hz] of VDC LPF */
#define ICHP_LPF_CUT_FREQ           (0.0f)             /* Cut off frequency[Hz] of Ichp LPF */

/*
 * for Driver Module(pfc_driver)
 */
#define PFC_DRIVER_CFG_FUNC_PWM_OUTPUT_START    (R_Config_PFC_StartTimerCtrl)
                                                   /* The StartTimerCtrl function of PFC component */
#define PFC_DRIVER_CFG_FUNC_PWM_OUTPUT_STOP     (R_Config_PFC_StopTimerCtrl)
                                                   /* The StopTimerCtrl function of PFC component */
#define PFC_DRIVER_CFG_FUNC_ADC_DATA_GET        (R_Config_PFC_AdcGetConvVal)
                                                   /* The AdcGetConvVal function of PFC component */
#define PFC_DRIVER_CFG_FUNC_DUTY_SET            (R_Config_PFC_UpdDuty)
                                                   /* The UpdDuty function of PFC component */
#define PFC_DRIVER_CFG_FUNC_RELAY_ON            (R_Config_PFC_RelayOn)
                                                   /* The RelayOn function of PFC component */
#define PFC_DRIVER_CFG_FUNC_RELAY_OFF           (R_Config_PFC_RelayOff)
                                                   /* The RelayOff function of PFC component */
#define PFC_DRIVER_CFG_FUNC_OVERHEAT_CHECK      (R_Config_PFC_OverheatCheck)
                                                   /* The OverheatCheck function of PFC component */
#define PFC_DRIVER_PWM_PEAK_CNT                 ((uint16_t)(PFC_MCU_CFG_PWM_TIMER_FREQ * 1000 / PFC_MCU_CFG_CARRIER_FREQ / 2))
                                                   /* Half of the counts per carrier period */
#define PFC_DRIVER_PWM_VALID_MAX_CNT            (PFC_DRIVER_PWM_PEAK_CNT - 1)
                                                   /* Valid maximum PWM timer count for duty setting */
#define PFC_DRIVER_PWM_VALID_MIN_CNT            (1)
                                                   /* Valid minimum PWM timer count for duty setting */

#endif /* R_PFC_CFG_H */
