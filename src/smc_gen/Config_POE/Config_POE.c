/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_POE.c
* Component Version: 1.11.0
* Device(s)        : R5F513T3AxFJ
* Description      : This file implements device driver for Config_POE.
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
#include "Config_POE.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_POE_Create
* Description  : This function initializes the POE3 module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_POE_Create(void)
{
    /* Active levels setting of the MTU outputs */
    POE.ALR1.WORD = _0000_POE_MTIOC3B_LOW | _0000_POE_MTIOC3D_LOW | _0000_POE_MTIOC4A_LOW | _0000_POE_MTIOC4C_LOW | 
                    _0000_POE_MTIOC4B_LOW | _0000_POE_MTIOC4D_LOW | _0080_POE_ACTIVELEL_ENABLE;

    /* Port output enable control setting */
    POE.POECR1.BYTE = _00_POE_MTU0APIN_DISABLE | _00_POE_MTU0BPIN_DISABLE | _00_POE_MTU0CPIN_DISABLE | 
                      _00_POE_MTU0DPIN_DISABLE;
    POE.POECR2.WORD = _0100_POE_MTU4BDPIN_ENABLE | _0200_POE_MTU4ACPIN_ENABLE | _0400_POE_MTU3BDPIN_ENABLE | _0007_POE_POECR2_DEFAULT;
    POE.POECR4.WORD = _0008_POE_IC3ADDMT34ZE_ENABLE | _0402_POE_POECR4_DEFAULT;
    POE.ICSR3.WORD = _0000_POE_POE8M_SEL_DEFAULT | _0100_POE_PIE3_ENABLE | _0200_POE_POE8E_ENABLE;
    POE.ICSR4.WORD = _0000_POE_POE10E_DISABLE;

    /* Stopped oscillation detection control setting */
    POE.ICSR6.WORD = _0000_POE_OSTSTE_DISABLE;

    /* Output level control setting */
    POE.OCSR1.WORD = _0100_POE_OIE1_ENABLE | _0200_POE_OCE1_ENABLE;

    /* Set POE3 interrupt priority level */
    IPR(POE,OEI1) = _0F_POE_PRIORITY_LEVEL15;

    /* Set POE8# pin */
    MPC.P11PFS.BYTE = 0x07U;
    PORT1.PMR.BYTE |= 0x02U;

    R_Config_POE_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_POE_Start
* Description  : This function starts the POE3 module
*                Note that this function is empty if no interrupt is configured for POE
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_POE_Start(void)
{
    /* Enable POE3 interrupts in ICU */
    IEN(POE,OEI1) = 1U;
    IEN(POE,OEI3) = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_POE_Stop
* Description  : This function stops the POE3 module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_POE_Stop(void)
{
    /* Disable POE3 interrupts in ICU */
    IEN(POE,OEI1) = 0U;
    IEN(POE,OEI3)= 0U;

    /* Clear POE8F flag */
    if (1U == POE.ICSR3.BIT.POE8F)
    {
        POE.ICSR3.BIT.POE8F = 0U;
    }

    /* Clear POE10F flag */
    if (1U == POE.ICSR4.BIT.POE10F)
    {
        POE.ICSR4.BIT.POE10F = 0U;
    }

    /* Clear output short flag 1 */
    if (1U == POE.OCSR1.BIT.OSF1)
    {
        POE.OCSR1.BIT.OSF1 = 0U;
    }

    /* Clear OSTST High-Impedance flag */
    if (1U == POE.ICSR6.BIT.OSTSTF)
    {
        POE.ICSR6.BIT.OSTSTF = 0U;
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
