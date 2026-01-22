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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_sensorless_vector_api.c
* Description : The processes of accessing driver
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <math.h>
#include <r_motor_sensorless_vector_api.h>

/* Main associated header file */
#include "r_motor_sensorless_vector_manager.h"

/* Module headers */
/* Submodule headers */
#include "r_motor_sensorless_vector_protection.h"

/* General library headers */
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
st_sensorless_vector_control_t g_st_sensorless_vector;

extern st_speed_control_t      g_st_sc;

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Open
* Description   : Open Sensorless FOC Control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Open(void)
{
    R_MOTOR_CURRENT_Open();
    R_MOTOR_SPEED_Open();
    R_MOTOR_DRIVER_Open();

    /* Store the address of the module instance in a structure and connect the module to be used */
    g_st_sensorless_vector.p_st_cc             = &g_st_cc;
    g_st_sensorless_vector.p_st_sc             = &g_st_sc;
    g_st_sensorless_vector.p_st_driver         = &g_st_driver;

    g_st_sensorless_vector.u1_relay_first_on   = MTR_FLG_CLR;
    g_st_sensorless_vector.f4_pfc_vdc_ad       = 0.0f;

    motor_sensorless_vector_statemachine_init(&g_st_sensorless_vector.st_stm);
    motor_flying_start_init(&g_st_sensorless_vector.st_flystart);
    motor_sensorless_vector_default_init(&g_st_sensorless_vector);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_Open */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Close
* Description   : Close Sensorless FOC Control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Close(void)
{
    R_MOTOR_CURRENT_Close();
    R_MOTOR_SPEED_Close();
    R_MOTOR_DRIVER_Close();
    motor_sensorless_vector_reset(&g_st_sensorless_vector);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_Close */

/**********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Reset
* Description   : Resets motor drive modules, configurations will not be reset
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Reset(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_reset(p_st_sensorless_vector);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_Reset */

/**********************************************************************************************************************
* Function Name: R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate
* Description  : Update parameter for Sensorless Vector Control
* Arguments    : p_st_sensorless_vector - The pointer to the motor control management data structure
*              : p_cfg                  - The pointer to the motor control management config structure
* Return Value : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate(st_sensorless_vector_control_t * p_st_sensorless_vector,
                                               st_sensorless_vector_cfg_t * p_cfg)
{
    st_flying_start_cfg_t st_flystart_cfg;

    if (motor_sensorless_vector_statemachine_status_get(&p_st_sensorless_vector->st_stm) == STATEMACHINE_STATE_RUN)
    {
        return;
    }

    p_st_sensorless_vector->u1_flag_flying_start_use     = p_cfg->u1_flag_flying_start_use;
    p_st_sensorless_vector->f4_overspeed_limit_rad       = p_cfg->f4_overspeed_limit_rpm * MTR_RPM2RAD;
    p_st_sensorless_vector->st_motor                     = p_cfg->st_motor;

    motor_sensorless_vector_nominal_current_set(&g_st_sensorless_vector,
                                                p_st_sensorless_vector->st_motor.f4_nominal_current_rms);

    motor_sensorless_phase_err_lpf_setup(p_st_sensorless_vector,
                                         p_cfg->f4_phase_err_lpf_cut_freq,
                                         p_cfg->f4_ctrl_period);

    st_flystart_cfg.f4_ctrl_period           = MOTOR_COMMON_CTRL_PERIOD;
    st_flystart_cfg.f4_off_time              = p_cfg->f4_off_time;
    st_flystart_cfg.f4_restart_speed         = p_cfg->f4_restart_speed * MTR_RPM2RAD;
    st_flystart_cfg.u2_off_time_cnt          = (uint16_t)(p_cfg->f4_off_time / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.u2_over_time_cnt         = (uint16_t)(p_cfg->f4_over_time / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.u2_active_brake_time_cnt = (uint16_t)(p_cfg->f4_active_brake_time / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.f4_on_current_th         = p_cfg->f4_on_current_th;
    st_flystart_cfg.p_st_motor               = &p_st_sensorless_vector->st_motor;
    motor_flying_start_parameter_set(&p_st_sensorless_vector->st_flystart, &st_flystart_cfg);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorStart
* Description   : Start to drive motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorStart(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_statemachine_event(&p_st_sensorless_vector->st_stm,
                                               p_st_sensorless_vector,
                                               STATEMACHINE_EVENT_RUN);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_MotorStart */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorStop
* Description   : Stop driving motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorStop(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_statemachine_event(&p_st_sensorless_vector->st_stm,
                                               p_st_sensorless_vector,
                                               STATEMACHINE_EVENT_STOP);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_MotorStop */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorReset
* Description   : Cancel error
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorReset(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_statemachine_event(&p_st_sensorless_vector->st_stm,
                                               p_st_sensorless_vector,
                                               STATEMACHINE_EVENT_RESET);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_MotorReset */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_ErrorSet
* Description   : Set error
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_ErrorSet(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_statemachine_event(&p_st_sensorless_vector->st_stm,
                                               p_st_sensorless_vector,
                                               STATEMACHINE_EVENT_ERROR);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_ErrorSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedSet
* Description   : Set speed reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 f4_ref_speed - speed reference [rpm]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_SpeedSet(st_sensorless_vector_control_t *p_st_sensorless_vector, float f4_ref_speed)
{
    R_MOTOR_SPEED_SpdRefSet(p_st_sensorless_vector->p_st_sc, f4_ref_speed);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_SpeedSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedGet
* Description   : Get speed value
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : speed
***********************************************************************************************************************/
float R_MOTOR_SENSORLESS_VECTOR_SpeedGet(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    float f4_speed_rpm;

    f4_speed_rpm = (p_st_sensorless_vector->st_speed_output.f4_speed_rad_lpf * MTR_RAD2RPM);

    return (f4_speed_rpm);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_SpeedGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_StatusGet
* Description   : Get status of motor control system
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : motor status
***********************************************************************************************************************/
uint8_t R_MOTOR_SENSORLESS_VECTOR_StatusGet(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    uint8_t u1_status;

    u1_status = motor_sensorless_vector_statemachine_status_get(&p_st_sensorless_vector->st_stm);

    return (u1_status);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_StatusGet */

/***********************************************************************************************************************
* Function Name: R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet
* Description  : Get current error status
* Arguments    : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value : Error status
***********************************************************************************************************************/
uint16_t R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector)
{
    uint16_t u2_error_status;

    u2_error_status = p_st_sensorless_vector->u2_error_status;

    return (u2_error_status);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet */

/***********************************************************************************************************************
* Function Name: R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet
* Description  : Set control type
* Arguments    : p_st_sensorless_vector - The pointer to the motor control management data structure
*                ctrl_type - The control type that defined in e_mtr_ctrl_type_t
* Return Value : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet(st_sensorless_vector_control_t * p_st_sensorless_vector, uint8_t ctrl_type)
{
    if (motor_sensorless_vector_statemachine_status_get(&p_st_sensorless_vector->st_stm) != STATEMACHINE_STATE_RUN)
    {
        switch (ctrl_type)
        {
            case MOTOR_CTRL_TYPE_SPEED:
            {
                p_st_sensorless_vector->u1_ctrl_loop_mode = MOTOR_LOOP_SPEED;
                break;
            }
            default:
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
} /* End of function R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet
* Description   : Get loop mode status
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : control loop mode status
***********************************************************************************************************************/
uint8_t R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    return (p_st_sensorless_vector->u1_ctrl_loop_mode);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt
* Description   : 500us interrupt handler of FOC control
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    st_speed_input_t    st_speed_input;

    if (STATEMACHINE_STATE_RUN == motor_sensorless_vector_statemachine_status_get(&p_st_sensorless_vector->st_stm))
    {
        /* Decide direction */
        if (MTR_FLG_SET == p_st_sensorless_vector->p_st_sc->u1_active)
        {
            motor_sensorless_vector_decide_direction(p_st_sensorless_vector);
            /* Id, Iq, speed, position references setting */
            switch (p_st_sensorless_vector->u2_run_mode)
            {
                case MOTOR_MODE_INIT:
                    if (MTR_FLG_SET == p_st_sensorless_vector->st_current_output.u1_flag_offset_calc)
                    {
                        p_st_sensorless_vector->u2_run_mode = MOTOR_MODE_BOOT;
                    }
                break;

                case MOTOR_MODE_BOOT:
                    st_speed_input.u1_state_speed_ref = p_st_sensorless_vector->u1_state_speed_ref;
                    st_speed_input.f4_speed_rad       = p_st_sensorless_vector->st_current_output.f4_speed_rad;
                    st_speed_input.f4_va_max          = p_st_sensorless_vector->st_current_output.f4_va_max;
                    R_MOTOR_SPEED_ParameterSet(p_st_sensorless_vector->p_st_sc, &st_speed_input);
                    R_MOTOR_SPEED_SpeedCyclic(p_st_sensorless_vector->p_st_sc);
                    R_MOTOR_SPEED_ParameterGet(p_st_sensorless_vector->p_st_sc, &p_st_sensorless_vector->st_speed_output);
                    /* When position estimation complete, go to Drive Mode (low speed sensorless) */
                    if (CURRENT_STATE_ESTMODE_DRIVE_SLOW == p_st_sensorless_vector->u1_state_estmode)
                    {
                        p_st_sensorless_vector->u2_run_mode = MOTOR_MODE_DRIVE;
                    }

                break;

                case MOTOR_MODE_DRIVE:
                    st_speed_input.u1_state_speed_ref = p_st_sensorless_vector->u1_state_speed_ref;
                    st_speed_input.f4_speed_rad       = p_st_sensorless_vector->st_current_output.f4_speed_rad;
                    st_speed_input.f4_va_max          = p_st_sensorless_vector->st_current_output.f4_va_max;
                    R_MOTOR_SPEED_ParameterSet(p_st_sensorless_vector->p_st_sc, &st_speed_input);
                    R_MOTOR_SPEED_SpeedCyclic(p_st_sensorless_vector->p_st_sc);
                    R_MOTOR_SPEED_ParameterGet(p_st_sensorless_vector->p_st_sc, &p_st_sensorless_vector->st_speed_output);
                break;

                default:
                    /* Do Nothing */
                break;
            }
            /* Set references */
            motor_sensorless_vector_speed_status_select(p_st_sensorless_vector);
        }
    }
} /* End of function R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt
* Description   : The carrier interrupt handler of FOC control
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    st_current_input_t st_current_input;

    /* Current, Voltage detection */
    R_MOTOR_DRIVER_BldcAnalogGet(p_st_sensorless_vector->p_st_driver,
          							 p_st_sensorless_vector->st_current_output.f4_modu,
      							     p_st_sensorless_vector->st_current_output.f4_modv,
      							     p_st_sensorless_vector->st_current_output.f4_modw,
                                       &p_st_sensorless_vector->f4_iu_ad,
      								 &p_st_sensorless_vector->f4_iv_ad,
                                       &p_st_sensorless_vector->f4_iw_ad,
                                       &p_st_sensorless_vector->f4_vdc_ad);

    /* Set the VDC value obtained by PFC */
    p_st_sensorless_vector->f4_vdc_ad = p_st_sensorless_vector->f4_pfc_vdc_ad;

    /* current offset adjustment */
       R_MOTOR_CURRENT_CurrentOffsetRemove(p_st_sensorless_vector->p_st_cc,
                                                  &p_st_sensorless_vector->f4_iu_ad,
                                                  &p_st_sensorless_vector->f4_iv_ad,
                                                  &p_st_sensorless_vector->f4_iw_ad);
    /* V-phase current calculation */
   // p_st_sensorless_vector->f4_iv_ad = -(p_st_sensorless_vector->f4_iu_ad + p_st_sensorless_vector->f4_iw_ad);

    /* error check */
    if (MTR_FLG_SET == p_st_sensorless_vector->u1_relay_first_on)
    {
        motor_sensorless_vector_error_check(p_st_sensorless_vector);
    }
    /* set current input */
    st_current_input.f4_iu_ad        = p_st_sensorless_vector->f4_iu_ad;
    st_current_input.f4_iv_ad        = p_st_sensorless_vector->f4_iv_ad;
    st_current_input.f4_iw_ad        = p_st_sensorless_vector->f4_iw_ad;;
    st_current_input.f4_vdc_ad       = p_st_sensorless_vector->f4_vdc_ad;
    st_current_input.f4_iq_ref       = p_st_sensorless_vector->st_speed_output.f4_iq_ref;
    st_current_input.f4_id_ref       = p_st_sensorless_vector->st_speed_output.f4_id_ref;
    st_current_input.f4_ref_speed_rad_ctrl = p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl;
    R_MOTOR_CURRENT_ParameterSet(p_st_sensorless_vector->p_st_cc, &st_current_input);

    if (STATEMACHINE_STATE_RUN == motor_sensorless_vector_statemachine_status_get(&p_st_sensorless_vector->st_stm))
    {
    	/* For Read offset flag */
    	R_MOTOR_CURRENT_ParameterGet(p_st_sensorless_vector->p_st_cc,&p_st_sensorless_vector->st_current_output);
        /* Calculate offset current values */
        if (MTR_FLG_CLR == p_st_sensorless_vector->st_current_output.u1_flag_offset_calc)
        {
            /* set half carrier */
            R_MOTOR_DRIVER_BldcDutySet(p_st_sensorless_vector->p_st_driver, 0.5f, 0.5f, 0.5f);
            R_MOTOR_CURRENT_OffsetCalibration(p_st_sensorless_vector->p_st_cc);
            R_MOTOR_CURRENT_ParameterGet(p_st_sensorless_vector->p_st_cc, &p_st_sensorless_vector->st_current_output);
            if((MTR_DISABLE == p_st_sensorless_vector->u1_flag_flying_start_use)
            		&&(MTR_FLG_SET == p_st_sensorless_vector->st_current_output.u1_flag_offset_calc))
            {
            	R_MOTOR_DRIVER_PWMControlStart(p_st_sensorless_vector->p_st_driver); /* PWM output enable */
            	motor_sensorless_vector_module_activate(p_st_sensorless_vector);
            }
        }
        else
        {
            if (MTR_ENABLE == p_st_sensorless_vector->u1_flag_flying_start_use)
            {
                /* Copy three-phase currents to the flying-start module */
                p_st_sensorless_vector->st_flystart.f4_iu_ad = p_st_sensorless_vector->f4_iu_ad;
                p_st_sensorless_vector->st_flystart.f4_iv_ad = p_st_sensorless_vector->f4_iv_ad;
                p_st_sensorless_vector->st_flystart.f4_iw_ad = p_st_sensorless_vector->f4_iw_ad;

                /* Executes the fying-start main function */
                motor_flying_start_main(&p_st_sensorless_vector->st_flystart);

                switch (p_st_sensorless_vector->st_flystart.u1_action)
                {
                    case FLY_ACTION_SET_DUTY:
                        R_MOTOR_DRIVER_BldcZeroDutySet(p_st_sensorless_vector->p_st_driver);
                    break;

                    case FLY_ACTION_PWM_OUTPUT_ENABLE:
                        R_MOTOR_DRIVER_PWMControlStart(p_st_sensorless_vector->p_st_driver);
                    break;

                    case FLY_ACTION_PWM_OUTPUT_DISABLE:
                        R_MOTOR_DRIVER_PWMControlStop(p_st_sensorless_vector->p_st_driver);
                    break;

                    case FLY_ACTION_CTRL_PARAM_SET:
                        /* Presets the angle and related trigonometric values */
                        R_MOTOR_CURRENT_RotorAngleSet(p_st_sensorless_vector->p_st_cc,
                                                      p_st_sensorless_vector->st_flystart.f4_angle_rotate);
                        /* Presets the PLL integral term */
                        R_MOTOR_CURRENT_PLLSpeedSet(p_st_sensorless_vector->p_st_cc,
                                                    p_st_sensorless_vector->st_flystart.f4_omega_e_ini);
                        /* Presets the initial speed to speed control module */
                        R_MOTOR_SPEED_ControlParamSet(p_st_sensorless_vector->p_st_sc,
                                                      p_st_sensorless_vector->st_flystart.f4_omega_m_ini);
                        /* Presets the Q-axis estimated voltage disturbance */
                        R_MOTOR_CURRENT_BEMFObserverParameterSet(p_st_sensorless_vector->p_st_cc,
                                                                 p_st_sensorless_vector->st_flystart.f4_emf_volt);
                        /* Preset the current control command status */
                        /* Presets the run mode */
                        p_st_sensorless_vector->u2_run_mode = MOTOR_MODE_DRIVE;
                        /* Presets the speed control command status */
                        p_st_sensorless_vector->u1_state_speed_ref = SPEED_STATE_MANUAL;

                        /* low speed sensorless 関数の修正後に対応する (削除？)*/
                        R_MOTOR_SPEED_RefstateSet(p_st_sensorless_vector->p_st_sc,SPEED_STATE_MANUAL);

                        /* low speed sensorless */
                        if(fabsf(p_st_sensorless_vector->p_st_sc->f4_speed_rad_ctrl) > p_st_sensorless_vector->p_st_cc->st_lowspd.f4_highspd_threshold )
                        {
                            p_st_sensorless_vector->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_HIGH;
                            p_st_sensorless_vector->p_st_cc->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_HIGH;
                            p_st_sensorless_vector->p_st_sc->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_HIGH;
                        }
                        else
                        {
                            p_st_sensorless_vector->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_SLOW;
                            p_st_sensorless_vector->p_st_cc->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_SLOW;
                            p_st_sensorless_vector->p_st_sc->u1_state_estmode = CURRENT_STATE_ESTMODE_DRIVE_SLOW;
                        }
                        /* low speed sensorless */
                        /* Decrease the pulse voltage (because the polarity determination is over) */
                        p_st_sensorless_vector->p_st_cc->st_lowspd.f4_volt_pulse_inj = CURRENT_CFG_ESTLOW_PULSEVOLT_RUNNING;

                        if( CURRENT_CFG_ESTLOW_PULSEFREQ_DRIVE <= MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE)
                        {
                          /* HFI frequency sets 1/CURRENT_CFG_ESTLOW_PULSEFREQ_DRIVE */
                          p_st_sensorless_vector->p_st_cc->st_lowspd.u1_pulse_freq = CURRENT_CFG_ESTLOW_PULSEFREQ_DRIVE;
                        }
                        else
                        {
                          /* HFI frequency sets 1/2 */
                          p_st_sensorless_vector->p_st_cc->st_lowspd.u1_pulse_freq = 1;
                        }

                        /* Activates Current and Speed Control Loop */
                        motor_sensorless_vector_module_activate(p_st_sensorless_vector);
                    break;

                    case FLY_ACTION_NORMAL_START:
                        /* Activates Current and Speed Control Loop */
                        motor_sensorless_vector_module_activate(p_st_sensorless_vector);
                    break;

                    default:
                        /* Do Nothing */
                    break;
                }

                if (FLY_STATE_COMPLETE == p_st_sensorless_vector->st_flystart.u1_state)
                {
                    R_MOTOR_DRIVER_BldcCompareDutySet(p_st_sensorless_vector->p_st_driver);

                    R_MOTOR_CURRENT_CurrentCyclic(p_st_sensorless_vector->p_st_cc);
                    R_MOTOR_CURRENT_ParameterGet(p_st_sensorless_vector->p_st_cc, &p_st_sensorless_vector->st_current_output);

                    /* PWM reference setting */
                    R_MOTOR_DRIVER_BldcDutySet(p_st_sensorless_vector->p_st_driver,
                                               p_st_sensorless_vector->st_current_output.f4_modu,
                                               p_st_sensorless_vector->st_current_output.f4_modv,
                                               p_st_sensorless_vector->st_current_output.f4_modw);
                }
            }
            else
            {
                R_MOTOR_CURRENT_CurrentCyclic(p_st_sensorless_vector->p_st_cc);
                R_MOTOR_CURRENT_ParameterGet(p_st_sensorless_vector->p_st_cc, &p_st_sensorless_vector->st_current_output);

                /* PWM reference setting */
                R_MOTOR_DRIVER_BldcDutySet(p_st_sensorless_vector->p_st_driver,
                                           p_st_sensorless_vector->st_current_output.f4_modu,
                                           p_st_sensorless_vector->st_current_output.f4_modv,
                                           p_st_sensorless_vector->st_current_output.f4_modw);
            }
        }
    }
    /* low speed sensorless state transition processing */
    motor_sensorless_vector_estmode_status_select(p_st_sensorless_vector);

} /* End of function R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt
* Description   : POE interrupt
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    motor_sensorless_vector_error_process(p_st_sensorless_vector, MOTOR_SENSORLESS_VECTOR_ERROR_OVER_CURRENT_HW);
} /* End of function R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt */
