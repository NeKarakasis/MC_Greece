/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_PORT.h
* Component Version: 2.4.1
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_PORT.
***********************************************************************************************************************/

#ifndef CFG_Config_PORT_H
#define CFG_Config_PORT_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_port.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_PORT_Create(void);
void R_Config_PORT_Create_UserInit(void);
/* Start user code for function. Do not edit comment generated here */
/******************************************************************************
* Function Name : R_Config_PORT_GetSW1
* Description   : Get state of SW1
* Arguments     : none
* Return Value  : state of SW1
******************************************************************************/
uint8_t R_Config_PORT_GetSW1(void);

/******************************************************************************
* Function Name : R_Config_PORT_GetSW2
* Description   : Get state of SW2
* Arguments     : none
* Return Value  : state of SW2
******************************************************************************/
uint8_t R_Config_PORT_GetSW2(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led1_on
* Description   : Turn on LED1
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led1_on(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led2_on
* Description   : Turn on LED2
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led2_on(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led3_on
* Description   : Turn on LED3
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led3_on(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led1_off
* Description   : Turn off LED1
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led1_off(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led2_off
* Description   : Turn off LED2
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led2_off(void);

/******************************************************************************
* Function Name : R_Config_PORT_Led3_off
* Description   : Turn off LED3
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led3_off(void);
/* End user code. Do not edit comment generated here */
#endif
