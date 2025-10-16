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
* File Name   : r_motor_speed_api.h
* Description : Definitions of speed control API
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_API_H
#define R_MOTOR_SPEED_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_filter.h"
#include "r_motor_pi_control.h"
#include "r_motor_speed_fluxwkn.h"
#include "r_motor_speed_opl_damp_ctrl.h"
#include "r_motor_speed_opl2less.h"
#include "r_motor_speed_mtpa.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Speed reference status */
#define     SPEED_STATE_ZERO_CONST         (0)
#define     SPEED_STATE_POS_CONTROL_OUTPUT (1)
#define     SPEED_STATE_MANUAL             (2)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    uint8_t       u1_flag_fluxwkn_use;
    float         f4_max_speed_rpm;
    float         f4_speed_ctrl_period;
    float         f4_speed_rate_limit_rpm;
    float         f4_speed_omega_hz;
    float         f4_speed_zeta;
    float         f4_speed_lpf_hz;
    float         f4_opl2less_sw_time;
    float         f4_ed_hpf_omega;
    float         f4_ol_damping_zeta;
    float         f4_ol_damping_fb_limit_rate;
    float         f4_ol_ref_id;
    float         f4_id_down_speed_rpm;
    st_motor_parameter_t st_motor;
} st_speed_cfg_t;

typedef struct
{
    uint8_t       u1_state_speed_ref;
    float         f4_speed_rad;
    float         f4_va_max;
} st_speed_input_t;

typedef struct
{
    float         f4_id_ref;
    float         f4_iq_ref;
    float         f4_ref_speed_rad_ctrl;
    float         f4_speed_rad_lpf;
} st_speed_output_t;

typedef struct
{
	uint8_t		  u1_sc_motor_id;						 /* The motor ID, id->0 is the circulation motor, id->1 is the dry motor */
    uint8_t       u1_active;                         /* Flag to set active/inactive the speed control */
    uint8_t       u1_state_speed_ref;                /* The speed command status */
    uint8_t       u1_flag_fluxwkn_use;               /* whether use the output from flux-weakening */
    uint8_t       u1_flag_switching;                 /* whether set estimated speed flag */
    uint8_t       u1_flag_mtpa_use;                  /* whether use the output from MTPA */
    float         f4_speed_ctrl_period;              /* speed control period */
    float         f4_ref_speed_rad_ctrl;             /* command speed value for speed PI control[rad/s] */
    float         f4_ref_speed_rad;                  /* reference speed value [rad/s] */
    float         f4_ref_speed_rad_manual;           /* reference speed value of speed control mode [rad/s] */
    float         f4_speed_rad_ctrl;                 /* speed_value for speed loop control [rad/s] */
    float         f4_speed_rad;                      /* speed value [rad/s] */
    float         f4_max_speed_rad;                  /* maximum speed command value [rad/s] */
    float         f4_speed_rate_limit_rad;           /* reference speed max change limit [rad/s] */
    float         f4_id_ref_output;                  /* the reference current vector (array) in format : Idref */
    float         f4_iq_ref_output;                  /* the reference current vector (array) in format : Iqref */
    float         f4_va_max;                         /* The maximum magnitude of voltage vector */
    float         f4_id_ad;                          /* The d-axis current value [A] */
    float         f4_iq_ad;                          /* The q-axis current value [A] */
    float         f4_opl2less_sw_time;               /* Time to switch open-loop to sensor-less [s] */
    float         f4_torque_current;                 /* The torque current (Iq) in sensor-less switching */
    st_motor_parameter_t  st_motor;                  /* Motor parameters structure */
    st_pi_ctrl_t          st_pi_speed;               /* PI control structure */
    st_fluxwkn_t          st_fluxwkn;                /* Flux-weakening structure */
    st_opl_damp_t         st_opl_damp;               /* Open loop damping control structure */
    st_1st_order_lpf_t    st_slpf;                   /* LPF for speed */
    st_mtpa_t             st_mtpa;                   /* MTPA structure */
} st_speed_control_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_speed_control_t g_st_sc;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Open
* Description   : Open speed control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Open(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Close
* Description   : Close speed control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Close(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Reset
* Description   : Reset variables when motor speed control
* Arguments     : p_st_sc           - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Reset(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Run
* Description   : Run the motor speed control
* Arguments     : st_sc               - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Run(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterSet
* Description   : Set the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_input   - The pointer to speed control input parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterSet(st_speed_control_t * p_st_sc, const st_speed_input_t * p_st_speed_input);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterGet
* Description   : Get the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_output  - The pointer to speed control output parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterGet(st_speed_control_t * p_st_sc, st_speed_output_t * p_st_speed_output);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterUpdate
* Description   : Update the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_cfg     - The pointer to speed control configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterUpdate(st_speed_control_t * p_st_sc, const st_speed_cfg_t * p_st_speed_cfg);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SpdRefSet
* Description   : Sets parameter for reference speed
* Arguments     : p_st_sc             - The pointer to speed control structure
*                 f4_ref_speed_rpm    - reference speed [rpm]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SpdRefSet(st_speed_control_t * p_st_sc, float f4_ref_speed_rpm);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SpeedCyclic
* Description   : The cyclic processing of speed control
* Arguments     : p_st_sc            - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SpeedCyclic(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Opl2lessReferenceIqCalc
* Description   : Preparations before switching from open-loop to normal FOC
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_ed - d-axis BEMF
*                 f4_eq - q-axis BEMF
*                 f4_id_ref - d-axis current reference
*                 f4_phase_err - phase error between the real motor axis and the controlled axis
* Return Value  : Iq reference
***********************************************************************************************************************/
float R_MOTOR_SPEED_Opl2lessReferenceIqCalc(st_speed_control_t * p_st_sc,
                                            float f4_ed,
                                            float f4_eq,
                                            float f4_id_ref,
                                            float f4_phase_err);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Opl2lessPreprocess
* Description   : Preparations before switching from open-loop to normal FOC
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_id_ref - The Id reference in open-loop drive mode
*                 f4_phase_err_rad_lpf - The phase error [rad] processed by an LPF
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Opl2lessPreprocess(st_speed_control_t * p_st_sc,
                                      float f4_id_ref,
                                      float f4_phase_err_rad_lpf);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_OplDampCtrl
* Description   : Preparations before switching from open-loop to normal FOC
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_ed - d-axis BEMF
* Return Value  : Feedback value for reference speed
***********************************************************************************************************************/
float R_MOTOR_SPEED_OplDampCtrl(st_speed_control_t * p_st_sc, float f4_ed);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_OplDampReset
* Description   : Preparations before switching from open-loop to normal FOC
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_OplDampReset(st_speed_control_t * p_st_sc);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_HuntingSuppress
* Description   : Preset for suppress speed hunting
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_iq_ref - The reference q-axis current value [A]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_HuntingSuppress(st_speed_control_t * p_st_sc, float f4_iq_ref);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SwitchingFlagSet
* Description   : Set the flag to set the estimated speed
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SwitchingFlagSet(st_speed_control_t * p_st_sc);
/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SetMotorID
* Description   : Set the motor ID, ID -> 0 circulation motor, ID -> 1 dry motor
* Arguments     : The motor ID
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SetMotorID(uint8_t motor_id);

#endif /* R_MOTOR_SPEED_API_H */
