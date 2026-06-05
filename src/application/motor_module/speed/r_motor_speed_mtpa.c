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
* File Name    : motor_speed_mtpa.c
* Description  : This module performs MTPA in FOC
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <stdint.h>
#include <math.h>

/* Main associated header file */
#include "r_motor_speed_mtpa.h"

/* Project headers */
#include "r_motor_common.h"

#define scaling_factor  (0.35f)
#define BLEND_RANGE_RAD_PER_SEC  (180.0f)

float Id_mtpa;
uint8_t MTPA_enabled_FW_disabled;
uint8_t MTPA_disabled_FW_enabled;

static float clamp(float value, float min_val, float max_val)
{
    if (value < min_val)
        return min_val;
    else if (value > max_val)
        return max_val;
    else
        return value;
}


/***********************************************************************************************************************
* Function Name: motor_speed_mtpa_init
* Description  : Initializes MTPA module
* Arguments    : p_mtpa - The pointer to the structure of MTPA module
*                p_mtr - The pointer to the motor parameter data structure
* Return Value : None
***********************************************************************************************************************/
void motor_speed_mtpa_init(st_mtpa_t * p_mtpa,  st_motor_parameter_t * p_mtr)
{
    /* Set up motor */
    motor_speed_mtpa_motor_param_set(p_mtpa, p_mtr);
} /* End of function motor_speed_mtpa_init */

/***********************************************************************************************************************
* Function Name: motor_speed_mtpa_wekn_judge
* Description  : Executes the MTPA and flux-weakening selection function
* Arguments    : p_mtpa - The pointer to the structure of MTPA module
*                f4_speed_rad - The electrical speed of motor in [rad/s]
*                f4_va_max - The maximum magnitude of voltage vector
*                f4_idref_mtpa - The reference current for d-axis of MTPA
*                f4_idref_weak - The reference current for d-axis of Flux-weakening
*                p_f4_idq_ref - The pointer to the reference current vector (array) in format {Idref, Iqref}
* Return Value : 0
***********************************************************************************************************************/
uint16_t motor_speed_mtpa_wekn_judge(st_mtpa_t * p_mtpa,
                                     float f4_speed_rad,
                                     float f4_va_max,
                                     float f4_idref_mtpa,
                                     float f4_idref_weak,
                                     float * p_f4_idq_ref)
{
    uint16_t u2_output_type = 0;
    float f4_flux_pm = p_mtpa->p_motor->f4_mtr_m;

    /* Calculate Max Ta and d-pole current command */
    float f4_wc = (f4_va_max / f4_flux_pm);
  //  const float BLEND_RANGE = f4_wc;
  //  float blend = 0.0f;
   //f4_wc = scaling_factor*f4_wc;

    if (f4_speed_rad < f4_wc) //-BLEND_RANGE_RAD_PER_SEC)
    {
        /* MTPA */

        /* Check rated speed or more / less */
        if (f4_idref_mtpa > f4_idref_weak)
        {
            /* Select Flux-weakening */
            u2_output_type = 0;
            MTPA_enabled_FW_disabled =0;
            MTPA_disabled_FW_enabled =1;
        }
        else
        {
            /* Select MTPA */
            u2_output_type = 1;
            MTPA_enabled_FW_disabled =1;
            MTPA_disabled_FW_enabled =0;
        }
    }
    else
    {
        /* Flux-weakening */
        u2_output_type = 0;
      //  blend = clamp((f4_speed_rad - f4_wc) / BLEND_RANGE, 0.0f, 1.0f);
           //     MTPA_enabled_FW_disabled = 0;
        //        MTPA_disabled_FW_enabled = 1;
    }

    /* Output Id */
    switch (u2_output_type)
    {
        case 0:
            p_f4_idq_ref[0] = f4_idref_weak;
        break;

        case 1:
            p_f4_idq_ref[0] = f4_idref_mtpa;
        break;
    }

   // p_f4_idq_ref[0] = (1.0f - blend) * f4_idref_mtpa + blend * f4_idref_weak;
    return (0);
} /* End of function motor_speed_mtpa_wekn_judge */

/***********************************************************************************************************************
* Function Name: motor_speed_mtpa_run
* Description  : Executes the MTPA
* Arguments    : p_mtpa - The pointer to the structure of MTPA module
*                p_f4_idq_ref - The pointer to the reference current vector (array) in format {Idref, Iqref}
* Return Value : 0
***********************************************************************************************************************/
uint16_t motor_speed_mtpa_run(st_mtpa_t * p_mtpa, float * p_f4_idq_ref)
{
    float f4_idref;
    float f4_iqref;
    float f4_max_ta;
    float f4_flux_pm = p_mtpa->p_motor->f4_mtr_m;
    float f4_ld      = p_mtpa->p_motor->f4_mtr_ld;
    float f4_lq      = p_mtpa->p_motor->f4_mtr_lq;

    /* Calculate Max Ta and d-pole current command */

    f4_max_ta = (f4_flux_pm * 0.5f / (f4_lq - f4_ld));
    f4_iqref = p_f4_idq_ref[1];

    /* Id */
    if (f4_max_ta > 0)
    {
        /* Maximum Torque Per Ampere (MTPA) Control */
        f4_idref = f4_max_ta - sqrtf(f4_max_ta * f4_max_ta + f4_iqref * f4_iqref);
    }
    else
    {
        /* Id = 0 control */
        f4_idref = 0.0f;
    }
    Id_mtpa = f4_idref;
    p_f4_idq_ref[0] = f4_idref;

    return (0);
} /* End of function motor_speed_mtpa_run */

/***********************************************************************************************************************
* Function Name: motor_speed_mtpa_motor_param_set
* Description  : Set up the motor parameter
* Arguments    : p_st_mtpa - The pointer to the structure of MTPA module
*                p_st_motor - The motor parameters to be used MTPA module
* Return Value : None
***********************************************************************************************************************/
void motor_speed_mtpa_motor_param_set(st_mtpa_t * p_st_mtpa,  st_motor_parameter_t * p_st_motor)
{
    p_st_mtpa->p_motor = p_st_motor;
} /* End of function motor_speed_mtpa_motor_param_set */
