/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MOTOR.c
* Component Version: 1.1.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_MOTOR.
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
#include "Config_MOTOR.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_Create
* Description  : This function initializes the MTU for motor
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_Create(void)
{
    /* Release MTU channel 3 from stop state */
    MSTP(MTU3) = 0U;
    /* Enable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 1U;
    /* Stop  MTU3, MTU4 counter */
    MTU.TSTRA.BYTE = MTU.TSTRA.BYTE & 0x3FU;

    /* Configure MTU3 and MTU4 as complementary PWM mode timer */
    MTU3.TIER.BYTE = 0x00U;
    MTU4.TIER.BYTE = 0x00U;
    MTU.TITCR1A.BIT.T3AEN = 0U;
    MTU.TITCR1A.BIT.T4VEN = 0U;
    MTU3.TCR.BYTE = _00_MTR_MTU_PCLK_1 | _00_MTR_MTU_CKEG_RISE;
    MTU4.TCR.BYTE = _00_MTR_MTU_PCLK_1 | _00_MTR_MTU_CKEG_RISE;
    MTU3.TCR2.BYTE = _00_MTR_MTU_PCLK_1;
    MTU4.TCR2.BYTE = _00_MTR_MTU_PCLK_1;
    MTU3.TCNT = _0040_3TCNT_VALUE;
    MTU4.TCNT = 0x0000U;
    MTU.TSYRA.BIT.SYNC3 = 0U;
    MTU.TSYRA.BIT.SYNC4 = 0U;
    MTU3.TGRB = _0554_3TGRB_VALUE;
    MTU3.TGRD = _0554_3TGRB_VALUE;
    MTU4.TGRA = _0554_4TGRA_VALUE;
    MTU4.TGRC = _0554_4TGRA_VALUE;
    MTU4.TGRB = _0554_4TGRB_VALUE;
    MTU4.TGRD = _0554_4TGRB_VALUE;
    MTU.TDERA.BIT.TDER = 1U;
    MTU.TDDRA = _0040_TDDRA_VALUE;
    MTU.TCDRA = _0A69_TCDRA_VALUE;
    MTU.TCBRA = _0A69_TCDRA_VALUE;
    MTU3.TGRA = _0AA9_SUM_VALUE;
    MTU3.TGRC = _0AA9_SUM_VALUE;
    MTU.TOCR1A.BYTE = _00_MTR_MTU_PSYE_DISABLE | _08_MTR_MTU_TOCL_DISABLE | _04_MTR_MTU_TOCS_TOCR2;
    MTU.TOCR2A.BYTE = _20_MTR_MTU_OLS3N_LH | _10_MTR_MTU_OLS3P_LH | _08_MTR_MTU_OLS2N_LH | _04_MTR_MTU_OLS2P_LH | 
                      _02_MTR_MTU_OLS1N_LH | _01_MTR_MTU_OLS1P_LH;
    MTU.TOLBRA.BYTE = MTU.TOCR2A.BYTE & 0x3FU;
    MTU4.TADCORA = _0002_MTR_MTU_TADCORA_VALUE;
    MTU4.TADCOBRA = _0002_MTR_MTU_TADCORA_VALUE;
    MTU4.TADCR.WORD = _0000_MTR_MTU_ADSYNCH_DISABLE | _0040_MTR_MTU_DTAE_ENABLE;
    MTU.TITMRA.BIT.TITM = 0U;
    MTU.TITCR1A.BYTE = _00_MTR_MTU_INT_SKIP_DISABLE << 4U | _00_MTR_MTU_INT_SKIP_DISABLE;
    MTU3.TMDR1.BYTE = _0E_MTR_MTU_CMT2 | _10_MTR_MTU_BFA_BUFFER | _20_MTR_MTU_BFB_BUFFER;
    MTU.TOERA.BYTE = 0xC0U;
    MTU3.TIORH.BYTE = 0x00U;
    MTU3.TIORL.BYTE = 0x00U;
    MTU4.TIORH.BYTE = 0x00U;
    MTU4.TIORL.BYTE = 0x00U;
    /* Disable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 0U;

    //Init Up (MTIOC3B)
    PORT7.PODR.BIT.B1 = 0U; //Active High
    PORT7.PDR.BIT.B1 = 1U;
    PORT7.PMR.BIT.B1 = 0U;
    MPC.P71PFS.BIT.PSEL = 1U;
    //Init Un (MTIOC3D)
    PORT7.PODR.BIT.B4 = 0U; //Active High
    PORT7.PDR.BIT.B4 = 1U;
    PORT7.PMR.BIT.B4 = 0U;
    MPC.P74PFS.BIT.PSEL = 1U;
    //Init Vp (MTIOC4A)
    PORT7.PODR.BIT.B2 = 0U; //Active High
    PORT7.PDR.BIT.B2 = 1U;
    PORT7.PMR.BIT.B2 = 0U;
    MPC.P72PFS.BIT.PSEL = 1U;
    //Init Wp (MTIOC4B)
    PORT7.PODR.BIT.B3 = 0U; //Active High
    PORT7.PDR.BIT.B3 = 1U;
    PORT7.PMR.BIT.B3 = 0U;
    MPC.P73PFS.BIT.PSEL = 1U;
    //Init Vn (MTIOC4C)
    PORT7.PODR.BIT.B5 = 0U; //Active High
    PORT7.PDR.BIT.B5 = 1U;
    PORT7.PMR.BIT.B5 = 0U;
    MPC.P75PFS.BIT.PSEL = 1U;
    //Init Wn (MTIOC4D)
    PORT7.PODR.BIT.B6 = 0U; //Active High
    PORT7.PDR.BIT.B6 = 1U;
    PORT7.PMR.BIT.B6 = 0U;
    MPC.P76PFS.BIT.PSEL = 1U;

    R_Config_MOTOR_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StarttTimerCount
* Description  : This function starts the MTU3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_StartTimerCount(void)
{

    /* Start MTU3, MTU4 counter */
    MTU.TCSYSTR.BYTE = (_10_MTR_MTU_SCH3_START | _08_MTR_MTU_SCH4_START);
}

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StopTimerCount
* Description  : This function stops the MTU3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_StopTimerCount(void)
{

    /* Stop MTU3, MTU4 channel counter */
    MTU.TSTRA.BYTE = MTU.TSTRA.BYTE & 0x3FU;
}

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StartTimerOutput
* Description  : This function starts the MTU output
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_StartTimerCtrl(void)
{
    /* Enable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 1U;

    //Control Up (MTIOC3B)
    PORT7.PODR.BIT.B1 = 0U; //Active High
    PORT7.PMR.BIT.B1 = 1U;
    //Control Un (MTIOC3D)
    PORT7.PODR.BIT.B4 = 0U; //Active High
    PORT7.PMR.BIT.B4 = 1U;
    //Control Vp (MTIOC4A)
    PORT7.PODR.BIT.B2 = 0U; //Active High
    PORT7.PMR.BIT.B2 = 1U;
    //Control Wp (MTIOC4B)
    PORT7.PODR.BIT.B3 = 0U; //Active High
    PORT7.PMR.BIT.B3 = 1U;
    //Control Vn (MTIOC4C)
    PORT7.PODR.BIT.B5 = 0U; //Active High
    PORT7.PMR.BIT.B5 = 1U;
    //Control Wn (MTIOC4D)
    PORT7.PODR.BIT.B6 = 0U; //Active High
    PORT7.PMR.BIT.B6 = 1U;
    //Enable MTU3,MTU4 PWM output
    MTU.TOERA.BYTE = _C1_MTR_MTU_OE3B_ENABLE | _C8_MTR_MTU_OE3D_ENABLE | _C2_MTR_MTU_OE4A_ENABLE | 
                     _D0_MTR_MTU_OE4C_ENABLE | _C4_MTR_MTU_OE4B_ENABLE | _E0_MTR_MTU_OE4D_ENABLE;

    /* Disable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StopTimerOutput
* Description  : This function stops the MTU output
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_StopTimerCtrl(void)
{
    /* Enable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 1U;

    //Disable MTU3,MTU4 PWM output
    MTU.TOERA.BYTE = 0xC0U;
    //Control Up (MTIOC3B)
    PORT7.PODR.BIT.B1 = 0U; //Active High
    PORT7.PMR.BIT.B1 = 0U;
    //Control Un (MTIOC3D)
    PORT7.PODR.BIT.B4 = 0U; //Active High
    PORT7.PMR.BIT.B4 = 0U;
    //Control Vp (MTIOC4A)
    PORT7.PODR.BIT.B2 = 0U; //Active High
    PORT7.PMR.BIT.B2 = 0U;
    //Control Wp (MTIOC4B)
    PORT7.PODR.BIT.B3 = 0U; //Active High
    PORT7.PMR.BIT.B3 = 0U;
    //Control Vn (MTIOC4C)
    PORT7.PODR.BIT.B5 = 0U; //Active High
    PORT7.PMR.BIT.B5 = 0U;
    //Control Wn (MTIOC4D)
    PORT7.PODR.BIT.B6 = 0U; //Active High
    PORT7.PMR.BIT.B6 = 0U;

    /* Disable read/write to MTU3, MTU4 registers */
    MTU.TRWERA.BIT.RWE = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_UpdDuty
* Description  : This function updates duty cycle
* Arguments    : duty_u -
*                    U phase duty register value
*                duty_v -
*                    V phase duty register value
*                duty_w -
*                    W phase duty register value
* Return Value : None
***********************************************************************************************************************/

void R_Config_MOTOR_UpdDuty(uint16_t duty_u, uint16_t duty_v, uint16_t duty_w)
{
    MTU3.TGRD = duty_u;
    MTU4.TGRC = duty_v;
    MTU4.TGRD = duty_w;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
