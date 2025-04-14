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
* File Name   : r_pfc_manager.c
* Description : The processes of PFC control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <math.h>
#include <stdint.h>
/* Main associated header file */

/* Project headers */
#include "r_pfc_common.h"
#include "r_motor_driver_fsp.h"
#include "r_pfc_manager.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : pfc_manager_default_init
* Description   : Initializes PFC drive modules with default configuration
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_default_init(st_pfc_manager_t *p_st_pfc_manager)
{
    uint16_t u2_array_num;
    st_pfc_driver_cfg_t st_pfc_driver_cfg;

    /* Initialize member of PFC parameter structure */
    p_st_pfc_manager->u1_pfc_ctrl_enable_flg    = PFC_FLG_CLR;
    p_st_pfc_manager->u1_relay_ctrl_on_flg      = PFC_FLG_CLR;
    p_st_pfc_manager->u1_error_cancel_flg       = PFC_FLG_CLR;
    p_st_pfc_manager->u1_volt_dip_flg           = PFC_FLG_CLR;
    p_st_pfc_manager->u2_error_status           = PFC_MANAGER_ERROR_NONE;
    p_st_pfc_manager->u2_run_mode               = PFC_MODE_IDLE;
    p_st_pfc_manager->f4_vac_ad                 = 0.0f;
    p_st_pfc_manager->f4_vdc_ad                 = 0.0f;
    p_st_pfc_manager->f4_ichp_ad                = 0.0f;
    p_st_pfc_manager->f4_vac_ad_lpf             = 0.0f;
    p_st_pfc_manager->f4_vdc_ad_lpf             = 0.0f;
    p_st_pfc_manager->f4_ref_vdc_ctrl           = 0.0f;
    p_st_pfc_manager->f4_vdc_up_step            = VDC_UP_STEP_RATE;
    p_st_pfc_manager->f4_target_vdc             = VDC_TARGET_VALUE;
    p_st_pfc_manager->f4_ac_overvoltage_limit   = VAC_OVER_VOLTAGE;
    p_st_pfc_manager->f4_bus_overvoltage_limit  = VDC_OVER_VOLTAGE;
    p_st_pfc_manager->f4_bus_undervoltage_limit = VDC_UNDER_VOLTAGE;
    p_st_pfc_manager->f4_overcurrent_limit      = ICHP_OVER_CURRENT;
    p_st_pfc_manager->u1_overheat_detect_level  = PFC_OVERHEAT_DETECT_LEVEL;

    /* Initialize LPF */
    pfc_filter_first_order_lpff_init(&p_st_pfc_manager->st_vac_ad_lpf);
    pfc_filter_first_order_lpff_init(&p_st_pfc_manager->st_vdc_ad_lpf);
    pfc_filter_first_order_lpff_gain_calc(&p_st_pfc_manager->st_vac_ad_lpf, VAC_LPF_CUT_FREQ, PFC_MCU_CFG_SAMP_PERIOD);
    pfc_filter_first_order_lpff_gain_calc(&p_st_pfc_manager->st_vdc_ad_lpf, VDC_LPF_CUT_FREQ, PFC_MCU_CFG_SAMP_PERIOD);

    /* Notch filter */
    pfc_filter_notch_init(&p_st_pfc_manager->st_vdc_notch_fil);
    pfc_filter_notch_coef_calc(&p_st_pfc_manager->st_vdc_notch_fil,
                               VDC_NOTCH_FILTER_CUT_FREQ,
                               PFC_MCU_CFG_SAMP_PERIOD,
                               VDC_NOTCH_FILTER_D,
                               VDC_NOTCH_FILTER_ZETA);

    /* PFC control output */
    p_st_pfc_manager->st_pfc_ctrl_output.f4_duty = 0.0f;

    /* Relay control */
    p_st_pfc_manager->st_ac_fil.u2_data_num  = 0U;
    p_st_pfc_manager->st_ac_fil.f4_data_sum  = 0.0f;
    p_st_pfc_manager->st_ac_fil.f4_data_ave  = 0.0f;
    p_st_pfc_manager->st_ac_fil.f4_data_peak = 0.0f;
    p_st_pfc_manager->st_ac_fil.f4_data_rms  = 0.0f;
    for (u2_array_num=0; u2_array_num<=(DATA_ARR_SIZE - 1); u2_array_num++)
    {
        p_st_pfc_manager->st_ac_fil.f4_data_array[u2_array_num] = 0.0f;
    }

    /* Relay control */
    p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt        = 0U;
    p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt       = 0U;
    p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt_limit  = (uint16_t)RELAY_ON_DELAY_CNT;
    p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt_limit = (uint16_t)RELAY_OFF_DELAY_CNT;
    p_st_pfc_manager->st_relay_ctrl.f4_relay_on_vac_rms_min      = RELAY_ON_VAC_RMS_MIN;
    p_st_pfc_manager->st_relay_ctrl.f4_relay_off_vac_rms_min     = RELAY_OFF_VAC_RMS_MIN;
    p_st_pfc_manager->st_relay_ctrl.f4_relay_on_div_min          = RELAY_ON_DIV_MIN;
    p_st_pfc_manager->st_relay_ctrl.f4_relay_off_div_max         = RELAY_OFF_DIV_MAX;

    /* PFC driver */
    st_pfc_driver_cfg.ADCDataGet           = PFC_DRIVER_CFG_FUNC_ADC_DATA_GET;
    st_pfc_driver_cfg.DutySet              = PFC_DRIVER_CFG_FUNC_DUTY_SET;
    st_pfc_driver_cfg.PWMOutputStart       = PFC_DRIVER_CFG_FUNC_PWM_OUTPUT_START;
    st_pfc_driver_cfg.PWMOutputStop        = PFC_DRIVER_CFG_FUNC_PWM_OUTPUT_STOP;
    st_pfc_driver_cfg.RelayOn              = PFC_DRIVER_CFG_FUNC_RELAY_ON;
    st_pfc_driver_cfg.RelayOff             = PFC_DRIVER_CFG_FUNC_RELAY_OFF;
    st_pfc_driver_cfg.OverheatCheck        = PFC_DRIVER_CFG_FUNC_OVERHEAT_CHECK;
    st_pfc_driver_cfg.f4_shunt_ohm         = PFC_CFG_SHUNT_RESIST;
    st_pfc_driver_cfg.f4_ac_volt_gain      = PFC_CFG_AC_VOLTAGE_GAIN;
    st_pfc_driver_cfg.f4_bus_volt_gain     = PFC_CFG_BUS_VOLTAGE_GAIN;
    st_pfc_driver_cfg.f4_crnt_amp_gain     = PFC_CFG_CURRENT_AMP_GAIN;
    st_pfc_driver_cfg.f4_pwm_period_cnt    = PFC_DRIVER_PWM_PEAK_CNT;
    st_pfc_driver_cfg.s2_input_volt_offset = PFC_CFG_INPUT_VOLTAGE_OFFSET;
    st_pfc_driver_cfg.s2_bus_volt_offset   = PFC_CFG_BUS_VOLTAGE_OFFSET;
    st_pfc_driver_cfg.s2_pfc_crnt_offset   = PFC_CFG_CURRENT_OFFSET;
    st_pfc_driver_cfg.u2_pwm_valid_max_cnt = PFC_DRIVER_PWM_VALID_MAX_CNT;
    st_pfc_driver_cfg.u2_pwm_valid_min_cnt = PFC_DRIVER_PWM_VALID_MIN_CNT;
    R_PFC_DRIVER_ParameterUpdate(p_st_pfc_manager->p_st_pfc_driver, &st_pfc_driver_cfg);
} /* End of function pfc_manager_default_init */

/**********************************************************************************************************************
* Function Name : pfc_manager_reset
* Description   : Resets PFC drive modules, configurations will not be reset
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_reset(st_pfc_manager_t *p_st_pfc_manager)
{
    p_st_pfc_manager->st_pfc_ctrl_output.f4_duty = PFC_DUTY_MIN;

    /* PWM reference setting */
    R_PFC_DRIVER_DutySet(p_st_pfc_manager->p_st_pfc_driver, PFC_DUTY_MIN);

    /* Reset PFC control module */
    R_PFC_CTRL_Reset(p_st_pfc_manager->p_st_pfc_ctrl);
} /* End of function pfc_manager_reset */

/***********************************************************************************************************************
* Function Name : pfc_manager_error_process
* Description   : Sets error flags when u2_error_flag is not zero
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
*                 u2_error_flag    - The error flags of the errors to report
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_error_process(st_pfc_manager_t *p_st_pfc_manager, uint16_t u2_error_flag)
{
    if (PFC_MANAGER_ERROR_NONE != u2_error_flag)
    {
        p_st_pfc_manager->u2_error_status |= u2_error_flag;
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function pfc_manager_error_process */

/***********************************************************************************************************************
* Function Name : pfc_manager_fil_vac_calc
* Description   : Calculate the input voltage effective value and peak value
* Arguments     : p_st_ac_fil - The pointer to the AC rms Data structure
*                 f4_vac      - The A/D conversion value of the input voltage
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_fil_vac_calc(st_ac_fil_t *p_st_ac_fil, float f4_vac)
{
    float f4_input_vac;
    f4_input_vac = f4_vac;

    if (DATA_ARR_SIZE <= p_st_ac_fil->u2_data_num)
    {
        p_st_ac_fil->u2_data_num = 0U;
    }

    p_st_ac_fil->f4_data_sum -= p_st_ac_fil->f4_data_array[p_st_ac_fil->u2_data_num];
    p_st_ac_fil->f4_data_array[p_st_ac_fil->u2_data_num] = f4_input_vac;
    p_st_ac_fil->f4_data_sum += p_st_ac_fil->f4_data_array[p_st_ac_fil->u2_data_num];
    p_st_ac_fil->u2_data_num++;

    /* average value */
    p_st_ac_fil->f4_data_ave  = p_st_ac_fil->f4_data_sum / (float)DATA_ARR_SIZE;
    /* peak value */
    p_st_ac_fil->f4_data_peak = p_st_ac_fil->f4_data_ave * PFC_HALF_PI;
    /* rms value */
    p_st_ac_fil->f4_data_rms  = p_st_ac_fil->f4_data_peak / PFC_SQRT_2;
} /* End of function pfc_manager_fil_vac_calc */

/**********************************************************************************************************************
* Function Name : pfc_manager_relay_ctrl
* Description   : Relay control
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_relay_ctrl(st_pfc_manager_t *p_st_pfc_manager)
{
    float f4_volt_div = 0.0f;

    f4_volt_div = p_st_pfc_manager->st_ac_fil.f4_data_peak - p_st_pfc_manager->f4_vdc_ad;

    /*===========================*/
    /*     Relay OFF Process     */
    /*===========================*/
    if (PFC_FLG_SET == p_st_pfc_manager->u1_relay_ctrl_on_flg)
    {
        if (f4_volt_div > p_st_pfc_manager->st_relay_ctrl.f4_relay_off_div_max)
        {
            p_st_pfc_manager->u1_relay_ctrl_on_flg = PFC_FLG_CLR;
            R_PFC_DRIVER_RelayOff(p_st_pfc_manager->p_st_pfc_driver);
        }
        else
        {
            /* Do nothing */
        }

        if (p_st_pfc_manager->st_ac_fil.f4_data_rms < p_st_pfc_manager->st_relay_ctrl.f4_relay_off_vac_rms_min)
        {
            p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt++;

            if (p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt >= p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt_limit)
            {
                p_st_pfc_manager->u1_relay_ctrl_on_flg = PFC_FLG_CLR;
                R_PFC_DRIVER_RelayOff(p_st_pfc_manager->p_st_pfc_driver);
                p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt = 0U;
            }
        }
        else
        {
            p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt = 0U;
        }
    }
    /*===========================*/
    /*     Relay ON Process     */
    /*===========================*/
    else if (PFC_FLG_CLR == p_st_pfc_manager->u1_relay_ctrl_on_flg)
    {
        if ((f4_volt_div < p_st_pfc_manager->st_relay_ctrl.f4_relay_on_div_min)
             && (p_st_pfc_manager->st_ac_fil.f4_data_rms > p_st_pfc_manager->st_relay_ctrl.f4_relay_on_vac_rms_min))
        {
            p_st_pfc_manager->u1_relay_ctrl_on_flg = PFC_FLG_SET;
            R_PFC_DRIVER_RelayOn(p_st_pfc_manager->p_st_pfc_driver);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
} /* End of function pfc_manager_relay_ctrl */

/***********************************************************************************************************************
* Function Name : pfc_manager_reference_vdc_set
* Description   : Updates the bus voltage reference
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : Bus voltage reference
***********************************************************************************************************************/
float pfc_manager_reference_vdc_set(st_pfc_manager_t *p_st_pfc_manager)
{
    float f4_vdc_ref_calc;

    if ((PFC_FLG_SET == p_st_pfc_manager->u1_pfc_ctrl_enable_flg) && (PFC_FLG_CLR == p_st_pfc_manager->u1_volt_dip_flg))
    {
        /* Vdc ramping */
        f4_vdc_ref_calc = p_st_pfc_manager->f4_ref_vdc_ctrl + p_st_pfc_manager->f4_vdc_up_step;
        if (f4_vdc_ref_calc > p_st_pfc_manager->f4_target_vdc)
        {
            f4_vdc_ref_calc = p_st_pfc_manager->f4_target_vdc;
        }
    }
    else
    {
        /* Soft start */
        f4_vdc_ref_calc = p_st_pfc_manager->f4_vdc_ad_lpf;
    }

    /* return Vdc reference */
    return (f4_vdc_ref_calc);
} /* End of function pfc_manager_reference_vdc_set */

/**********************************************************************************************************************
* Function Name : pfc_manager_state_machine_process
* Description   : Performs state transition and execute corresponding action when specified event happen
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_state_machine_process(st_pfc_manager_t *p_st_pfc_manager)
{
    if (PFC_MANAGER_ERROR_NONE != p_st_pfc_manager->u2_error_status)
    {
        p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt = 0U;
        p_st_pfc_manager->u1_pfc_ctrl_enable_flg = PFC_FLG_CLR;
        p_st_pfc_manager->u2_run_mode = PFC_MODE_PFC_ERROR;
    }
    else if (PFC_FLG_CLR == p_st_pfc_manager->u1_relay_ctrl_on_flg)
    {
        p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt = 0U;
        p_st_pfc_manager->st_relay_ctrl.u2_relay_off_delay_cnt = 0U;
        p_st_pfc_manager->u1_pfc_ctrl_enable_flg = PFC_FLG_CLR;
        p_st_pfc_manager->u2_run_mode = PFC_MODE_IDLE;
    }
    else
    {
        /* Do nothing */
    }

    switch (p_st_pfc_manager->u2_run_mode)
    {
        case PFC_MODE_IDLE:
            if (PFC_FLG_SET == p_st_pfc_manager->u1_relay_ctrl_on_flg)
            {
                p_st_pfc_manager->u2_run_mode = PFC_MODE_RELAY_READY;
            }
        break;

        case PFC_MODE_RELAY_READY:
            p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt++;
            if (p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt >= p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt_limit)
            {
                p_st_pfc_manager->st_relay_ctrl.u2_relay_on_delay_cnt = 0U;
                p_st_pfc_manager->u1_pfc_ctrl_enable_flg = PFC_FLG_SET;
                p_st_pfc_manager->u2_run_mode = PFC_MODE_RAMP_UP;
            }
        break;

        case PFC_MODE_RAMP_UP:
            if (p_st_pfc_manager->f4_ref_vdc_ctrl >= p_st_pfc_manager->f4_target_vdc)
            {
                p_st_pfc_manager->u2_run_mode = PFC_MODE_PFC_ON;
            }
        break;

        case PFC_MODE_PFC_ON:
            /* Do Nothing */
        break;

        case PFC_MODE_PFC_ERROR:
            if (PFC_FLG_SET == p_st_pfc_manager->u1_error_cancel_flg)
            {
                p_st_pfc_manager->u2_error_status = PFC_MANAGER_ERROR_NONE;
                p_st_pfc_manager->u1_error_cancel_flg = PFC_FLG_CLR;

                if (PFC_FLG_SET == p_st_pfc_manager->u1_relay_ctrl_on_flg)
                {
                    p_st_pfc_manager->u2_run_mode = PFC_MODE_RAMP_UP;
                    p_st_pfc_manager->u1_pfc_ctrl_enable_flg = PFC_FLG_SET;
                }
                else
                {
                    p_st_pfc_manager->u2_run_mode = PFC_MODE_IDLE;
                }
            }
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function pfc_manager_state_machine_process */

/***********************************************************************************************************************
* Function Name : pfc_manager_voltage_dip_detection
* Description   : Detect the input voltage dip
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_voltage_dip_detection(st_pfc_manager_t *p_st_pfc_manager)
{
    /* Voltage dip detection */
    if (p_st_pfc_manager->st_ac_fil.f4_data_rms < PFC_VAC_DIP_DETECT_LEVEL)
    {
        p_st_pfc_manager->u1_volt_dip_flg = PFC_FLG_SET;
    }
    else
    {
        p_st_pfc_manager->u1_volt_dip_flg = PFC_FLG_CLR;
    }
} /* End of function pfc_manager_voltage_dip_detection */
