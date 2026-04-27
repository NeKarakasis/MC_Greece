/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_PM_FOC
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_OUTER_FNC_H
#define RM_MOTOR_OUTER_FNC_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Library Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/** Dammy configured function  */
void algorithm_disabled(void * p_ctrl, void * p_cfg);

/** MTPA for function pointer table */
void motor_outer_fnctbl_mtpa_ctrl(void * p_ctrl, void * p_cfg);

/** Fluxweak control for function pointer table */
void motor_outer_fnctbl_fw_ctrl(void * p_ctrl, void * p_cfg);

/** Speed Observer related for function pointer table */
void motor_outer_fnctbl_spdobsrv_ctrl(void * p_ctrl, void * p_cfg);
void motor_outer_fnctbl_spddirct_ctrl(void * p_ctrl, void * p_cfg);
void motor_outer_fnctbl_spdlpf_ctrl(void * p_ctrl, void * p_cfg);
void motor_outer_fnctbl_extobsrv_ctrl(void * p_ctrl, void * p_cfg);

/** Control mode function for function pointer table */
void motor_outer_speed_control(void * p_ctrl, void * p_cfg);
void motor_outer_position_control(void * p_ctrl, void * p_cfg);

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_OUTER_FNC_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_PM_FOC)
 **********************************************************************************************************************/
