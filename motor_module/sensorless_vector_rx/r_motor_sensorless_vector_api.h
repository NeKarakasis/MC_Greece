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
#define		MOTOR_SENSORLESS_VECTOR_ERROR_APPLICATION		(0x0120)
#define     MOTOR_SENSORLESS_VECTOR_ERROR_UNKNOWN           (0xffff)

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
    uint8_t       u1_flag_less_switch_use;      /* Soft switching between open-loop and vector control use flag */
    uint8_t       u1_flag_openloop_damping_use; /* Open-loop damping control use flag */
    uint8_t       u1_flag_down_to_ol;           /* The open-loop drive flag */
    uint8_t       u1_state_id_ref;              /* The d-axis current command status */
    uint8_t       u1_state_iq_ref;              /* The q-axis current command status */
    uint8_t       u1_state_speed_ref;           /* The speed command status */
    uint8_t       u1_direction;                 /* Reference rotation direction */
    uint8_t       u1_ctrl_loop_mode;            /* control loop select */
    uint16_t      u2_error_status;              /* FOC error flags */
    uint16_t      u2_run_mode;                  /* Run mode */
    float         f4_vdc_ad;                    /* Bus voltage [V] */
    float         f4_iu_ad;                     /* U-phase current value [A] */
    float         f4_iv_ad;                     /* V-phase current value [A] */
    float         f4_iw_ad;                     /* W-phase current value [A] */
    float         f4_overcurrent_limit;         /* Over-current limit [A] */
    float         f4_overvoltage_limit;         /* Over-voltage limit [V] */
    float         f4_undervoltage_limit;        /* Under voltage limit [V]*/
    float         f4_overspeed_limit_rad;       /* motor speed limit [rad/s] */
    float         f4_phase_err_rad_lpf;         /* LPF value of phase error[rad] */
    float         f4_switch_phase_err_rad;      /* Phase error to decide sensor-less switch timing [rad] */
    float         f4_id_down_speed_rad;         /* The speed threshold[rad/s] to ramp down the d-axis current */
    float         f4_id_up_speed_rad;           /* The speed threshold[rad/s] to ramp up the d-axis current */
    float         f4_damp_comp_speed;           /* Feedback value for reference speed [rad/s] */
    float         f4_ol_speed_rad;              /* Speed value in open loop mode [rad/s] */

    st_1st_order_lpf_t      st_phase_err_lpf;   /* Phase error LPF structure */
    st_current_output_t     st_current_output;  /* Current control output parameter structure */
    st_speed_output_t       st_speed_output;    /* Speed control output parameter structure */

    /* Sub-modules */
    st_statemachine_t       st_stm;             /* Action structure */
    st_motor_parameter_t    st_motor;           /* Motor parameters structure */

    /* Other-modules */
    st_motor_driver_t       * p_st_driver;      /* Motor driver structure */
    st_current_control_t    * p_st_cc;          /* Current control structure */
    st_speed_control_t      * p_st_sc;          /* Speed control structure */
} st_sensorless_vector_control_t;

typedef struct
{
    uint8_t                 u1_flag_less_switch_use;
    uint8_t                 u1_flag_openloop_damping_use;
    float                   f4_overspeed_limit_rpm;
    float                   f4_switch_phase_err_deg;
    float                   f4_phase_err_lpf_cut_freq;
    float                   f4_id_down_speed_rpm;
    float                   f4_id_up_speed_rpm;
    float                   f4_ctrl_period;
    st_motor_parameter_t    st_motor;
} st_sensorless_vector_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_sensorless_vector_control_t g_st_sensorless_vector;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Open
* Description   : Open Sensorless FOC Control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Open(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Close
* Description   : Close Sensorless FOC Control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Close(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_Reset
* Description   : Resets motor drive modules, configurations will not be reset
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_Reset(st_sensorless_vector_control_t * p_st_sensorless_vector);

/**********************************************************************************************************************
 * Function Name: R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate
 * Description  : Update parameter for Sensorless Vector Control
 * Arguments    : p_st_sensorless_vector    - The pointer to the motor control management data structure
 *              : p_cfg                  - The pointer to the motor control management config structure
 * Return Value : None
 *********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate(st_sensorless_vector_control_t * p_st_sensorless_vector,
                                               st_sensorless_vector_cfg_t * p_cfg);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorStart
* Description   : Start to drive motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorStart(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorStop
* Description   : Stop driving motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorStop(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_MotorReset
* Description   : Cancel error
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_MotorReset(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_ErrorSet
* Description   : Set error
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_ErrorSet(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedSet
* Description   : Set speed reference
* Arguments     : p_st_sensorless_vector     - The pointer to the motor control management data structure
*                 f4_ref_speed - speed reference [rpm]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_SpeedSet(st_sensorless_vector_control_t * p_st_sensorless_vector, float f4_ref_speed);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedGet
* Description   : Get speed value
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : speed
***********************************************************************************************************************/
float R_MOTOR_SENSORLESS_VECTOR_SpeedGet(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_StatusGet
* Description   : Get status of motor control system
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : motor status
***********************************************************************************************************************/
uint8_t R_MOTOR_SENSORLESS_VECTOR_StatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);

/**********************************************************************************************************************
 * Function Name: R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet
 * Description  : Get current error status
 * Arguments    : p_st_sensorless_vector    - The pointer to the motor control management data structure
 * Return Value : Error status
 *********************************************************************************************************************/
uint16_t R_MOTOR_SENSORLESS_VECTOR_ErrorStatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);

/**********************************************************************************************************************
 * Function Name: R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet
 * Description  : Set control type
 * Arguments    : p_st_sensorless_vector    - The pointer to the motor control management data structure
 *                ctrl_type -
 *                   The control type that defined in e_mtr_ctrl_type_t
 * Return Value : None
 *********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet(st_sensorless_vector_control_t * p_st_sensorless_vector, uint8_t ctrl_type);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet
* Description   : Get loop mode status
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : control loop mode status
***********************************************************************************************************************/
uint8_t R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt
* Description   : 500us interrupt handler of FOC control
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_SpeedInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt
* Description   : The carrier interrupt handler of FOC control
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_CurrentInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);

/***********************************************************************************************************************
* Function Name : R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt
* Description   : POE interrupt handler
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SENSORLESS_VECTOR_OverCurrentInterrupt(st_sensorless_vector_control_t * p_st_sensorless_vector);

#endif /* R_MOTOR_SENSORLESS_VECTOR_API_H */
