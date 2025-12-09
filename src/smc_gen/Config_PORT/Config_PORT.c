/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_PORT.c
* Component Version: 2.4.1
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_PORT.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_PORT.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_motor_common.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_PORT_Create
* Description  : This function initializes the PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_PORT_Create(void)
{
    /* Set PORTB registers */
    PORTB.PODR.BYTE = _00_Pm0_OUTPUT_0;
    PORTB.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTB.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTB.DSCR.BYTE = _01_Pm0_HIDRV_ON | _00_Pm3_HIDRV_OFF | _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTB.PMR.BYTE = _00_Pm0_PIN_GPIO;
    PORTB.PDR.BYTE = _01_Pm0_MODE_OUTPUT;

    R_Config_PORT_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/******************************************************************************
* Function Name : R_Config_PORT_GetSW1
* Description   : Get state of SW1
* Arguments     : none
* Return Value  : state of SW1
******************************************************************************/
uint8_t R_Config_PORT_GetSW1(void)
{
    return(PORTB.PIDR.BIT.B5);
} /* End of function R_Config_PORT_GetSW1 */

/******************************************************************************
* Function Name : R_Config_PORT_GetSW2
* Description   : Get state of SW2
* Arguments     : none
* Return Value  : state of SW2
******************************************************************************/
uint8_t R_Config_PORT_GetSW2(void)
{
    return (PORTB.PIDR.BIT.B4);
} /* End of function R_Config_PORT_GetSW2 */

/******************************************************************************
* Function Name : R_Config_PORT_Led1_on
* Description   : Turn on LED1
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led1_on(void)
{
    PORTD.PODR.BIT.B6 = MTR_LED_ON;
} /* End of function R_Config_PORT_Led1_on */

/******************************************************************************
* Function Name : R_Config_PORT_Led2_on
* Description   : Turn on LED2
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led2_on(void)
{
    PORTD.PODR.BIT.B4 = MTR_LED_ON;
} /* End of function R_Config_PORT_Led2_on */

/******************************************************************************
* Function Name : R_Config_PORT_Led3_on
* Description   : Turn on LED3
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led3_on(void)
{
    PORTB.PODR.BIT.B3 = MTR_LED_ON;
} /* End of function R_Config_PORT_Led3_on */

/******************************************************************************
* Function Name : R_Config_PORT_Led1_off
* Description   : Turn off LED1
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led1_off(void)
{
    PORTD.PODR.BIT.B6 = MTR_LED_OFF;
} /* End of function R_Config_PORT_Led1_off */

/******************************************************************************
* Function Name : R_Config_PORT_Led2_off
* Description   : Turn off LED2
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led2_off(void)
{
    PORTD.PODR.BIT.B4 = MTR_LED_OFF;
} /* End of function R_Config_PORT_Led2_off */

/******************************************************************************
* Function Name : R_Config_PORT_Led3_off
* Description   : Turn off LED3
* Arguments     : none
* Return Value  : none
******************************************************************************/
void R_Config_PORT_Led3_off(void)
{
    PORTD.PODR.BIT.B3 = MTR_LED_OFF;
} /* End of function R_Config_PORT_Led3_off */
/* End user code. Do not edit comment generated here */
