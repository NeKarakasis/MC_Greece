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
* File Name   : r_motor_sensorless_vector_flyingstart.h
* Description : Definitions for Flying start
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_SENSORLESS_VECTOR_FLYINGSTART_H
#define R_MOTOR_SENSORLESS_VECTOR_FLYINGSTART_H

/***********************************************************************************************************************
* Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Control parameter */
#define FLY_START_DEFAULT_CURRENT_TH                (2.0f)      /* Current threshold of Short-circuit ON [A] */
#define FLY_START_DEFAULT_OVER_TIME_SEC             (0.5f)      /* Detection excess time [s] */
#define FLY_START_DEFAULT_OFF_TIME_SEC              (0.002f)    /* Short-circuit OFF time [s] */
#define FLY_START_DEFAULT_ACTIVE_BRAKE_TIME_SEC     (1.0f)      /* Active braking time [s] */
#define FLY_START_DEFAULT_RESTART_SPEED_LIMIT       (660.0f)    /* Restart judgment speed reference value [r/min] */

/* Flying start state */
#define FLY_STATE_IDLE                      (0U)
#define FLY_STATE_FIRST_ON                  (1U)
#define FLY_STATE_FIRST_OFF                 (2U)
#define FLY_STATE_SECOND_ON                 (3U)
#define FLY_STATE_SECOND_OFF                (4U)
#define FLY_STATE_ACTIVE_BRAKE              (5U)
#define FLY_STATE_COMPLETE                  (6U)
#define FLY_STATE_CTRL_START                (7U)

/* Flying start action */
#define FLY_ACTION_IDLE                     (0U)
#define FLY_ACTION_SET_DUTY                 (1U)
#define FLY_ACTION_PWM_OUTPUT_ENABLE        (2U)
#define FLY_ACTION_PWM_OUTPUT_DISABLE       (3U)
#define FLY_ACTION_CTRL_PARAM_SET           (4U)
#define FLY_ACTION_NORMAL_START             (5U)

/***********************************************************************************************************************
* Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    float f4_iu_ad;                                     /* U-phase current value [A] */
    float f4_iv_ad;                                     /* V-phase current value [A] */
    float f4_iw_ad;                                     /* W-phase current value [A] */

    float f4_ia_ad;                                     /* a-phase current value [A] */
    float f4_ib_ad;                                     /* b-phase current value [A] */
    float f4_current_mag;                               /* The magnitude of the current vector */

    float f4_ia_ad1st;                                  /* a-phase current value [A] after first on */
    float f4_ib_ad1st;                                  /* b-phase current value [A] after first on  */

    float f4_ia_ad2nd;                                  /* a-phase current value [A] after second on */
    float f4_ib_ad2nd;                                  /* b-phase current value [A] after second on */

    float f4_id_ad;                                     /* The d-axis current value [A] */
    float f4_iq_ad;                                     /* The q-axis current value [A] */

    float f4_angle1st;                                  /* The phase of vector current after first on */
    float f4_angle2nd;                                  /* The phase of vector current after second on */

    float f4_omega_e_ini;                               /* Speed value(electric angle) [rad/s] */
    float f4_omega_m_ini;                               /* Speed value(mechanical angle) [rad/s] */

    float f4_angle_dq;                                  /* The current phase on the dq-axes */
    float f4_angle_e_ini;                               /* The phase of rotor (electric) [rad] after second on */
    float f4_angle_rotate;                              /* The phase of rotor (electric) [rad] when starting control */

    float f4_emf_volt;                                  /* The EMF-Voltage */
    float f4_restart_speed;                             /* Restart judgment speed reference value [rad/s] */

    float f4_ctrl_period;                               /* The control period */
    float f4_on_time;                                   /* Short-circuit ON time [s] */
    float f4_off_time;                                  /* Short-circuit OFF time [s] */
    float f4_on_current_th;                             /* Current threshold of Short-circuit ON [A] */

    uint16_t u2_time_cnt;                               /* The timer counts (cycles) */
    uint16_t u2_on_time_cnt;                            /* Short-circuit ON time (converted into cycles) */
    uint16_t u2_off_time_cnt;                           /* Short-circuit OFF time (converted into cycles) */
    uint16_t u2_over_time_cnt;                          /* Detection excess time (cycle conversion) */
    uint16_t u2_active_brake_time_cnt;                  /* Active braking time (converted into cycles) */
    uint8_t  u1_state;                                  /* The state of flying-start */
    uint8_t  u1_action;                                 /* The action of flying-start */

    st_motor_parameter_t * p_st_motor;                  /* Motor parameter structure */
} st_flying_start_t;

typedef struct
{
    uint16_t u2_off_time_cnt;                           /* Short-circuit OFF time (converted into cycles) */
    uint16_t u2_over_time_cnt;                          /* Detection excess time (cycle conversion) */
    uint16_t u2_active_brake_time_cnt;                  /* Active braking time (converted into cycles) */
    float    f4_ctrl_period;                            /* The control period */
    float    f4_restart_speed;                          /* Restart judgment speed reference value [rad/s] */
    float    f4_off_time;                               /* Short-circuit OFF time [s] */
    float    f4_on_current_th;                          /* Current threshold of Short-circuit ON [A] */
    st_motor_parameter_t * p_st_motor;                  /* Motor parameter structure */
} st_flying_start_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_flying_start_t g_st_flying_start;

/***********************************************************************************************************************
* Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void motor_flying_start_init(st_flying_start_t *p_st_flystart);
void motor_flying_start_reset(st_flying_start_t *p_st_flystart);
void motor_flying_start_parameter_set(st_flying_start_t *p_st_flystart, const st_flying_start_cfg_t *p_st_flystart_cfg);
void motor_flying_start_main(st_flying_start_t *p_st_flystart);

#endif /* R_MOTOR_SENSORLESS_VECTOR_FLYINGSTART_H */
