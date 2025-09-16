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
* File Name   : r_system_manager_api.c
* Description : The processes of accessing motor and PFC modules
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_system_manager_api.h"


/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_Open
* Description   : Open system manager module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_Open(void)
{
    /* PFC status initialize */
//    g_u2_pfc_status_pre = PFC_MODE_IDLE;

    /* Create motor manager */
    R_MOTOR_SENSORLESS_VECTOR_Open();

    /* Create PFC manager */
  //  R_PFC_MANAGER_Open();
} /* End of function R_SYSTEM_MANAGER_Open */

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_MotorStatusGet
* Description   : Get motor manager state
* Arguments     : None
* Return Value  : motor status
***********************************************************************************************************************/
uint8_t R_SYSTEM_MANAGER_MotorStatusGet(void)
{
    uint8_t u1_motor_status;

    /* Get Motor manager state */
    u1_motor_status = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);

    return (u1_motor_status);
} /* End of function R_SYSTEM_MANAGER_MotorStatusGet */



/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_SystemMode
* Description   : Process of system mode command
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_SystemMode(void)
{
    uint8_t u1_temp;

    /*============================*/
    /*        Execute event       */
    /*============================*/
    u1_temp = com_u1_system_mode;

    if (g_u1_system_mode != u1_temp)
    {
        if (u1_temp >= STATEMACHINE_EVENT_SIZE)
        {
            com_u1_system_mode = g_u1_system_mode;
        }
        else
        {
            g_u1_system_mode = u1_temp;
            switch (g_u1_system_mode)
            {
                case STATEMACHINE_EVENT_STOP:
                    R_MOTOR_SENSORLESS_VECTOR_MotorStop(&g_st_sensorless_vector);
                break;

                case STATEMACHINE_EVENT_RUN:
                    R_MOTOR_SENSORLESS_VECTOR_MotorStart(&g_st_sensorless_vector);
                break;

                case STATEMACHINE_EVENT_RESET:
                    R_MOTOR_SENSORLESS_VECTOR_MotorReset(&g_st_sensorless_vector);
                break;

                default:
                    /* Do nothing */
                break;
            }
        }
    }
} /* End of function R_SYSTEM_MANAGER_SystemMode */

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_ParameterUpdate
* Description   : Apply parameters in buffer through API
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_ParameterUpdate(void)
{
     st_rmw_param_buffer_t *p_st_rmw_input = &g_st_rmw_input_buffer;

     st_current_cfg_t           st_cur_cfg;
     st_bemf_observer_cfg_t     st_bemf_obs_cfg;
     st_ext_observer_cfg_t      st_ext_obs_cfg;
     st_speed_cfg_t             st_spd_cfg;
     st_sensorless_vector_cfg_t st_sensorless_vector_cfg;

     /* FOC configuration set */
     st_sensorless_vector_cfg.u1_flag_flying_start_use     = p_st_rmw_input->u1_flag_flying_start_use;
     st_sensorless_vector_cfg.f4_overspeed_limit_rpm       = p_st_rmw_input->f4_overspeed_limit_rpm;
     st_sensorless_vector_cfg.f4_ctrl_period               = SPEED_CFG_CTRL_PERIOD;
     st_sensorless_vector_cfg.st_motor                     = p_st_rmw_input->st_motor;
     st_sensorless_vector_cfg.f4_restart_speed             = p_st_rmw_input->f4_restart_speed;
     st_sensorless_vector_cfg.f4_off_time                  = p_st_rmw_input->f4_off_time;
     st_sensorless_vector_cfg.f4_over_time                 = p_st_rmw_input->f4_over_time;
     st_sensorless_vector_cfg.f4_active_brake_time         = p_st_rmw_input->f4_active_brake_time;
     st_sensorless_vector_cfg.f4_on_current_th             = p_st_rmw_input->f4_on_current_th;
     R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate(&g_st_sensorless_vector, &st_sensorless_vector_cfg);

     /* Commands */
     R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet(&g_st_sensorless_vector, p_st_rmw_input->u1_ctrl_loop_mode);
     R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, p_st_rmw_input->f4_ref_speed_rpm);

     /* Current configuration set */
     st_cur_cfg.u1_flag_volt_err_comp_use      = p_st_rmw_input->u1_flag_volt_err_comp_use;
     st_cur_cfg.u2_offset_calc_time            = p_st_rmw_input->u2_offset_calc_time;
     st_cur_cfg.u1_flag_stall_detection_use    = p_st_rmw_input->u1_flag_stall_detection_use;
     st_cur_cfg.u1_flag_trq_vibration_comp_use = p_st_rmw_input->u1_flag_trq_vibration_comp_use;
     st_cur_cfg.f4_ctrl_period                 = MOTOR_COMMON_CTRL_PERIOD;
     st_cur_cfg.f4_current_omega_hz            = p_st_rmw_input->f4_current_omega_hz;
     st_cur_cfg.f4_current_zeta                = p_st_rmw_input->f4_current_zeta;
     st_cur_cfg.st_motor                       = p_st_rmw_input->st_motor;
     st_cur_cfg.f4_timelead                    = p_st_rmw_input->f4_timelead;
     st_cur_cfg.f4_id_hpf_time                 = p_st_rmw_input->f4_id_hpf_time;
     st_cur_cfg.f4_iq_hpf_time                 = p_st_rmw_input->f4_iq_hpf_time;
     st_cur_cfg.f4_threshold_level             = p_st_rmw_input->f4_threshold_level;
     st_cur_cfg.f4_threshold_time              = p_st_rmw_input->f4_threshold_time;
     st_cur_cfg.u1_flag_trqvib_comp_learning   = p_st_rmw_input->u1_flag_trqvib_comp_learning;
     st_cur_cfg.f4_tf_lpf_time                 = p_st_rmw_input->f4_tf_lpf_time;
     st_cur_cfg.f4_output_gain                 = p_st_rmw_input->f4_output_gain;
     st_cur_cfg.f4_input_weight2               = p_st_rmw_input->f4_input_weight2;
     st_cur_cfg.f4_input_weight1               = p_st_rmw_input->f4_input_weight1;
     st_cur_cfg.f4_input_weight0               = p_st_rmw_input->f4_input_weight0;
     /* High/low speed switching speed threshold */
     /* low speed sensorless */
     st_cur_cfg.f4_highspd_threshold           = p_st_rmw_input->f4_highspd_threshold;
     st_cur_cfg.f4_lowspd_threshold            = p_st_rmw_input->f4_lowspd_threshold;
     R_MOTOR_CURRENT_ParameterUpdate(&g_st_cc, &st_cur_cfg);

     /* BEMF observer */
     st_bemf_obs_cfg.f4_e_obs_omega_hz         = p_st_rmw_input->f4_e_obs_omega_hz;
     st_bemf_obs_cfg.f4_e_obs_zeta             = p_st_rmw_input->f4_e_obs_zeta;
     st_bemf_obs_cfg.f4_pll_est_omega_hz       = p_st_rmw_input->f4_pll_est_omega_hz;
     st_bemf_obs_cfg.f4_pll_est_zeta           = p_st_rmw_input->f4_pll_est_zeta;
     st_bemf_obs_cfg.f4_pll_estlow_omega_hz    = p_st_rmw_input->f4_pll_estlow_omega_hz;/* low speed sensorless */
     st_bemf_obs_cfg.f4_pll_estlow_zeta        = p_st_rmw_input->f4_pll_estlow_zeta;/* low speed sensorless */
     R_MOTOR_CURRENT_BEMFObserverParameterUpdate(&g_st_cc, &st_bemf_obs_cfg);

     /* Speed configuration set */
     st_spd_cfg.u1_flag_extobserver_use = p_st_rmw_input->u1_flag_extobserver_use;/* low speed sensorless */
     st_spd_cfg.u1_flag_fluxwkn_use            = p_st_rmw_input->u1_flag_fluxwkn_use;
     st_spd_cfg.u1_flag_mtpa_use               = p_st_rmw_input->u1_flag_mtpa_use;
     st_spd_cfg.f4_max_speed_rpm               = p_st_rmw_input->f4_max_speed_rpm;
     st_spd_cfg.f4_speed_ctrl_period           = SPEED_CFG_CTRL_PERIOD;
     st_spd_cfg.f4_speed_rate_limit_rpm        = p_st_rmw_input->f4_speed_rate_limit_rpm;
     st_spd_cfg.f4_speed_omega_hz              = p_st_rmw_input->f4_speed_omega_hz;
     st_spd_cfg.f4_speed_zeta                  = p_st_rmw_input->f4_speed_zeta;
     st_spd_cfg.f4_speed_lpf_hz                = p_st_rmw_input->f4_speed_lpf_hz;
     st_spd_cfg.st_motor                       = p_st_rmw_input->st_motor;
     R_MOTOR_SPEED_ParameterUpdate(&g_st_sc, &st_spd_cfg);

     /* extended observer */
     st_ext_obs_cfg.f4_extobs_omega = p_st_rmw_input->f4_extobs_omega;
     R_MOTOR_SPEED_ExtObserverParameterUpdate(&g_st_sc, &st_ext_obs_cfg);

} /* End of function R_SYSTEM_MANAGER_UpdateParameter */

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_UpdateCommand
* Description   : Update commands without copying to buffer or update trigger
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_UpdateCommand(void)
{
    if (MOTOR_LOOP_SPEED == g_st_sensorless_vector.u1_ctrl_loop_mode)
    {
        R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, com_f4_ref_speed_rpm);
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function R_SYSTEM_MANAGER_UpdateCommand */

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_MotorCurrentInterrupt
* Description   : The carrier interrupt handler of FOC control (motor manager : 8kHz)
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_MotorCurrentInterrupt(void)
{
    R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt(&g_st_sensorless_vector);
} /* End of function R_SYSTEM_MANAGER_MotorCurrentInterrupt */



/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_MotorSpeedInterrupt
* Description   : 500us interrupt handler of FOC control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_MotorSpeedInterrupt(void)
{
    R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt(&g_st_sensorless_vector);
} /* End of function R_SYSTEM_MANAGER_MotorSpeedInterrupt */

/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_ControlInterrupt
* Description   : System manager periodic interrupt handling
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_ControlInterrupt(void)
{
    /* PFC error check and error set */
  //  system_manager_pfc_error_process();

    /* PFC Relay check */
  //  system_manager_pfc_relay_status_judgment();
} /* End of function R_SYSTEM_MANAGER_ControlInterrupt */


/***********************************************************************************************************************
* Function Name : R_SYSTEM_MANAGER_MotorOverCurrentInterrupt
* Description   : Inverter board H/W over current interrupt
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_SYSTEM_MANAGER_MotorOverCurrentInterrupt(void)
{
    R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt(&g_st_sensorless_vector);
} /* End of function R_SYSTEM_MANAGER_MotorOverCurrentInterrupt */
