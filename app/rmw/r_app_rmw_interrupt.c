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
* File Name   : r_app_rmw_interrupt.c
* Description : Interrupt Handlers
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 16.06.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_app_rmw.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/

#if     defined(ICS_INT_ERI)
/***********************************************************************************************************************
* Function Name : ics_eri_interrupt
* Description   : SCI ERI interrupt for ICS board
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
#pragma interrupt ics_eri_interrupt(vect=ICS_INT_ERI)
static void ics_eri_interrupt(void)
{
#if     defined(BSP_MCU_RX13T)
    /* no code */
#else
    ics_int_sci_eri();
#endif
} /* End of function ics_eri_interrupt */
#endif

/***********************************************************************************************************************
* Function Name : ics_rxi_interrupt
* Description   : SCI RXI interrupt for ICS board
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
#pragma interrupt ics_rxi_interrupt(vect=ICS_INT_RXI)
static void ics_rxi_interrupt(void)
{
    ics_int_sci_rxi();
} /* End of function ics_rxi_interrupt */

/***********************************************************************************************************************
* Function Name : ics_txi_interrupt
* Description   : SCI TXI interrupt for ICS board
*                 Interrupt triggered by DTC transmission, for preventing undefined interrupt
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
#pragma interrupt ics_txi_interrupt(vect=ICS_INT_TXI)
static void ics_txi_interrupt(void)
{
    /* no code */
} /* End of function ics_txi_interrupt */

#if     defined(ICS_INT_TEI)
/***********************************************************************************************************************
* Function Name : ics_eri_interrupt
* Description   : SCI TEI interrupt for ICS board
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
#pragma interrupt ics_tei_interrupt(vect=ICS_INT_TEI)
static void ics_tei_interrupt(void)
{
    /* no code */
} /* End of function ics_tei_interrupt */
#endif
