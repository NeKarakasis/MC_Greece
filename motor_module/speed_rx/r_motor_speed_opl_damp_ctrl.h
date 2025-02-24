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
* File Name   : r_motor_speed_opl_damp_ctrl.h
* Description : Definitions of the open-loop damping control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_OPL_DAMP_CTRL_H
#define R_MOTOR_SPEED_OPL_DAMP_CTRL_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_filter.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    st_1st_order_lpf_t  st_ed_lpf;                  /* Ed LPF structure */
    float               f4_damp_comp_gain;          /* The gain for open-loop damping */
    float               f4_fb_speed_limit_rate;     /* The limit of speed feed-back rate */
} st_opl_damp_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_ctrl
* Description   : Open-loop damping control
* Arguments     : p_st_opl_damp - damping control structure (pointer)
*                 f4_ed - d-axis BEMF
*                 f4_speed_ref - reference speed
* Return Value  : Feedback value for reference speed
***********************************************************************************************************************/
float motor_speed_opl_damp_ctrl(st_opl_damp_t * p_st_opl_damp, float f4_ed, float f4_speed_ref);

/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_init
* Description   : Initializes open-loop damping control
* Arguments     : p_st_opl_damp - Open-loop damping control structure (pointer)
*                 f4_fb_speed_limit_rate -  Rate of reference speed for feedback speed limiter
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_opl_damp_init(st_opl_damp_t * p_st_opl_damp, float f4_fb_speed_limit_rate);

/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_reset
* Description   : Resets open-loop damping control (except for gains and limiters)
* Arguments     : p_st_opl_damp - Open-loop damping control structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_opl_damp_reset(st_opl_damp_t * p_st_opl_damp);

/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_gain_set
* Description   : Sets open-loop damping control gains
* Arguments     : p_st_opl_damp - Open-loop damping control structure (pointer)
*                 f4_damp_gain - d-axis BEMF feedback gain
*                 f4_omega_t - (ed HPF cutoff frequency) * (control period)
*                 f4_gain_ka - ed LPF gain Ka
*                 f4_gain_kb - ed LPF gain Kb
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_opl_damp_gain_set(st_opl_damp_t * p_st_opl_damp,
                                   float f4_damp_gain,
                                   float f4_omega_t,
                                   float f4_gain_ka,
                                   float f4_gain_kb);

/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_r_gain_set
* Description   : Designs and sets open-loop damping control gains
* Arguments     : p_st_opl_damp - Open-loop damping control structure (pointer)
*                 u2_pp - Pole pairs
*                 f4_ke - Magnetic flux [Wb]
*                 f4_j - Inertia [kgm^2]
*                 f4_zeta - Damping ratio of open-loop damping control
*                 f4_ed_hpf_fc - Cutoff frequency of HPF for ed [Hz]
*                 f4_opl_current - The current (Id reference)[A] in open-loop
*                 f4_id_down_speed - Sensor-less control switch speed [rad/s]
*                 f4_tc - Control period [s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_opl_damp_r_gain_set(st_opl_damp_t * p_st_opl_damp,
                                     uint16_t u2_pp,
                                     float f4_ke,
                                     float f4_j,
                                     float f4_zeta,
                                     float f4_ed_hpf_fc,
                                     float f4_opl_current,
                                     float f4_id_down_speed,
                                     float f4_tc);

/***********************************************************************************************************************
* Function Name : motor_speed_opl_damp_limit_set
* Description   : Sets open-loop damping control limiters
* Arguments     : p_st_opl_damp - Open-loop damping control structure (pointer)
*                 f4_limit_rate - Rate of reference speed for feedback speed limiter
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_opl_damp_limit_set(st_opl_damp_t * p_st_opl_damp, float f4_limit_rate);

#endif /* R_MOTOR_SPEED_OPL_DAMP_CTRL_H */
