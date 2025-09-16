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
* File Name   : r_motor_sensorless_vector_api.h
* Description : Definitions of accessing driver processes
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SENSORLESS_VECTOR_API_H
#define R_MOTOR_SENSORLESS_VECTOR_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_module_cfg.h"
#include "r_motor_sensorless_vector_statemachine.h"
#include "r_motor_current_api.h"
#include "r_motor_speed_api.h"
#include "r_motor_driver.h"
#include "r_motor_sensorless_vector_flyingstart.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Control loop mode */
#define     MOTOR_LOOP_POSITION          (0)
#define     MOTOR_LOOP_SPEED             (1)

/* Error status */
#define     MOTOR_SENSORLESS_VECTOR_ERROR_NONE              (0x0000)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_OVER_CURRENT_HW   (0x0001)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_OVER_VOLTAGE      (0x0002)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_OVER_SPEED        (0x0004)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_HALL_TIMEOUT      (0x0008)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_BEMF_TIMEOUT      (0x0010)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_HALL_PATTERN      (0x0020)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_BEMF_PATTERN      (0x0040)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_LOW_VOLTAGE       (0x0080)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_OVER_CURRENT_SW   (0x0100)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_STALL_DETECTED    (0x0200)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_PFC               (0x0400)

#define     MOTOR_SENSORLESS_VECTOR_ERROR_FAIL_POLES        (0x0800)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_FAIL_POSITION     (0x1000)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_UNKNOWN           (0xffff)

/* max count of Sensorless mode changing cycle  */
#define     CURRENT_SENSORLESS_CHGARGCNT_TOSLOW             (1)
#define     CURRENT_SENSORLESS_CHGARGCNT_TOHIGH             (2)


/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/* Control types */
typedef enum
{
    MOTOR_CTRL_TYPE_POS             = 0,
    MOTOR_CTRL_TYPE_SPEED           = 1,
} e_mtr_ctrl_type_t;

/***********************************************************************************************************************
* Global structure / type definitions
***********************************************************************************************************************/
typedef struct
{
    uint8_t       u1_flag_flying_start_use;     /* flying start control use flag */
    uint8_t       u1_state_speed_ref;           /* The speed command status */
    uint8_t       u1_state_estmode;             /* The estimation mode status */
    uint16_t      u2_estmode_state_chg_cnt;     /* State changing counter */
    uint8_t       u1_direction;                 /* Reference rotation direction */
    uint8_t       u1_ctrl_loop_mode;            /* control loop select */
    uint16_t      u2_error_status;              /* FOC error flags */
    uint16_t      u2_run_mode;                  /* Run mode */
    float         f4_vdc_ad;                    /* Bus voltage [V] */
    float         f4_pfc_vdc_ad;                /* Bus voltage [V] from PFC module */
    float         f4_iu_ad;                     /* U-phase current value [A] */
    float         f4_iv_ad;                     /* V-phase current value [A] */
    float         f4_iw_ad;                     /* W-phase current value [A] */
    float         f4_iu_ref_ad;
    float         f4_iv_ref_ad;
    float         f4_iw_ref_ad;
    float         f4_overcurrent_limit;         /* Over-current limit [A] */
    float         f4_overvoltage_limit;         /* Over-voltage limit [V] */
    float         f4_undervoltage_limit;        /* Under voltage limit [V]*/
    float         f4_overspeed_limit_rad;       /* motor speed limit [rad/s] */
    uint16_t      u2_est_timeout_cnt;           /* est timeout value (counter) */
    float         f4_phase_err_rad_lpf;         /* LPF value of phase error [rad] */
    uint8_t       u1_relay_first_on;            /* PFC Relay ON flag at boot */

    st_1st_order_lpf_t      st_phase_err_lpf;   /* Phase error LPF structure */
    st_current_output_t     st_current_output;  /* Current control output parameter structure */
    st_speed_output_t       st_speed_output;    /* Speed control output parameter structure */

    /* Sub-modules */
    st_statemachine_t       st_stm;             /* Action structure */
    st_motor_parameter_t    st_motor;           /* Motor parameters structure */
    st_flying_start_t       st_flystart;        /* Flying start structure */

    /* Other-modules */
    st_motor_driver_t       * p_st_driver;      /* Motor driver structure */
    st_current_control_t    * p_st_cc;          /* Current control structure */
    st_speed_control_t      * p_st_sc;          /* Speed control structure */
} st_sensorless_vector_control_t;

typedef struct
{
    uint8_t                 u1_flag_flying_start_use;
    uint16_t                u2_off_time_cnt;
    float                   f4_overspeed_limit_rpm;
    float                   f4_phase_err_lpf_cut_freq;
    float                   f4_ctrl_period;
    float                   f4_restart_speed;
    float                   f4_off_time;
    float                   f4_over_time;
    float                   f4_active_brake_time;
    float                   f4_on_current_th;
    st_motor_parameter_t    st_motor;
} st_sensorless_vector_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_sensorless_vector_control_t g_st_sensorless_vector;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Open(void);
void R_MOTOR_SENSORLESS_VECTOR_Close(void);
void R_MOTOR_SENSORLESS_VECTOR_Reset(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate(st_sensorless_vector_control_t * p_st_sensorless_vector,
                                               st_sensorless_vector_cfg_t * p_cfg);
void R_MOTOR_SENSORLESS_VECTOR_MotorStart(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_MotorStop(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_MotorReset(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_ErrorSet(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_SpeedSet(st_sensorless_vector_control_t * p_st_sensorless_vector, float f4_ref_speed);
float R_MOTOR_SENSORLESS_VECTOR_SpeedGet(st_sensorless_vector_control_t * p_st_sensorless_vector);
uint8_t R_MOTOR_SENSORLESS_VECTOR_StatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);
uint16_t R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet(st_sensorless_vector_control_t * p_st_sensorless_vector, uint8_t ctrl_type);
uint8_t R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);
void R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);

#endif /* R_MOTOR_SENSORLESS_VECTOR_API_H */
