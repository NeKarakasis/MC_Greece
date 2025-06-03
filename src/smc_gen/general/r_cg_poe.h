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
* File Name        : r_cg_poe.h
* Version          : 1.0.150
* Device(s)        : R5F513T3AxFL
* Description      : General header file for POE peripheral.
***********************************************************************************************************************/

#ifndef POE_H
#define POE_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Input Level Control/Status Register 1 (ICSR1)
*/
/* POE0 Mode Select (POE0M) */
#define _0000_POE_POE0M_SEL_DEFAULT      (0x0000U) /* Accepts request on the falling edge of POE0# input */
#define _0001_POE_POE0M_SEL_8            (0x0001U) /* Accepts request when sampled 16 times at PCLK/8 and all low */
#define _0002_POE_POE0M_SEL_16           (0x0002U) /* Accepts request when sampled 16 times at PCLK/16 and all low */
#define _0003_POE_POE0M_SEL_128          (0x0003U) /* Accepts request when sampled 16 times at PCLK/128 and all low */
/* Port Interrupt Enable 1 (PIE1) */
#define _0000_POE_PIE1_DISABLE           (0x0000U) /* OEI1 interrupt requests by the input level detection disabled */
#define _0100_POE_PIE1_ENABLE            (0x0100U) /* OEI1 interrupt requests by the input level detection enabled */

/*
    Input Level Control/Status Register 3 (ICSR3)
*/
/* POE8 Mode Select (POE8M) */
#define _0000_POE_POE8M_SEL_DEFAULT      (0x0000U) /* Accepts request on the falling edge of POE8# input */
#define _0001_POE_POE8M_SEL_8            (0x0001U) /* Accepts request when sampled 16 times at PCLK/8 and all low */
#define _0002_POE_POE8M_SEL_16           (0x0002U) /* Accepts request when sampled 16 times at PCLK/16 and all low */
#define _0003_POE_POE8M_SEL_128          (0x0003U) /* Accepts request when sampled 16 times at PCLK/128 and all low */
/* Port Interrupt Enable 3 (PIE3) */
#define _0000_POE_PIE3_DISABLE           (0x0000U) /* OEI3 interrupt requests by the input level detection disabled */
#define _0100_POE_PIE3_ENABLE            (0x0100U) /* OEI3 interrupt requests by the input level detection enabled */
/* POE8 High-Impedance Enable (POE8E) */
#define _0000_POE_POE8E_DISABLE          (0x0000U) /* Not places the POE8 pin in high-impedance state */
#define _0200_POE_POE8E_ENABLE           (0x0200U) /* Places the POE8 pin in high-impedance state */

/*
    Input Level Control/Status Register 4 (ICSR4)
*/
/* POE10 Mode Select (POE10M) */
#define _0000_POE_POE10M_SEL_DEFAULT     (0x0000U) /* Accepts request on the falling edge of POE10# input */
#define _0001_POE_POE10M_SEL_8           (0x0001U) /* Accepts request when sampled 16 times at PCLK/8 and all low */
#define _0002_POE_POE10M_SEL_16          (0x0002U) /* Accepts request when sampled 16 times at PCLK/16 and all low */
#define _0003_POE_POE10M_SEL_128         (0x0003U) /* Accepts request when sampled 16 times at PCLK/128 and all low */
/* Port Interrupt Enable 4 (PIE4) */
#define _0000_POE_PIE4_DISABLE           (0x0000U) /* OEI4 interrupt requests by the input level detection disabled */
#define _0100_POE_PIE4_ENABLE            (0x0100U) /* OEI4 interrupt requests by the input level detection enabled */
/* POE10 High-Impedance Enable (POE10E) */
#define _0000_POE_POE10E_DISABLE         (0x0000U) /* Not places the POE10 pin in high-impedance state */
#define _0200_POE_POE10E_ENABLE          (0x0200U) /* Places the POE10 pin in high-impedance state */

/*
    Input Level Control/Status Register 6 (ICSR6)
*/
/* OSTST High-Impedance Enable (OSTSTE) */
#define _0000_POE_OSTSTE_DISABLE         (0x0000U) /* MTU0 and complementary PWM output pins are disabled */
#define _0200_POE_OSTSTE_ENABLE          (0x0200U) /* MTU0 and complementary PWM output pins are enabled */

/*
    Output Level Control/Status Register 1 (OCSR1)
*/
/* Output Short Interrupt Enable 1 (OIE1) */
#define _0000_POE_OIE1_DISABLE           (0x0000U) /* OEI1 interrupt requests by the output level comparison disabled */
#define _0100_POE_OIE1_ENABLE            (0x0100U) /* OEI1 interrupt requests by the output level comparison enabled */
/* Output Short High-Impedance Enable 1 (OCE1) */
#define _0000_POE_OCE1_DISABLE           (0x0000U) /* Does not place the pins in high-impedance */
#define _0200_POE_OCE1_ENABLE            (0x0200U) /* Places the pins in high-impedance */

/*
    Active Level Setting Register 1 (ALR1)
*/
/* MTIOC3B Active Level Setting (OLSG0A) */
#define _0000_POE_MTIOC3B_LOW            (0x0000U) /* MTIOC3B active level low */
#define _0001_POE_MTIOC3B_HIGH           (0x0001U) /* MTIOC3B active level high */
/* MTIOC3D Active Level Setting (OLSG0B) */
#define _0000_POE_MTIOC3D_LOW            (0x0000U) /* MTIOC3D active level low */
#define _0002_POE_MTIOC3D_HIGH           (0x0002U) /* MTIOC3D active level high */
/* MTIOC4A Active Level Setting (OLSG1A) */
#define _0000_POE_MTIOC4A_LOW            (0x0000U) /* MTIOC4A active level low */
#define _0004_POE_MTIOC4A_HIGH           (0x0004U) /* MTIOC4A active level high */
/* MTIOC4C Active Level Setting (OLSG1B) */
#define _0000_POE_MTIOC4C_LOW            (0x0000U) /* MTIOC4C active level low */
#define _0008_POE_MTIOC4C_HIGH           (0x0008U) /* MTIOC4C active level high */
/* MTIOC4B Active Level Setting (OLSG2A) */
#define _0000_POE_MTIOC4B_LOW            (0x0000U) /* MTIOC4B active level low */
#define _0010_POE_MTIOC4B_HIGH           (0x0010U) /* MTIOC4B active level high */
/* MTIOC4D Active Level Setting (OLSG2B) */
#define _0000_POE_MTIOC4D_LOW            (0x0000U) /* MTIOC4D active level low */
#define _0020_POE_MTIOC4D_HIGH           (0x0020U) /* MTIOC4D active level high */
/* Active Level Setting Enable (OLSEN) */
#define _0000_POE_ACTIVELEL_DISABLE      (0x0000U) /* Active level disable */
#define _0080_POE_ACTIVELEL_ENABLE       (0x0080U) /* Active level enable */

/*
    Software Port Output Enable Register (SPOER)
*/
/* MTU3 and MTU4 Output High-Impedance Enable (MTUCH34HIZ) */
#define _00_POE_MTU34HIZ_DISABLE         (0x00U) /* Does not place the pins in high-impedance */
#define _01_POE_MTU34HIZ_ENABLE          (0x01U) /* Places the pins in high-impedance */
/* MTU0 Output High-Impedance Enable (MTUCH0HIZ) */
#define _00_POE_MTU0HIZ_DISABLE          (0x00U) /* Does not place the pins in high-impedance */
#define _04_POE_MTU0HIZ_ENABLE           (0x04U) /* Places the pins in high-impedance */

/*
    Port Output Enable Control Register 1 (POECR1)
*/
/* MTIOC0A High-Impedance Enable (MTU0AZE) */
#define _00_POE_NOPIN_SET                (0x00U) /* Does not perform the output disabling control for MTU pin */
#define _00_POE_MTU0APIN_DISABLE         (0x00U) /* Does not place the pin in high-impedance */
#define _01_POE_MTU0APIN_PB3_ENABLE      (0x01U) /* Places the pin PB3 in high-impedance */
/* MTIOC0B High-Impedance Enable (MTU0BZE) */
#define _00_POE_MTU0BPIN_DISABLE         (0x00U) /* Does not place the pin in high-impedance */
#define _02_POE_MTU0BPIN_PB2_ENABLE      (0x02U) /* Places the pin PB2 in high-impedance */
/* MTIOC0C High-Impedance Enable (MTU0CZE) */
#define _00_POE_MTU0CPIN_DISABLE         (0x00U) /* Does not place the pin in high-impedance */
#define _04_POE_MTU0CPIN_PB1_ENABLE      (0x04U) /* Places the pin PB1 in high-impedance */
/* MTIOC0D High-Impedance Enable (MTU0DZE) */
#define _00_POE_MTU0DPIN_DISABLE         (0x00U) /* Does not place the pin in high-impedance */
#define _08_POE_MTU0DPIN_PB0_ENABLE      (0x08U) /* Places the pin PB0 in high-impedance */
/* MTIOC0A High-Impedance Enable (MTU0A1ZE) */
#define _10_POE_MTU0APIN_PD3_ENABLE      (0x10U) /* Places the pin PD3 in high-impedance */
/* MTIOC0B High-Impedance Enable (MTU0B1ZE) */
#define _20_POE_MTU0BPIN_PD4_ENABLE      (0x20U) /* Places the pin PD4 in high-impedance */
/* MTIOC0C High-Impedance Enable (MTU0C1ZE) */
#define _40_POE_MTU0CPIN_PD5_ENABLE      (0x40U) /* Places the pin PD5 in high-impedance */
/* MTIOC0D High-Impedance Enable (MTU0D1ZE) */
#define _80_POE_MTU0DPIN_PD6_ENABLE      (0x80U) /* Places the pin PD6 in high-impedance */

/*
    Port Output Enable Control Register 2 (POECR2)
*/
/* MTIOC4B/4D High-Impedance Enable (MTU4BDZE) */
#define _0000_POE_MTU4BDPIN_DISABLE       (0x0000U) /* Does not place the pin in high-impedance */
#define _0100_POE_MTU4BDPIN_ENABLE        (0x0100U) /* Places the pin in high-impedance */
/* MTIOC4A/4C High-Impedance Enable (MTU4ACZE) */
#define _0000_POE_MTU4ACPIN_DISABLE       (0x0000U) /* Does not place the pin in high-impedance */
#define _0200_POE_MTU4ACPIN_ENABLE        (0x0200U) /* Places the pin in high-impedance */
/* MTIOC3B/3D High-Impedance Enable (MTU3BDZE) */
#define _0000_POE_MTU3BDPIN_DISABLE       (0x0000U) /* Does not place the pin in high-impedance */
#define _0400_POE_MTU3BDPIN_ENABLE        (0x0400U) /* Places the pin in high-impedance */
/* POECR2 default value */
#define _0007_POE_POECR2_DEFAULT         (0x0007U) /* POECR2 default value */

/*
    Port Output Enable Control Register 4 (POECR4)
*/
/* MTU CH34 High-Impedance CFLAG Add (CMADDMT34ZE) */
#define _0000_POE_CMADDMT34ZE_DISABLE    (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0001_POE_CMADDMT34ZE_ENABLE     (0x0001U) /* Add the pins to high-impedance control conditions */
/* MTU3 and MTU4 High-Impedance POE8F Add (IC3ADDMT34ZE) */
#define _0000_POE_IC3ADDMT34ZE_DISABLE   (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0008_POE_IC3ADDMT34ZE_ENABLE    (0x0008U) /* Add the pins to high-impedance control conditions */
/* MTU3 and MTU4 High-Impedance POE10F Add (IC4ADDMT34ZE) */
#define _0000_POE_IC4ADDMT34ZE_DISABLE   (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0010_POE_IC4ADDMT34ZE_ENABLE    (0x0010U) /* Add the pins to high-impedance control conditions */
/* POECR4 default value */
#define _0402_POE_POECR4_DEFAULT         (0x0402U) /* POECR4 default value */

/*
    Port Output Enable Control Register 5 (POECR5)
*/
/* MTU0 High-Impedance CFLAG Add (CMADDMT0ZE) */
#define _0000_POE_CMADDMT0ZE_DISABLE     (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0001_POE_CMADDMT0ZE_ENABLE      (0x0001U) /* Add the pins to high-impedance control conditions */
/* MTU0 High-Impedance POE0F Add(IC1ADDMT0Z) */
#define _0000_POE_IC1ADDMT0Z_DISABLE     (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0002_POE_IC1ADDMT0Z_ENABLE      (0x0002U) /* Add the pins to high-impedance control conditions */
/* MTU0 High-Impedance POE10F Add (IC4ADDMT0Z) */
#define _0000_POE_IC4ADDMT0Z_DISABLE     (0x0000U) /* Does not add the pins to high-impedance control conditions */
#define _0010_POE_IC4ADDMT0Z_ENABLE      (0x0010U) /* Add the pins to high-impedance control conditions */
/* POECR5 default value */
#define _0008_POE_POECR5_DEFAULT         (0x0008U) /* POECR5 default value */

/*
    Port Output Enable Comparator Request Select Register (POECMPSEL)
*/
/* Comparator Channel 0 POE Request Enable (POEREQ0) */
#define _0000_POE_POEREQ0_DISABLE        (0x0000U) /* Disables POE request generation upon comparator detection. */
#define _0001_POE_POEREQ0_ENABLE         (0x0001U) /* Enables POE request generation upon comparator detection. */
/* Comparator Channel 1 POE Request Enable (POEREQ1) */
#define _0000_POE_POEREQ1_DISABLE        (0x0000U) /* Enables POE request generation upon comparator detection. */
#define _0002_POE_POEREQ1_ENABLE         (0x0002U) /* Disables POE request generation upon comparator detection. */
/* Comparator Channel 2 POE Request Enable (POEREQ2) */
#define _0000_POE_POEREQ2_DISABLE        (0x0000U) /* Enables POE request generation upon comparator detection. */
#define _0004_POE_POEREQ2_ENABLE         (0x0004U) /* Disables POE request generation upon comparator detection. */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_POE_PRIORITY_LEVEL0          (0x00U) /* Level 0 (interrupt disabled) */
#define _01_POE_PRIORITY_LEVEL1          (0x01U) /* Level 1 */
#define _02_POE_PRIORITY_LEVEL2          (0x02U) /* Level 2 */
#define _03_POE_PRIORITY_LEVEL3          (0x03U) /* Level 3 */
#define _04_POE_PRIORITY_LEVEL4          (0x04U) /* Level 4 */
#define _05_POE_PRIORITY_LEVEL5          (0x05U) /* Level 5 */
#define _06_POE_PRIORITY_LEVEL6          (0x06U) /* Level 6 */
#define _07_POE_PRIORITY_LEVEL7          (0x07U) /* Level 7 */
#define _08_POE_PRIORITY_LEVEL8          (0x08U) /* Level 8 */
#define _09_POE_PRIORITY_LEVEL9          (0x09U) /* Level 9 */
#define _0A_POE_PRIORITY_LEVEL10         (0x0AU) /* Level 10 */
#define _0B_POE_PRIORITY_LEVEL11         (0x0BU) /* Level 11 */
#define _0C_POE_PRIORITY_LEVEL12         (0x0CU) /* Level 12 */
#define _0D_POE_PRIORITY_LEVEL13         (0x0DU) /* Level 13 */
#define _0E_POE_PRIORITY_LEVEL14         (0x0EU) /* Level 14 */
#define _0F_POE_PRIORITY_LEVEL15         (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
