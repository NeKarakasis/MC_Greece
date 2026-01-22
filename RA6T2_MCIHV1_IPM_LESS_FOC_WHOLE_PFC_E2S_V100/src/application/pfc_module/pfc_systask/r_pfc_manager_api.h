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
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_pfc_manager_api.h
* Description : Definitions of accessing driver processes
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_PFC_MANAGER_API_H
#define R_PFC_MANAGER_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_pfc_cfg.h"
#include "r_pfc_filter.h"
#include "r_pfc_ctrl_api.h"
#include "r_pfc_driver.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Error status */
#define     PFC_MANAGER_ERROR_NONE              (0x0000)
#define     PFC_MANAGER_ERROR_AC_OVER_VOLTAGE   (0x0001)
#define     PFC_MANAGER_ERROR_BUS_OVER_VOLTAGE  (0x0002)
#define     PFC_MANAGER_ERROR_BUS_LOW_VOLTAGE   (0x0004)
#define     PFC_MANAGER_ERROR_OVER_CURRENT_SW   (0x0008)
#define     PFC_MANAGER_ERROR_OVER_CURRENT_HW   (0x0010)
#define     PFC_MANAGER_ERROR_OVER_HEATING      (0x0020)
#define     PFC_MANAGER_ERROR_UNKNOWN           (0xffff)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global structure / type definitions
***********************************************************************************************************************/
/* AC rms Data structure */
typedef struct
{
    uint16_t u2_data_num;                       /* The number of data */
    float    f4_data_sum;                       /* The integrated value of the input voltage */
    float    f4_data_array[DATA_ARR_SIZE];      /* The array of the input voltage */
    float    f4_data_ave;                       /* The average value of the input voltage */
    float    f4_data_peak;                      /* The peak value of the input voltage */
    float    f4_data_rms;                       /* The effective value of the input voltage */
} st_ac_fil_t;

/* Relay Control Data structure */
typedef struct
{
    uint16_t u2_relay_on_delay_cnt;             /* Waiting time counter for relay ON [count] */
    uint16_t u2_relay_off_delay_cnt;            /* Waiting time counter for relay OFF [count] */
    uint16_t u2_relay_on_delay_cnt_limit;       /* Waiting time maximum count value for relay ON [count] */
    uint16_t u2_relay_off_delay_cnt_limit;      /* Waiting time maximum count value for relay OFF [count] */
    float    f4_relay_on_vac_rms_min;           /* Minimum VAC rms value for relay ON */
    float    f4_relay_off_vac_rms_min;          /* Minimum VAC rms value for relay OFF */
    float    f4_relay_on_div_min;               /* Minimum voltage deviation for relay ON */
    float    f4_relay_off_div_max;              /* Maximum voltage deviation for relay OFF */
} st_relay_ctrl_t;

typedef struct
{
    uint8_t       u1_pfc_ctrl_enable_flg;       /* PFC control ON flag */
    uint8_t       u1_relay_ctrl_on_flg;         /* Relay control ON flag */
    uint8_t       u1_error_cancel_flg;          /* Error cancel flag */
    uint8_t       u1_volt_dip_flg;              /* Input voltage dip flag */
    uint16_t      u2_error_status;              /* PFC error flags */
    uint16_t      u2_run_mode;                  /* Run mode */
    float         f4_vac_ad;                    /* AC voltage [V] */
    float         f4_vdc_ad;                    /* Bus voltage [V] */
    float         f4_ichp_ad;                   /* Current value [A] */
    float         f4_vac_ad_lpf;                /* LPF value of AC voltage[V] */
    float         f4_vdc_ad_lpf;                /* LPF value of Bus voltage[V] */
    float         f4_ref_vdc_ctrl;              /* The reference Vdc value [V] */
    float         f4_vdc_up_step;               /* Reference Vdc adding value [V] */
    float         f4_target_vdc;                /* The target value of bus voltage [V] */
    float         f4_ac_overvoltage_limit;      /* AC over-voltage limit [V] */
    float         f4_bus_overvoltage_limit;     /* Bus over-voltage limit [V] */
    float         f4_bus_undervoltage_limit;    /* Bus under voltage limit [V]*/
    float         f4_overcurrent_limit;         /* Over-current limit [A] */
    uint8_t       u1_overheat_detect_level;     /* Port level when over heating error is detected */

    st_pfc_1st_order_lpf_t  st_vac_ad_lpf;      /* AC voltage LPF structure */
    st_pfc_1st_order_lpf_t  st_vdc_ad_lpf;      /* Bus voltage LPF structure */
    st_pfc_notch_fil_t      st_vdc_notch_fil;   /* Bus voltage notch filter structure */
    st_pfc_ctrl_output_t    st_pfc_ctrl_output; /* PFC control output parameter structure */

    st_ac_fil_t             st_ac_fil;          /* AC rms Data stucture */
    st_relay_ctrl_t         st_relay_ctrl;      /* Relay Control Data structure */

    /* Other-modules */
    st_pfc_driver_t         * p_st_pfc_driver;  /* PFC driver structure */
    st_pfc_ctrl_t           * p_st_pfc_ctrl;    /* PFC control structure */
} st_pfc_manager_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_pfc_manager_t g_st_pfc_manager;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_PFC_MANAGER_Open(void);
void R_PFC_MANAGER_Close(void);
void R_PFC_MANAGER_Reset(st_pfc_manager_t * p_st_pfc_manager);
void R_PFC_MANAGER_ErrorCancel(st_pfc_manager_t * p_st_pfc_manager);
uint16_t R_PFC_MANAGER_StatusGet(st_pfc_manager_t * p_st_pfc_manager);
void R_PFC_MANAGER_Main(st_pfc_manager_t * p_st_pfc_manager);
void R_PFC_MANAGER_OverCurrentInterrupt(st_pfc_manager_t * p_st_pfc_manager);

#endif /* R_PFC_MANAGER_API_H */
