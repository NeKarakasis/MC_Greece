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
* File Name   : r_motor_current_api.h
* Description : Definitions of current control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_API_H
#define R_MOTOR_CURRENT_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_pi_control.h"
#include "r_motor_current_modulation.h"
#include "r_motor_current_volt_err_comp.h"
#include "r_motor_current_bemf_observer.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Id reference status */
#define     CURRENT_STATE_ID_ZERO_CONST        (0)
#define     CURRENT_STATE_ID_INPUT             (1)
#define     CURRENT_STATE_ID_MANUAL            (2)
#define     CURRENT_STATE_ID_UP                (3)
#define     CURRENT_STATE_ID_DOWN              (4)
/* Iq reference status */
#define     CURRENT_STATE_IQ_ZERO_CONST         (0)
#define     CURRENT_STATE_IQ_SPEED_PI_OUTPUT    (1)
#define     CURRENT_STATE_IQ_AUTO_ADJ           (2)
#define     CURRENT_STATE_IQ_DOWN               (3)

#define     CURRENT_VERR_COMP_LIMIT             (MOTOR_MCU_CFG_CARRIER_FREQ * INVERTER_CFG_DEADTIME / 1000.0f)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/* For 1shunt phase detection */
typedef enum
{
    CURRENT_PHASE_U_PHASE = 1,
    CURRENT_PHASE_V_PHASE,
    CURRENT_PHASE_W_PHASE,
} e_mtr_current_phase_t;
#endif

typedef struct
{
    uint8_t              u1_flag_volt_err_comp_use;    /* Voltage error compensation use flag */
    uint16_t             u2_offset_calc_time;          /* Offset calculation time  */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    int16_t              s2_difference_minimum;        /* Minimum difference of PWM duty */
    int16_t              s2_adjust_adc_delay;          /* Adjustment delay for A/D conversion */
#endif
    float                f4_ctrl_period;               /* The control period */
    float                f4_current_omega_hz;          /* Current PI control omega */
    float                f4_current_zeta;              /* Current PI control zeta */
    float                f4_id_up_step;                /* Reference d-axis current adding value [A] */
    float                f4_id_down_step;              /* Reference d-axis current subtracting value [A] */
    float                f4_iq_down_step_time_inv;     /* Inverse number of time to decrease iq before open-loop [1/ms] */
    float                f4_ol_ref_id;                 /* d-axis current command value in open loop mode [A] */
    st_motor_parameter_t st_motor;                     /* Motor parameter structure */
} st_current_cfg_t;

typedef struct
{
    uint8_t u1_state_id_ref;      /* The d-axis current command status */
    uint8_t u1_state_iq_ref;      /* The q-axis current command status */
    float   f4_iu_ad;             /* U-phase current value */
    float   f4_iv_ad;             /* V-phase current value */
    float   f4_iw_ad;             /* W-phase current value */
    float   f4_vdc_ad;            /* Bus voltage [V] */
    float   f4_id_ref;            /* The reference d-axis current value [A] */
    float   f4_iq_ref;            /* The reference q-axis current value [A] */
    float   f4_ol_speed_rad;      /* Speed value in open loop mode [rad/s] */
} st_current_input_t;

typedef struct
{
    uint8_t u1_flag_offset_calc;  /* Current offset flag */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
    float   f4_modu;              /* U-phase modulation value */
    float   f4_modv;              /* V-phase modulation value */
    float   f4_modw;              /* W-phase modulation value */
#endif
    float   f4_neutral_duty;      /* Duty at the time of offset measurement */
    float   f4_va_max;            /* Maximum voltage on the dq axis */
    float   f4_ref_id_ctrl;       /* The reference d-axis current value [A] */
    float   f4_speed_rad;         /* Speed value [rad/s] */
    float   f4_ed;                /* d-axis BEMF */
    float   f4_eq;                /* q-axis BEMF */
    float   f4_phase_err_rad;     /* Phase error between the real motor axis and the controlled axis */
} st_current_output_t;

typedef struct
{
    float f4_rotor_angle_rad;     /* Rotor angle */
    float f4_sin;                 /* sin */
    float f4_cos;                 /* cos */
} st_rotor_angle_t;

/* Motor current control parameter structure */
typedef struct
{
    uint8_t               u1_active;                   /* Flag to set active/inactive the speed control */
    uint8_t               u1_flag_volt_err_comp_use;   /* Voltage error compensation use flag */
    uint8_t               u1_state_id_ref;             /* The d-axis current command status */
    uint8_t               u1_state_iq_ref;             /* The q-axis current command status */
    uint8_t               u1_flag_offset_calc;         /* q-axis current command value management flag */
    uint16_t              u2_offset_calc_time;         /* Calculation time for current offset */
    uint16_t              u2_crnt_offset_cnt;          /* The number of samples to be acquired in offset measurement */
    float                 f4_ctrl_period;              /* The control period */
    float                 f4_refu;                     /* U-phase reference voltage value [V] */
    float                 f4_refv;                     /* V-phase reference voltage value [V] */
    float                 f4_refw;                     /* W-phase reference voltage value [V] */
    float                 f4_vd_ref;                   /* The reference d-axis voltage value [V] */
    float                 f4_vq_ref;                   /* The reference q-axis voltage value [V] */
    float                 f4_id_ref;                   /* The reference d-axis current value [A] */
    float                 f4_iq_ref;                   /* The reference q-axis current value [A] */
    float                 f4_id_ad;                    /* The d-axis current value [A] */
    float                 f4_iq_ad;                    /* The q-axis current value [A] */
    float                 f4_lim_iq;                   /* The speed PI control output limit value [A}*/
    float                 f4_offset_iu;                /* U-phase current offset value [A] */
    float				  f4_offset_iv;				   /* U-phase current offset value [A] */
    float                 f4_offset_iw;                /* W-phase current offset value [A] */
    float                 f4_sum_iu_ad;                /* U-phase current summation value for offset calculation [A] */
    float                 f4_sum_iw_ad;                /* W-phase current summation value for offset calculation [A] */
    float                 f4_vdc_ad;                   /* Bus voltage [V] */
    float                 f4_iu_ad;                    /* U-phase current value [A] */
    float                 f4_iv_ad;                    /* V-phase current value [A] */
    float                 f4_iw_ad;                    /* W-phase current value [A] */
    float                 f4_modu;                     /* U-phase duty cycle */
    float                 f4_modv;                     /* V-phase duty cycle */
    float                 f4_modw;                     /* W-phase duty cycle */
    float                 f4_speed_rad;                /* Speed value [rad/s] */
    float                 f4_ref_id_ctrl;              /* The reference d-axis current value [A] */
    float                 f4_ref_iq_ctrl;              /* The reference q-axis current value [A] */
    float                 f4_id_up_step;               /* Reference d-axis current adding value [A] */
    float                 f4_id_down_step;             /* Reference d-axis current subtracting value [A] */
    float                 f4_iq_down_step;             /* Reference q-axis current subtracting value [A] */
    float                 f4_iq_down_step_time_inv;    /* Inverse number of time to decrease iq before open-loop */
    float                 f4_ol_ref_id;                /* d-axis current command value in open loop mode [A] */
    float                 f4_va_max;                   /* The maximum magnitude of voltage vector */
    float                 f4_ed;                       /* d-axis BEMF */
    float                 f4_eq;                       /* q-axis BEMF */
    float                 f4_phase_err_rad;            /* Phase error between the real motor axis and the controlled axis */
    float                 f4_ol_speed_rad;             /* Speed value in open loop mode [rad/s] */
    st_mod_t              st_mod;                      /* Modulation structure */
    st_volt_comp_t        st_volt_comp;                /* Compensates voltage error structure */
    st_bemf_observer_t    st_bemf_observer;            /* BEMF observer structure */
    st_pll_est_t          st_pll_est;                  /* PLL estimator structure */
    st_pi_ctrl_t          st_pi_id;                    /* The d-axis current PI controller */
    st_pi_ctrl_t          st_pi_iq;                    /* The q-axis current PI controller */
    st_rotor_angle_t      st_rotor_angle;              /* Rotor angle structure */
    st_motor_parameter_t  st_motor;                    /* Motor parameter structure */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    uint16_t              u2_duty_u;                   /* U-phase duty cycle */
    uint16_t              u2_duty_v;                   /* V-phase duty cycle */
    uint16_t              u2_duty_w;                   /* W-phase duty cycle */
    uint16_t              u2_duty_buff_u;              /* U-phase duty cycle buffer */
    uint16_t              u2_duty_buff_v;              /* V-phase duty cycle buffer */
    uint16_t              u2_duty_buff_w;              /* W-phase duty cycle buffer */
    int16_t               s2_difference_minimum;       /* Minimum difference of PWM duty */
    int16_t               s2_adjust_adc_delay;         /* Adjustment delay for A/D conversion */
    float                 f4_pwm_period_cnt;           /* The number of counts per PWM period  */
    float                 f4_pwm_dead_time_cnt;        /* The number of counts of PWM deadtime */
    e_mtr_current_phase_t min_phase;                   /* Minimum phase information to calculate 1shunt current */
    e_mtr_current_phase_t mid_phase;                   /* Middle phase information to calculate 1shunt current */

    /* The function pointer for 1ShuntUpdDuty function in PWM timer component */
    void (*BLDC1ShuntDutySet)(uint16_t duty_u,
                              uint16_t duty_v,
                              uint16_t duty_w,
                              uint16_t duty_u_buff,
                              uint16_t duty_v_buff,
                              uint16_t duty_w_buff);

    /* The function pointer for AdcTriggerSet function in PWM timer component */
    void (*ADCTriggerSet)(uint16_t compare_match_value_a, uint16_t compare_match_value_b);

    /* The function pointer for MaxCountGet function in PWM timer component */
    uint16_t (*PWMMaxCountGet)(void);
#endif
} st_current_control_t;

typedef struct
{
    float f4_e_obs_omega_hz;      /* Natural frequency for BEMF observer [Hz] */
    float f4_e_obs_zeta;          /* Damping ratio for BEMF observer */
    float f4_pll_est_omega_hz;    /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
    float f4_pll_est_zeta;        /* Damping ratio for rotor position Phase-Locked Loop */
} st_bemf_observer_cfg_t;

#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
typedef struct
{
    /* The function pointer for 1ShuntUpdDuty function in PWM timer component */
    void (*BLDC1ShuntDutySet)(uint16_t duty_u,
                              uint16_t duty_v,
                              uint16_t duty_w,
                              uint16_t duty_u_buff,
                              uint16_t duty_v_buff,
                              uint16_t duty_w_buff);

    /* The function pointer for AdcTriggerSet function in PWM timer component */
    void (*ADCTriggerSet)(uint16_t compare_match_value_a, uint16_t compare_match_value_b);

    /* The function pointer for MaxCountGet function in PWM timer component */
    uint16_t (*PWMMaxCountGet)(void);
} st_current_driver_cfg_t;
#endif

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_current_control_t g_st_cc;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Open
* Description   : Open current control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Open(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Close
* Description   : Close current control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Close(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Reset
* Description   : Resets current control module
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Reset(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Run
* Description   : Run current control module
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Run(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterSet
* Description   : Set the current control parameter
* Arguments     : p_st_cc            - The pointer to the current control structure
*                 p_st_current_input - The pointer to the current input parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterSet(st_current_control_t *p_st_cc, const st_current_input_t * p_st_current_input);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterGet
* Description   : Get the current control parameter
* Arguments     : p_st_cc             - The pointer to the current control structure
*                 p_st_current_output - The pointer to the current control output parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterGet(st_current_control_t *p_st_cc, st_current_output_t * p_st_current_output);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterUpdate
* Description   : Update the current control parameter
* Arguments     : p_st_cc          - The pointer to current control structure
*                 p_st_current_cfg - The pointer to current control configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterUpdate(st_current_control_t *p_st_cc, const st_current_cfg_t *p_st_current_cfg);

#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_1ShuntCurrentGet
* Description   : Determine the three phase current for 1shunt
* Arguments     : p_st_cc     - The pointer to the current control structure
*                 p_f4_iu_ad  - U phase current A/D conversion value (pointer)
*                 p_f4_iv_ad  - V phase current A/D conversion value (pointer)
*                 p_f4_iw_ad  - W phase current A/D conversion value (pointer)
*                 p_f4_Iac_ad - Phase current A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_1ShuntCurrentGet(st_current_control_t *p_st_cc,
                                      float * p_f4_iu_ad,
                                      float * p_f4_iv_ad,
                                      float * p_f4_iw_ad,
                                      float * p_f4_Iac_ad);
#endif

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_CurrentCyclic
* Description   : The cyclic processing of current control
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_CurrentCyclic(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_OffsetCalibration
* Description   : Measures the current offset
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_OffsetCalibration(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_CurrentOffsetRemove
* Description   : Calibrates the current offset
* Arguments     : p_st_cc - The pointer to the current control structure
*                 p_f4_iu - The pointer to measured phase u current
*                 p_f4_iw - The pointer to measured phase w current
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_CurrentOffsetRemove(st_current_control_t *p_st_cc, float *p_f4_iu,float *p_f4_iv, float *p_f4_iw);

/**********************************************************************************************************************
 * Function Name: R_MOTOR_CURRENT_VoltErrCompParamSet
 * Description  : Set voltage error compensation parameters
 * Arguments    : p_st_cc -
 *                   The pointer to the current control structure
 *                crnt_array -
 *                   The current array [A]
 *                volterr_array
 *                   The voltage error array [V]
 *                ref_voltage
 *                   The DC bus voltage that table created for
 * Return Value : None
 *********************************************************************************************************************/
void R_MOTOR_CURRENT_VoltErrCompParamSet(st_current_control_t *p_st_cc,
                                         const float *crnt_array,
                                         const float *volterr_array,
                                         float ref_voltage);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_BEMFObserverParameterUpdate
* Description   : Update the BEMF observer parameter
* Arguments     : p_st_cc           - The pointer to current control structure
*                 p_st_bemf_obs_cfg - The pointer to BEMF observer configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_BEMFObserverParameterUpdate(st_current_control_t * p_st_cc,
                                                 st_bemf_observer_cfg_t * p_st_bemf_obs_cfg);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_HuntingSuppress
* Description   : Preset for suppress speed hunting
* Arguments     : p_st_cc - The pointer to current control structure
*                 f4_ref_speed_rad_ctrl - Command speed value
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_HuntingSuppress(st_current_control_t * p_st_cc, float f4_ref_speed_rad_ctrl);

#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/***********************************************************************************************************************
* Function Name: R_MOTOR_CURRENT_1shuntModulation
* Description  : Perform PWM modulation for 1shunt
* Arguments    : p_st_cc - The pointer to the current control structure
* Return Value : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_1shuntModulation(st_current_control_t *p_st_cc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_DriverParameterUpdate
* Description   : Update the current driver parameter
* Arguments     : p_st_cc                 - The pointer to current control structure
*                 p_st_current_driver_cfg - The pointer to current driver configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_DriverParameterUpdate(st_current_control_t * p_st_cc,
                                           const st_current_driver_cfg_t * p_st_current_driver_cfg);
#endif

#endif /* R_MOTOR_CURRENT_API_H */
