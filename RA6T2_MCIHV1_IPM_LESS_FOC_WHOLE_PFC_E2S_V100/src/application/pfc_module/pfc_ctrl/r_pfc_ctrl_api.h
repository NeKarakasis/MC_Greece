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
* File Name   : r_pfc_ctrl_api.h
* Description : Definitions of PFC control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_PFC_CTRL_API_H
#define R_PFC_CTRL_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_pfc_pi_control.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    float             f4_vac_ad;                    /* AC voltage [V] */
    float             f4_vdc_ad;                    /* Bus voltage [V] */
    float             f4_ichp_ad;                   /* Current value [A] */
    float             f4_vac_rms;                   /* Effective value of AC voltage [V] */
    float             f4_ref_vdc_ctrl;              /* The reference Vdc value [V] */
} st_pfc_ctrl_input_t;

typedef struct
{
    float             f4_duty;                      /* PWM duty */
} st_pfc_ctrl_output_t;

/* PFC control parameter structure */
typedef struct
{
    float             f4_vac_ad;                    /* AC voltage [V] */
    float             f4_vdc_ad;                    /* Bus voltage [V] */
    float             f4_ichp_ad;                   /* Current value [A] */
    float             f4_vac_rms;                   /* Effective value of AC voltage [V] */
    float             f4_ref_vdc_ctrl;              /* The reference Vdc value [V] */
    float             f4_ref_ichp_ctrl;             /* The reference Ichp value [A] */
    float             f4_power_max_limit;           /* Maximum limit value of power [W] */
    float             f4_power_min_limit;           /* Minimum limit value of power [W] */
    float             f4_power_slope;               /* The slope value of power [W/V] */
    float             f4_avr_ff_comp_min_limit;     /* Minimum limit value of Vrms for AVR FF compensation [V] */
    float             f4_acr_ff_comp_min_limit;     /* Minimum limit value of Vdc for ACR FF compensation [V] */
    uint16_t          u2_duty_ff_softstart_cnt;     /* The number of PWM valid maximum counts */
    uint16_t          u2_duty_ff_softstart_max_cnt; /* The maximum counts for soft-start for duty FF compensation */
    float             f4_duty_ff_softstart_up_step; /* Duty FF soft start adding value */
    float             f4_duty_ff_comp;              /* Duty FF compensation value */
    float             f4_duty_ff_comp_coef;         /* Compensation coefficient for duty FF compensation */
    float             f4_duty;                      /* Duty ratio */
    float             f4_duty_max;                  /* Maximum limit value of duty ratio */
    float             f4_duty_min;                  /* Minimum limit value of duty ratio */
    float             f4_avr_pi_output;             /* PI control output for AVR */
    float             f4_acr_pi_output;             /* PI control output for ACR */
    st_pfc_pi_ctrl_t  st_pi_voltage;                /* PI control structure for AVR */
    st_pfc_pi_ctrl_t  st_pi_current;                /* PI control structure for ACR */
} st_pfc_ctrl_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_pfc_ctrl_t g_st_pfc_ctrl;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_PFC_CTRL_Open(void);
void R_PFC_CTRL_Close(void);
void R_PFC_CTRL_Reset(st_pfc_ctrl_t *p_st_pfc_ctrl);
void R_PFC_CTRL_ParameterSet(st_pfc_ctrl_t *p_st_pfc_ctrl, const st_pfc_ctrl_input_t * p_st_pfc_ctrl_input);
void R_PFC_CTRL_ParameterGet(st_pfc_ctrl_t *p_st_pfc_ctrl, st_pfc_ctrl_output_t *p_st_pfc_ctrl_output);
void R_PFC_CTRL_Cyclic(st_pfc_ctrl_t *p_st_pfc_ctrl);

#endif /* R_PFC_CTRL_API_H */
