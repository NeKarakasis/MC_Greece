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
* File Name   : r_app_rmw.h
* Description : Definitions of user interface (tool) processes
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_APP_RMW_H
#define R_APP_RMW_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_smc_entry.h"
#include "r_app_control_cfg.h"
#include "r_motor_sensorless_vector_api.h"

#if     defined(BSP_MCU_RX13T)
    #include "ICS2_RX13T.h"
#elif   defined(BSP_MCU_RX23T)
    #include "ICS_RX23T.h"
#elif   defined(BSP_MCU_RX24T)
    #include "ICS_RX24T.h"
#elif   defined(BSP_MCU_RX24U)
    #include "ICS_RX24U.h"
#elif   defined(BSP_MCU_RX66T)
    #include "ICS2_RX66T.h"
#elif   defined(BSP_MCU_RX72T)
    #include "ICS2_RX72T.h"
#elif   defined(BSP_MCU_RX72M)
    #include "ICS2_RX72M.h"
#else
    #error Please select a valid CPU board by defining a <MCU>_<MRSSK> macro in r_motor_module_cfg.h
#endif

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define     ICS_DECIMATION          (3)         /* ICS watch skipping number */

/* for ICS */
#define     ICS_INT_LEVEL           (6)         /* interrupt priority level */
#define     ICS_BRR                 (3)         /* baudrate select */
#define     ICS_INT_MODE            (1)         /* mode select */

#define     ICS_SCI_CH_SELECT       (APP_CFG_SCI_CH_SELECT)

#if     defined(BSP_MCU_RX13T)                  /* for RX13T */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_ERI    VECT(SCI1,ERI1)
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_INT_TEI    VECT(SCI1,TEI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI1_PB6_PB7)   /* SCI1 PB6/TXD1 PB7/RXD1 */
    #define ICS_INT_ERI    VECT(SCI1,ERI1)
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_INT_TEI    VECT(SCI1,TEI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PB6_PB7)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB6_PB7)   /* SCI5 PB6/TXD5 PB7/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB6_PB7)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB2_PB1)   /* SCI5 PB2/TXD5 PB1/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB2_PB1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_P23_P24)   /* SCI5 P23/TXD5 P24/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_P23_P24)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_P80_P94)  /* SCI12 P80/TXD12 P94/RXD12 */
    #define ICS_INT_ERI    VECT(SCI12,ERI12)
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_INT_TEI    VECT(SCI12,TEI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_P80_P94)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX23T)                  /* for RX23T */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_ERI    VECT(SCI1,ERI1)
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_INT_TEI    VECT(SCI1,TEI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB5_PB6)   /* SCI5 PB5/TXD5 PB6/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB2_PB1)   /* SCI5 PB2/TXD5 PB1/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB2_PB1)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX24T)                  /* for RX24T */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_ERI    VECT(SCI1,ERI1)
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_INT_TEI    VECT(SCI1,TEI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB5_PB6)   /* SCI5 PB5/TXD5 PB6/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB2_PB1)   /* SCI6 PB2/TXD6 PB1/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB2_PB1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB0_PA5)   /* SCI6 PB0/TXD6 PA5/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB0_PA5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_P81_P80)   /* SCI6 P81/TXD6 P80/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_P81_P80)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX24U)                  /* for RX24U */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_ERI    VECT(SCI1,ERI1)
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_INT_TEI    VECT(SCI1,TEI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB5_PB6)   /* SCI5 PB5/TXD5 PB6/RXD5 */
    #define ICS_INT_ERI    VECT(SCI5,ERI5)
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_INT_TEI    VECT(SCI5,TEI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB2_PB1)   /* SCI6 PB2/TXD6 PB1/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB2_PB1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB0_PA5)   /* SCI6 PB0/TXD6 PA5/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB0_PA5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_P81_P80)   /* SCI6 P81/TXD6 P80/RXD6 */
    #define ICS_INT_ERI    VECT(SCI6,ERI6)
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_INT_TEI    VECT(SCI6,TEI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_P81_P80)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX66T)                  /* for RX66T */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PD7_PE0)   /* SCI5 PD7/TXD5 PE0/RXD5 */
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PD7_PE0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB5_PB6)   /* SCI5 PB5/TXD5 PB6/RXD5 */
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB0_PB1)   /* SCI6 PB0/TXD6 PB1/RXD6 */
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB0_PB1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PD0_PD1)   /* SCI8 PD0/TXD8 PD1/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PD0_PD1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PC1_PC0)   /* SCI8 PC1/TXD8 PC0/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PC1_PC0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PA4_PA5)   /* SCI8 PA4/TXD8 PA5/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PA4_PA5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_P23_P22)   /* SCI8 P23/TXD8 P22/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_P23_P22)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_P01_P00)   /* SCI9 P01/TXD9 P00/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_P01_P00)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_PA3_PA2)   /* SCI9 PA3/TXD9 PA2/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_PA3_PA2)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_PG1_PG0)   /* SCI9 PG1/TXD9 PG0/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_PG1_PG0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PD3_PD5)  /* SCI11 PD3/TXD11 PD5/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PB5_PB6)  /* SCI11 PB5/TXD11 PB6/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PA0_PA1)  /* SCI11 PA0/TXD11 PA1/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PA0_PA1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_PB5_PB6)  /* SCI12 PB5/TXD12 PB6/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_P23_P22)  /* SCI12 P23/TXD12 P22/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_P23_P22)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_P01_P00)  /* SCI12 P01/TXD12 P00/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_P01_P00)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX72T)                  /* for RX72T */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PD3_PD5)     /* SCI1 PD3/TXD1 PD5/RXD1 */
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PD7_PE0)   /* SCI5 PD7/TXD5 PE0/RXD5 */
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PD7_PE0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PB5_PB6)   /* SCI5 PB5/TXD5 PB6/RXD5 */
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB0_PB1)   /* SCI6 PB0/TXD6 PB1/RXD6 */
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB0_PB1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PD0_PD1)   /* SCI8 PD0/TXD8 PD1/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PD0_PD1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PC1_PC0)   /* SCI8 PC1/TXD8 PC0/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PC1_PC0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PA4_PA5)   /* SCI8 PA4/TXD8 PA5/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PA4_PA5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_P23_P22)   /* SCI8 P23/TXD8 P22/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_P23_P22)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_P01_P00)   /* SCI9 P01/TXD9 P00/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_P01_P00)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_PA3_PA2)   /* SCI9 PA3/TXD9 PA2/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_PA3_PA2)
#elif (ICS_SCI_CH_SELECT == ICS_SCI9_PG1_PG0)   /* SCI9 PG1/TXD9 PG0/RXD9 */
    #define ICS_INT_RXI    VECT(SCI9,RXI9)
    #define ICS_INT_TXI    VECT(SCI9,TXI9)
    #define ICS_CTRL_PORT  (ICS_SCI9_PG1_PG0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PD3_PD5)  /* SCI11 PD3/TXD11 PD5/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PD3_PD5)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PB5_PB6)  /* SCI11 PB5/TXD11 PB6/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI11_PA0_PA1)  /* SCI11 PA0/TXD11 PA1/RXD11 */
    #define ICS_INT_RXI    VECT(SCI11,RXI11)
    #define ICS_INT_TXI    VECT(SCI11,TXI11)
    #define ICS_CTRL_PORT  (ICS_SCI11_PA0_PA1)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_PB5_PB6)  /* SCI12 PB5/TXD12 PB6/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_PB5_PB6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_P23_P22)  /* SCI12 P23/TXD12 P22/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_P23_P22)
#elif (ICS_SCI_CH_SELECT == ICS_SCI12_P01_P00)  /* SCI12 P01/TXD12 P00/RXD12 */
    #define ICS_INT_RXI    VECT(SCI12,RXI12)
    #define ICS_INT_TXI    VECT(SCI12,TXI12)
    #define ICS_CTRL_PORT  (ICS_SCI12_P01_P00)
#else
    #error Please select a valid SCI channel
#endif

#elif   defined(BSP_MCU_RX72M)                  /* for RX72M */
#if (ICS_SCI_CH_SELECT == ICS_SCI1_PF0_PF2)     /* SCI1 PF0/TXD1 PF2/RXD1 */
    #define ICS_INT_RXI    VECT(SCI1,RXI1)
    #define ICS_INT_TXI    VECT(SCI1,TXI1)
    #define ICS_CTRL_PORT  (ICS_SCI1_PF0_PF2)
#elif (ICS_SCI_CH_SELECT == ICS_SCI2_P50_P52)   /* SCI2 P50/TXD2 P52/RXD2 */
    #define ICS_INT_RXI    VECT(SCI2,RXI2)
    #define ICS_INT_TXI    VECT(SCI2,TXI2)
    #define ICS_CTRL_PORT  (ICS_SCI2_P50_P52)
#elif (ICS_SCI_CH_SELECT == ICS_SCI3_P23_P25)   /* SCI3 P23/TXD3 P25/RXD3 */
    #define ICS_INT_RXI    VECT(SCI3,RXI3)
    #define ICS_INT_TXI    VECT(SCI3,TXI3)
    #define ICS_CTRL_PORT  (ICS_SCI3_P23_P25)
#elif (ICS_SCI_CH_SELECT == ICS_SCI4_PB1_PB0)   /* SCI4 PB1/TXD4 PB0/RXD4 */
    #define ICS_INT_RXI    VECT(SCI4,RXI4)
    #define ICS_INT_TXI    VECT(SCI4,TXI4)
    #define ICS_CTRL_PORT  (ICS_SCI4_PB1_PB0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI5_PA4_PA3)   /* SCI5 PA4/TXD5 PA3/RXD5 */
    #define ICS_INT_RXI    VECT(SCI5,RXI5)
    #define ICS_INT_TXI    VECT(SCI5,TXI5)
    #define ICS_CTRL_PORT  (ICS_SCI5_PA4_PA3)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_P00_P01)   /* SCI6 P00/TXD6 P01/RXD6 */
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_P00_P01)
#elif (ICS_SCI_CH_SELECT == ICS_SCI6_PB1_PB0)   /* SCI6 PB1/TXD6 PB0/RXD6 */
    #define ICS_INT_RXI    VECT(SCI6,RXI6)
    #define ICS_INT_TXI    VECT(SCI6,TXI6)
    #define ICS_CTRL_PORT  (ICS_SCI6_PB1_PB0)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PJ2_PC6)   /* SCI8 PJ2/TXD8 PC6/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PJ2_PC6)
#elif (ICS_SCI_CH_SELECT == ICS_SCI8_PJ2_PJ1)   /* SCI8 PJ2/TXD8 PJ1/RXD8 */
    #define ICS_INT_RXI    VECT(SCI8,RXI8)
    #define ICS_INT_TXI    VECT(SCI8,TXI8)
    #define ICS_CTRL_PORT  (ICS_SCI8_PJ2_PJ1)
#else
    #error Please select a valid SCI channel
#endif

#else
    #error Please select a valid CPU board
#endif

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    /* Offset parameters */
    uint16_t    u2_offset_calc_time;        /* calculation time for current offset / 100 [us] */

    /* Motor parameters */
    st_motor_parameter_t   st_motor;        /* motor parameters */
    float       f4_max_speed_rpm;           /* maximum speed [rpm] (mechanical) */

    /* Sensorless parameter */
    uint8_t     u1_ctrl_loop_mode;          /* loop mode select */
    float       f4_ol_ref_id;               /* id reference when open loop [A] */
    float       f4_id_up_time;              /* time to increase id */
    float       f4_id_down_time;            /* time to decrease id */
    float       f4_id_down_speed_rpm;       /* The speed threshold[rpm] to ramp down the d-axis current */
    float       f4_id_up_speed_rpm;         /* The speed threshold[rpm] to ramp up the d-axis current */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    int16_t     s2_difference_minimum;      /* Minimum difference of PWM duty */
    int16_t     s2_adjust_adc_delay;        /* Adjustment delay for A/D conversion */
#endif

    /* Control parameters */
    /* Current control */
    float       f4_current_omega_hz;        /* Natural frequency[Hz] for current loop gain design */
    float       f4_current_zeta;            /* Damping ratio for current loop gain design */

    /* Speed control */
    float       f4_speed_omega_hz;          /* Natural frequency[Hz] for speed loop gain design */
    float       f4_speed_zeta;              /* Damping ratio for speed loop gain design */
    float       f4_speed_lpf_hz;            /* Natural frequency for speed LPF [Hz] */
    float       f4_ref_speed_rpm;           /* reference speed [rpm] (mechanical) */
    float       f4_speed_rate_limit_rpm;    /* reference speed max change limit [rpm/s] */
    float       f4_overspeed_limit_rpm;     /* over speed limit [rpm] (mechanical) */

    /* Optional functions */
    /* Voltage error compensation */
    uint8_t     u1_flag_volt_err_comp_use;  /* voltage error compensation */

    /* Flux-weakening control */
    uint8_t     u1_flag_fluxwkn_use;        /* flux-weakening */

    /* BEMF observer */
    float       f4_e_obs_omega_hz;          /* Natural frequency for BEMF observer [Hz] */
    float       f4_e_obs_zeta;              /* Damping ratio for BEMF observer */
    float       f4_pll_est_omega_hz;        /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
    float       f4_pll_est_zeta;            /* Damping ratio for rotor position Phase-Locked Loop */

    /* Sensor-less switch control */
    uint8_t     u1_flag_less_switch_use;    /* Flags whether use sensor-less switch control*/
    float       f4_switch_phase_err_deg;    /* Phase error to decide sensor-less switch timing [deg] */
    float       f4_opl2less_sw_time;        /* Time to switch open-loop to sensor-less [s] */
    float       f4_phase_err_lpf_cut_freq;  /* Cut off frequency[Hz] of phase error LPF */

    /* Open-loop damping control */
    uint8_t     u1_flag_openloop_damping_use;   /* Flags whether use open-loop damping control */
    float       f4_ed_hpf_omega;                /* HPF cutoff frequency for ed [Hz] */
    float       f4_ol_damping_zeta;             /* Damping ratio of open-loop damping control */
    float       f4_ol_damping_fb_limit_rate;    /* Rate of reference speed for feedback speed limiter */
} st_rmw_param_buffer_t;

/***********************************************************************************************************************
* External global variables
***********************************************************************************************************************/
/* Operation variables */
extern uint8_t      com_u1_system_mode;             /* System mode */
extern uint8_t      g_u1_system_mode;               /* System mode */
extern uint8_t      com_u1_enable_write;            /* ICS write enable flag */
extern uint8_t      g_u1_enable_write;              /* ICS write enable flag */
extern uint8_t      com_u1_sw_userif;               /* User interface switch */
extern uint8_t      g_u1_sw_userif;                 /* User interface switch */

/* Offset parameters */
extern uint16_t     com_u2_offset_calc_time;        /* current offset calculation time */

/* Motor parameters */
extern uint16_t     com_u2_mtr_pp;                  /* pole pairs */
extern float        com_f4_mtr_r;                   /* resistance [ohm] */
extern float        com_f4_mtr_ld;                  /* d-axis inductance [H] */
extern float        com_f4_mtr_lq;                  /* q-axis inductance [H] */
extern float        com_f4_mtr_m;                   /* permanent magnetic flux [Wb] */
extern float        com_f4_mtr_j;                   /* rotor inertia [kgm^2] */
extern float        com_f4_nominal_current_rms;     /* The nominal current[Arms] */
extern float        com_f4_max_speed_rpm;           /* maximum speed [rpm] (mechanical) */

/* Sensorless parameter */
extern uint8_t      com_u1_ctrl_loop_mode;          /* loop mode select */
extern float        com_f4_ol_ref_id;               /* id reference when open loop [A] */
extern float        com_f4_id_up_time;              /* time to increase id */
extern float        com_f4_id_down_time;            /* time to decrease id */
extern float        com_f4_id_down_speed_rpm;       /* The speed threshold[rpm] to ramp down the d-axis current */
extern float        com_f4_id_up_speed_rpm;         /* The speed threshold[rpm] to ramp up the d-axis current */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
extern int16_t      com_s2_difference_minimum;      /* Minimum difference of PWM duty */
extern int16_t      com_s2_adjust_adc_delay;        /* Adjustment delay for A/D conversion */
#endif

/* Control parameters */
/* Current control */
extern float        com_f4_current_omega_hz;        /* natural frequency for current loop [Hz] */
extern float        com_f4_current_zeta;            /* damping ratio for current loop */

/* Speed control */
extern float        com_f4_speed_omega_hz;          /* natural frequency for speed loop [Hz] */
extern float        com_f4_speed_zeta;              /* damping ratio for speed loop */
extern float        com_f4_speed_lpf_hz;            /* Natural frequency for speed LPF [Hz] */
extern float        com_f4_ref_speed_rpm;           /* motor speed reference [rpm] (mechanical) */
extern float        com_f4_speed_rate_limit_rpm;    /* limit of speed change [rpm/s] */
extern float        com_f4_overspeed_limit_rpm;     /* over speed limit [rpm] (mechanical) */

/* Optional functions */
/* Voltage error compensation */
extern uint8_t      com_u1_flag_volt_err_comp_use;  /* Flags whether use voltage error compensation */

/* Flux-weakening control */
extern uint8_t      com_u1_flag_fluxwkn_use;        /* Flags whether use flux-weakening */

/* BEMF observer */
extern float        com_f4_e_obs_omega_hz;          /* Natural frequency for BEMF observer [Hz] */
extern float        com_f4_e_obs_zeta;              /* Damping ratio for BEMF observer */
extern float        com_f4_pll_est_omega_hz;        /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
extern float        com_f4_pll_est_zeta;            /* Damping ratio for rotor position Phase-Locked Loop */

/* Sensor-less switch control */
extern uint8_t      com_u1_flag_less_switch_use;    /* Flags whether use sensor-less switch control*/
extern float        com_f4_switch_phase_err_deg;    /* Phase error to decide sensor-less switch timing [deg] */
extern float        com_f4_opl2less_sw_time;        /* Time to switch open-loop to sensor-less [s] */
extern float        com_f4_phase_err_lpf_cut_freq;  /* Cut off frequency[Hz] of phase error LPF */

/* Open-loop damping control */
extern uint8_t      com_u1_flag_openloop_damping_use;   /* Flags whether use open-loop damping control */
extern float        com_f4_ed_hpf_omega;                /* HPF cutoff frequency for ed [Hz] */
extern float        com_f4_ol_damping_zeta;             /* Damping ratio of open-loop damping control */
extern float        com_f4_ol_damping_fb_limit_rate;    /* Rate of reference speed for feedback speed limiter */

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : r_app_rmw_copy_com_to_buffer
* Description   : Set com variables to global value
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_copy_com_to_buffer(void);

/***********************************************************************************************************************
* Function Name : r_app_rmw_ui_init
* Description   : Initialize variables for Analyzer interface
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_ui_init(void);

/***********************************************************************************************************************
* Function Name : r_app_rmw_ui_mainloop
* Description   : RMW communication main-loop process
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_ui_mainloop(void);

/***********************************************************************************************************************
* Function Name : r_app_rmw_interrupt_handler
* Description   : Process for ICS(Analyzer)
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_interrupt_handler(void);

#endif /* R_APP_RMW_H */
