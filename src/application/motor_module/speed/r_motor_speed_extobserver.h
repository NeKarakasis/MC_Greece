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
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_speed_extobserver.h
* Description : Extended observer which estimates speed and disturbance torque from torque command and measured speed
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 20.12.2022 1.00
***********************************************************************************************************************/

#ifndef R_MOTOR_SPEED_EXTOBSERVER_H
#define R_MOTOR_SPEED_EXTOBSERVER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/



/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/

/* First order LPF structure */
typedef struct
{
    float f4_pre_output;      /* Previous value of output */
    float f4_pre_input;       /* Previous value of input */
    float f4_omega_t;         /* The value of (Angular speed) * (control period) */
    float f4_gain_ka;         /* LPF gain A */
    float f4_gain_kb;         /* LPF gain B */
}st_extobs_lpf_t;

/* Main structure */
typedef struct
{
    float           f4_gain_distb_est;           /* The gain of disturbance estimator */
    float           f4_gain_speed_est;           /* The gain of speed estimator */
    float           f4_estimated_distb;          /* The estimated disturbance */
    float           f4_estimated_speed;          /* The estimated speed */
    float           f4_inertia_kgms2;            /* The total inertia of system */
    float           f4_natural_freq_hz;          /* The natural frequency [Hz] */
    float           f4_ctrl_period_s;            /* The interval of estimator execution */
    float           f4_nominal_cur_peak;         /* Nominal current peak*/
    float           f4_nominal_cur;              /* Nominal current */
    float           f4_outputlim_start_rad;      /*  */
    float           f4_outputlim_end_rad;        /*  */
    st_extobs_lpf_t st_lpf;                      /* The LPF*/
} st_extobs_t;

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_init
* Description   : Initializes the extended observer with given parameters
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_natural_freq_hz - The natural frequency [Hz] that determine gain of the observer
*                 f4_ctrl_period_s   - The sample period [s]
*                 f4_inertia_kgms2   - The inertia moment [kgm/s^2]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_init(st_extobs_t * p_st_extobs,
                                  float f4_natural_freq_hz,
                                  float f4_ctrl_period_s,
                                  float f4_inertia_kgms2,
                                  float f4_outputlim_start,
                                  float f4_outputlim_end,
                                  float f4_nominal_current);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_reset
* Description   : Resets the extended observer without reseting the parameters
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_reset(st_extobs_t * p_st_extobs);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_start
* Description   : Executes the extended observer tick processes
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_torque_cmd_nm - The torque command [Nm], the actual torque output should be used if the Iq saturate
*                 f4_speed_rad     - The rotor speed [rad/s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_start(st_extobs_t * p_st_extobs, float f4_torque_cmd_nm, float f4_speed_rad);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_mech_speed_get
* Description   : Gets the estimated mechanical speed
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : The estimated mechanical speed[rad/s]
***********************************************************************************************************************/
float motor_speed_extobserver_mech_speed_get(st_extobs_t * p_st_extobs);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_disturbance_get
* Description   : Gets the estimated torque
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : The estimated torque[Nm]
***********************************************************************************************************************/
float motor_speed_extobserver_disturbance_get(st_extobs_t * p_st_extobs);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_natural_freq_set
* Description   : Sets the natural frequency of estimators
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_natural_freq_hz - The natural frequency [Hz] to set
*                 f4_ctrl_period_s   - The control period [s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_natural_freq_set(st_extobs_t * p_st_extobs,
                                              float f4_natural_freq_hz,
                                              float f4_ctrl_period_s);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_init_speed_set
* Description   : Sets the initial speed (estimated speed), use this function to synchronize the speed before
*                 calling motor_speed_extobserver_start function
* Arguments     : p_st_extobs  - The pointer to the extended observer structure
*                 f4_speed_rad - The mechanical speed [rad/sec]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_init_speed_set(st_extobs_t * p_st_extobs, float f4_speed_rad);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_inertia_set
* Description   : Sets the inertia
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_inertia_kgms2 - The motor inertia [kgms2] to set
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_inertia_set(st_extobs_t * p_st_extobs, float f4_inertia_kgms2);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_current_limit_set
* Description   : Sets the inertia
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_nominal_current - Nominal Current
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_current_limit_set(st_extobs_t * p_st_extobs, float f4_nominal_current);

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_disturbance_current_limit
* Description   : Disturbance current limit
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_speed_rad     - The rotor speed [rad/s]
*                 f4_disturb_current - The distrubance current [A], ASR feed forward
* Return Value  : limited distrubance current [A]
***********************************************************************************************************************/
float motor_speed_extobserver_disturbance_current_limit(st_extobs_t * p_st_extobs, float f4_speed_rad, float f4_disturb_current);

#endif /* R_MOTOR_SPEED_EXTOBSERVER_H */
