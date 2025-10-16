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
* File Name   : r_app_board_ui.c
* Description : Processes of user interface on inverter board
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_app_board_ui.h"
#include "r_app_board_ui_ctrl.h"
#include "r_motor_sensorless_vector_api.h"

/***********************************************************************************************************************
* Private & global variables and functions
***********************************************************************************************************************/
static uint8_t  s_u1_sw_cnt        = 0;         /* Counter to remove chattering */
static uint8_t  s_u1_reset_req     = 0;         /* Reset request flag */
static float    s_f4_ref_speed_rpm = 0.0f;      /* speed reference */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_mainloop
* Description   : User interface using board UI
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_mainloop(void)
{
    uint8_t  u1_temp;
    uint8_t  u1_motor_status;
    uint8_t  u1_temp_sw_signal;
    uint16_t u2_temp_vr1_signal;

    /*======================*/
    /*     Mode control     */
    /*======================*/
    u1_motor_status = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);
                                                                        /* Get status of motor control system */

    switch (u1_motor_status)
    {
        case STATEMACHINE_STATE_STOP:

            u1_temp_sw_signal = 1;//r_app_board_ui_get_sw1();
            if (MTR_FLG_SET == r_app_board_ui_remove_sw_chattering(u1_temp_sw_signal, BOARD_SW_ON)) /* Check SW1 */
            {
                R_MOTOR_SENSORLESS_VECTOR_MotorStart(&g_st_sensorless_vector); /* Execute active event */
            }
        break;

        case STATEMACHINE_STATE_RUN:

            u1_temp_sw_signal = 0;//r_app_board_ui_get_sw1();
            if (MTR_FLG_SET == r_app_board_ui_remove_sw_chattering(u1_temp_sw_signal, BOARD_SW_OFF)) /* Check SW1 */
            {
                R_MOTOR_SENSORLESS_VECTOR_MotorStop(&g_st_sensorless_vector); /* Execute stop event */
            }
            u1_temp = R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet(&g_st_sensorless_vector);
            if (u1_temp == MOTOR_LOOP_SPEED)
            {
                /*=============================*/
                /*     Set speed reference     */
                /*=============================*/
                u2_temp_vr1_signal = 0;//r_app_board_ui_get_vr1();
                s_f4_ref_speed_rpm = (BOARD_ADJUST_OFFSET - u2_temp_vr1_signal) * BOARD_VR1_SCALING_SPEED(0);
                                                                                   /* read speed reference from VR1 */
                s_f4_ref_speed_rpm = motor_filter_limitf_abs(s_f4_ref_speed_rpm, MOTOR_CFG_MAX_SPEED_RPM(0));
                /* limit dead-band */
                if ((s_f4_ref_speed_rpm <= BOARD_VR1_SPEED_DEAD_BAND) &&
                    (s_f4_ref_speed_rpm >= - BOARD_VR1_SPEED_DEAD_BAND))
                {
                    s_f4_ref_speed_rpm = 0.0f;
                }
                R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, s_f4_ref_speed_rpm);
                                                                                /* set speed reference */
            }
            else
            {
                /* Do nothing */
            }
        break;

        case STATEMACHINE_STATE_ERROR:

            /* check SW2 & reset request flag */
            u1_temp_sw_signal = 0;//r_app_board_ui_get_sw2();
            if ((BOARD_SW_OFF == s_u1_reset_req) &&
                (MTR_FLG_SET == r_app_board_ui_remove_sw_chattering(u1_temp_sw_signal, BOARD_SW_ON)))
            {
                s_u1_reset_req = BOARD_SW_ON;
            }
            else if ((BOARD_SW_ON == s_u1_reset_req) &&
                     (MTR_FLG_SET == r_app_board_ui_remove_sw_chattering(u1_temp_sw_signal, BOARD_SW_OFF)))
            {
                s_u1_reset_req = BOARD_SW_OFF;
                R_MOTOR_SENSORLESS_VECTOR_MotorReset(&g_st_sensorless_vector); /* Execute reset event */
            }
            else
            {
                /* Do nothing */
            }
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function r_app_board_ui_mainloop */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led_control
* Description   : Set LED pattern depend on motor status
* Arguments     : u1_motor_status - Motor control status
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led_control(uint8_t u1_motor_status)
{
    /***** LED control *****/
    if (STATEMACHINE_STATE_STOP == u1_motor_status)         /* Check motor status */
    {
        //r_app_board_ui_led1_off();                          /* LED1 off */
        //r_app_board_ui_led2_off();                          /* LED2 off */
        //r_app_board_ui_led3_off();                          /* LED3 off */
    }
    else if (STATEMACHINE_STATE_RUN == u1_motor_status)     /* Check motor status */
    {
        //r_app_board_ui_led1_on();                           /* LED1 on */
        //r_app_board_ui_led2_off();                          /* LED2 off */
    }
    else if (STATEMACHINE_STATE_ERROR == u1_motor_status)   /* Check motor status */
    {
        //r_app_board_ui_led1_off();                          /* LED1 off */
        //r_app_board_ui_led2_on();                           /* LED2 on */
        //r_app_board_ui_led3_off();                          /* LED3 off */
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function r_app_board_ui_led_control */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_remove_sw_chattering
* Description   : Get switch status with removing chattering
* Arguments     : u1_sw - Board interface switch signal
*                 u1_on_off - Detected status (ON/OFF)
* Return Value  : u1_remove_chattering_flg - Detection result
***********************************************************************************************************************/
uint8_t r_app_board_ui_remove_sw_chattering(uint8_t u1_sw, uint8_t u1_on_off)
{
    uint8_t u1_remove_chattering_flg;

    u1_remove_chattering_flg = 0;
    if (u1_on_off == u1_sw)
    {
        s_u1_sw_cnt++;
        if (BOARD_CHATTERING_CNT < s_u1_sw_cnt)
        {
            u1_remove_chattering_flg = MTR_FLG_SET;
            s_u1_sw_cnt = 0;
        }
    }
    else
    {
        s_u1_sw_cnt = 0;
    }

    return (u1_remove_chattering_flg);
} /* End of function r_app_board_ui_remove_sw_chattering */
