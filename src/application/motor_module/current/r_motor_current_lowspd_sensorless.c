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
* File Name    : r_motor_current_lowspd_sensorless.c
* Description  : This module convert voltages to duties
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 20.12.2022 1.00    First Release
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <stdint.h>
#include <math.h>

/* Project headers */
#include "r_motor_current_lowspd_sensorless.h"

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_init
* Description  : Initializes lowspeed sensorless module
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                f4_ld_pulse           - D-axis inductance
*                f4_lq_pulse           - Q-axis inductance
*                f4_volt_puls_inj      - Pulse injection frequency 1/f at the boot mode
*                f4_ctrl_period        - Control Period
*                u1_puls_inj_freq_boot - Pulse injection current pattern
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_init(st_lowspd_sensorless_t *p_st_lowspd,
                                         float f4_ld_pulse,
                                         float f4_lq_pulse,
                                         float f4_volt_puls_inj,
                                         float f4_ctrl_period,
                                         uint8_t u1_puls_inj_freq_boot)
{
    if (p_st_lowspd)
    {
        for( int i = 0; i < MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE; i++)
        {
            p_st_lowspd->f4_array_id_p[i] = 0.0f;
            p_st_lowspd->f4_array_id_n[i] = 0.0f;
            p_st_lowspd->f4_array_iq_p[i] = 0.0f;
            p_st_lowspd->f4_array_iq_n[i] = 0.0f;
        }

        p_st_lowspd->f4_ctrl_period = f4_ctrl_period;

        p_st_lowspd->f4_ld_pulse = f4_ld_pulse;
        p_st_lowspd->f4_lq_pulse = f4_lq_pulse;

        p_st_lowspd->f4_volt_pulse_inj = f4_volt_puls_inj;
        p_st_lowspd->u1_pulse_cnt = 0;

        if( u1_puls_inj_freq_boot <= MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE)
        {
            p_st_lowspd->u1_pulse_freq = u1_puls_inj_freq_boot; /* Pulse continuous generation count */
                                                                /* (1 unit is PWM cycle, 1d/50us) */
        }
        else
        {
            p_st_lowspd->u1_pulse_freq = MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE;
                                                                /* Pulse continuous generation count max set */
        }

        p_st_lowspd->u1_pulse_cnt = 0;
        p_st_lowspd->u1_pulse_sign = 0;

        p_st_lowspd->f4_delt_id = 0.0f;
        p_st_lowspd->f4_delt_iq = 0.0f;

        p_st_lowspd->f4_id_peak_p = 0.0f;
        p_st_lowspd->f4_id_peak_n = 0.0f;

        p_st_lowspd->f4_delta_ang = 0.0f;

        p_st_lowspd->u2_ang_est_count = 0;
        p_st_lowspd->f4_div_ang = 0.0f;
        p_st_lowspd->f4_last_ang = 0.0f;

        p_st_lowspd->u1_cur_chg_cnt = 0;
        p_st_lowspd->u1_div_ang_cnt = 0;

        p_st_lowspd->u1_pf_calculated = 0;
        p_st_lowspd->u1_posest_calculated = 0;
        p_st_lowspd->f4_pf_last_idc_plus = 0.0f;
        p_st_lowspd->f4_pf_last_idc_minus = 0.0f;
        p_st_lowspd->f4_PF = 0.0f;
        p_st_lowspd->u1_PF_start_cnt = 0;

        p_st_lowspd->f4_highspd_threshold = MOTOR_SENSORLESS_VECTOR_THRESHOLD_HIGHSPEED;
        p_st_lowspd->f4_lowspd_threshold = MOTOR_SENSORLESS_VECTOR_THRESHOLD_LOWSPEED;
    }
} /* End of function motor_current_lowspdsensorless_init() */

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_dtheta
* Description  : calcurate delta theta
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_dtheta(st_lowspd_sensorless_t *p_st_lowspd)
{
    float f4_ld = p_st_lowspd->f4_ld_pulse;
    float f4_lq = p_st_lowspd->f4_lq_pulse;

    /* Calculate delta theta  */
    float f4_delt_cur = ((f4_ld * f4_lq)/((f4_lq - f4_ld) * p_st_lowspd->f4_volt_pulse_inj *
                          p_st_lowspd->f4_ctrl_period ));
    
    p_st_lowspd->f4_delta_ang = f4_delt_cur * p_st_lowspd->f4_delt_iq;
} /* End of function motor_current_lowspdsensorless_calc_dtheta() */

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_save_current
* Description  : save current data
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_id - d-axis detected current
*                 f4_iq - q-axis detected current
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_save_current(st_lowspd_sensorless_t *p_st_lowspd, float f4_id, float f4_iq)
{
    uint8_t  u1_pulse_no = (uint8_t)(p_st_lowspd->u1_pulse_sign * 10 + p_st_lowspd->u1_pulse_cnt);

    switch(u1_pulse_no)
    {
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        p_st_lowspd->f4_array_id_p[p_st_lowspd->u1_pulse_cnt] = f4_id;
        p_st_lowspd->f4_array_iq_p[p_st_lowspd->u1_pulse_cnt] = f4_iq;
        break;

    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        p_st_lowspd->f4_array_id_n[p_st_lowspd->u1_pulse_cnt] = f4_id;
        p_st_lowspd->f4_array_iq_n[p_st_lowspd->u1_pulse_cnt] = f4_iq;
        break;
    }
} /* End of function motor_current_lowspdsensorless_save_current() */


/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_current
* Description  : calcurate current avarage and delta id / iq
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_vd_ref - d-axis voltage reference
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_current(st_lowspd_sensorless_t *p_st_lowspd, float f4_vd_ref)
{
    float f4_id_sum_p = 0.0f;
    float f4_id_sum_n = 0.0f;
    float f4_iq_sum_p = 0.0f;
    float f4_iq_sum_n = 0.0f;

    float f4_id_peak_p = 0.0f;
    float f4_id_peak_n = 0.0f;
    float f4_iq_peak_p = 0.0f;
    float f4_iq_peak_n = 0.0f;

    uint8_t u1_div = p_st_lowspd->u1_pulse_freq;

    /* Calculate each Id/Iq sum for averaging */
    for( int i = 0; i < p_st_lowspd->u1_pulse_freq; i++)
    {
        f4_id_sum_p += p_st_lowspd->f4_array_id_p[i];
        f4_id_sum_n += p_st_lowspd->f4_array_id_n[i];
        f4_iq_sum_p += p_st_lowspd->f4_array_iq_p[i];
        f4_iq_sum_n += p_st_lowspd->f4_array_iq_n[i];
    }

    /* Store current peak to peak point */
    f4_id_peak_p = p_st_lowspd->f4_array_id_p[0];
    f4_id_peak_n = p_st_lowspd->f4_array_id_n[0];
    f4_iq_peak_p = p_st_lowspd->f4_array_iq_p[0];
    f4_iq_peak_n = p_st_lowspd->f4_array_iq_n[0];

    /* Avoid zero div */
    if(u1_div == 0)
    {
        u1_div = 1.0f;
    }

    /* calculates average id and iq*/
    p_st_lowspd->f4_id_ad_ave = (f4_id_sum_p + f4_id_sum_n) / u1_div / 2.0f;
    p_st_lowspd->f4_iq_ad_ave = (f4_iq_sum_p + f4_iq_sum_n) / u1_div / 2.0f;


    /* Calculation of delta Id and Iq */
    p_st_lowspd->f4_id_peak_p = f4_id_peak_p;
    p_st_lowspd->f4_id_peak_n = f4_id_peak_n;

    p_st_lowspd->f4_delt_id = 0.5f * (f4_id_peak_p - f4_id_peak_n);
    p_st_lowspd->f4_delt_iq = 0.5f * (f4_iq_peak_p - f4_iq_peak_n);
} /* End of function motor_current_lowspdsensorless_calc_current() */


/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_vd_pulse_injection
* Description  : calcurate delta theta
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_vd_ref - d-axis voltage reference
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_vd_pulse_injection(st_lowspd_sensorless_t *p_st_lowspd, float f4_vd_ref)
{
    float f4_pulse_volt = 0;

    /* Calculate delta theta  */
    p_st_lowspd->u1_pulse_cnt++;

    if( p_st_lowspd->u1_pulse_cnt >= p_st_lowspd->u1_pulse_freq)
    {
        p_st_lowspd->u1_pulse_cnt = 0;

        /* Invert pulse porality  */
        if( p_st_lowspd->u1_pulse_sign == 0)
        {
            /* 0--> 1 */
            p_st_lowspd->u1_pulse_sign = 1;
        }
        else
        {
            /* 1 --> 0 */
            p_st_lowspd->u1_pulse_sign = 0;
        }

        /*calculate current ripple and average */
        motor_current_lowspdsensorless_calc_current(p_st_lowspd, f4_vd_ref);

        /* Calculate porality detection of motor magnetic pole */
        motor_current_lowspdsensorless_calc_pf(p_st_lowspd);
    }
    else
    {
        /* Nothing to do */
    }

    if( p_st_lowspd->u1_pulse_sign == 0)
    {
    	f4_pulse_volt = -1.0f * p_st_lowspd->f4_volt_pulse_inj;
    }
    else
    {
    	f4_pulse_volt = 1.0f * p_st_lowspd->f4_volt_pulse_inj;
    }
    p_st_lowspd->f4_vd_ref_pulse_inj = f4_pulse_volt;
} /* End of function motor_current_lowspdsensorless_vd_pulse_injection() */

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_pf
* Description  : calcurate delta pf
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_pf(st_lowspd_sensorless_t *p_st_lowspd)
{
    float f4_delta_pf = 0;

    /* Skip calacute. Already PF calculated. */
    if(p_st_lowspd->u1_pf_calculated == 1)
    {
        return;
    }
    
    /* Skip calculate when id peak p/n is not stored. */

    if( (p_st_lowspd->f4_id_peak_p == 0.0f) || (p_st_lowspd->f4_id_peak_n == 0.0f))
    {
	    return;
    }
    
    /* Skip calculate first pulses */
    
    if( p_st_lowspd->u1_PF_start_cnt <= MOTOR_SENSORLESS_VECTOR_PF_START_CNT)
    {
	    p_st_lowspd->u1_PF_start_cnt++;
	    return;
    }
    
    

    /* Calculate delta PF  */
    /* Store Idc+ / Idc-. */
    if( p_st_lowspd->f4_id_peak_n < 0.0f)
    {
        p_st_lowspd->f4_pf_last_idc_plus = p_st_lowspd->f4_id_peak_p;
        p_st_lowspd->f4_pf_last_idc_minus = p_st_lowspd->f4_id_peak_n;
    }
    else
    {
        p_st_lowspd->f4_pf_last_idc_plus = p_st_lowspd->f4_id_peak_n;
        p_st_lowspd->f4_pf_last_idc_minus = p_st_lowspd->f4_id_peak_p;
    }

    /* delta PF = |idc_amp_1| - |idc_amp_0| */
    f4_delta_pf = fabsf(p_st_lowspd->f4_pf_last_idc_minus) - fabsf(p_st_lowspd->f4_pf_last_idc_plus);

    /* Integrate PF  */
    p_st_lowspd->f4_PF += f4_delta_pf;

    /* PF Limitter */
    if( p_st_lowspd->f4_PF > 65535.0f)
    {
        p_st_lowspd->f4_PF = 65535.0f;
    }
    else if( p_st_lowspd->f4_PF < -65535.0f)
    {
        p_st_lowspd->f4_PF = -65535.0f;
    }
} /* End of function motor_current_lowspdsensorless_calc_pf() */
