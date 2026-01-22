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
* File Name   : r_pfc_manager.h
* Description : Definitions for PFC control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_MANAGER_H
#define R_PFC_MANAGER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_pfc_manager_api.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define     PFC_MODE_IDLE               (0x00)
#define     PFC_MODE_RELAY_READY        (0x01)
#define     PFC_MODE_RAMP_UP            (0x02)
#define     PFC_MODE_PFC_ON             (0x03)
#define     PFC_MODE_PFC_ERROR          (0x04)

/***********************************************************************************************************************
* Macro definitions for sequence control
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global structure / type definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void pfc_manager_default_init(st_pfc_manager_t *p_st_pfc_manager);
void pfc_manager_reset(st_pfc_manager_t *p_st_pfc_manager);
void pfc_manager_error_process(st_pfc_manager_t *p_st_pfc_manager, uint16_t u2_error_flag);
void pfc_manager_fil_vac_calc(st_ac_fil_t *p_st_ac_fil, float f4_vac);
void pfc_manager_relay_ctrl(st_pfc_manager_t *p_st_pfc_manager);
float pfc_manager_reference_vdc_set(st_pfc_manager_t *p_st_pfc_manager);
void pfc_manager_state_machine_process(st_pfc_manager_t *p_st_pfc_manager);
void pfc_manager_voltage_dip_detection(st_pfc_manager_t *p_st_pfc_manager);

#endif /* R_PFC_MANAGER_H */
