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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_speed.c
* Description : Processes of speed control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 10.06.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <math.h>
#include <stdbool.h>
/* Main associated header file */
#include "r_motor_speed.h"
#include "r_mtr_ics.h"

#define RPM_TO_RAD_PER_SEC(rpm) ((rpm) * (2.0f * 3.14159265f / 60.0f))

/***********************************************************************************************************************
* Function Name : motor_speed_max_speed_set
* Description   : Sets parameter for maximum speed
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_max_speed_rpm - maximum speed
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_max_speed_set(st_speed_control_t * p_st_sc, float f4_max_speed_rpm)
{
    p_st_sc->f4_max_speed_rad = f4_max_speed_rpm * MTR_RPM2RAD;
} /* End of function motor_speed_max_speed_set */

/***********************************************************************************************************************
* Function Name : motor_speed_rate_limit_set
* Description   : Sets parameter for speed rate limit
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_speed_rate_limit_rpm - speed rate limit[rpm/s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_rate_limit_set(st_speed_control_t * p_st_sc, float f4_speed_rate_limit_rpm)
{
    if (f4_speed_rate_limit_rpm > 0.0f)
    {
        p_st_sc->f4_speed_rate_limit_rad = p_st_sc->f4_speed_ctrl_period *
                f4_speed_rate_limit_rpm * MTR_RPM2RAD;
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function motor_speed_rate_limit_set */

/***********************************************************************************************************************
* Function Name : motor_speed_rate_limit_apply
* Description   : Limits the rate of change of speed reference
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : Limited speed reference
***********************************************************************************************************************/
float motor_speed_rate_limit_apply(st_speed_control_t * p_st_sc)
{
    float f4_temp0;
    float f4_temp1;
    float f4_speed_ref_calc_rad;

    f4_temp0 = p_st_sc->f4_ref_speed_rad_manual - p_st_sc->f4_ref_speed_rad_ctrl;
    f4_temp1 = fminf(p_st_sc->f4_speed_rate_limit_rad, fabsf(f4_temp0));
    f4_speed_ref_calc_rad = p_st_sc->f4_ref_speed_rad_ctrl + copysignf(f4_temp1, f4_temp0);

    return (f4_speed_ref_calc_rad);
} /* End of function motor_speed_rate_limit_apply */


float dynamic_motor_speed_limit(st_speed_control_t * p_st_sc)
{
    float current_speed_rad = p_st_sc->f4_speed_rad;
    float speed_rad_ctrl = p_st_sc->f4_ref_speed_rad_ctrl;

    bool is_decelarating = speed_rad_ctrl < current_speed_rad;

    if(is_decelarating){
        p_st_sc->f4_speed_rate_limit_rad = RPM_TO_RAD_PER_SEC(SPEED_CFG_RATE_LIMIT_RPM3);
    }
    else{
             if (current_speed_rad < RPM_TO_RAD_PER_SEC(5000.0f))
           {
              p_st_sc->f4_speed_rate_limit_rad = RPM_TO_RAD_PER_SEC(SPEED_CFG_RATE_LIMIT_RPM1); // High acceleration
               }
             else
             {
        p_st_sc->f4_speed_rate_limit_rad = RPM_TO_RAD_PER_SEC(SPEED_CFG_RATE_LIMIT_RPM2); // Low acceleration
               }
    }
    return motor_speed_rate_limit_apply(p_st_sc);
}


/***********************************************************************************************************************
* Function Name : motor_speed_pi_control
* Description   : Speed PI control
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_speed_rad - The electrical speed [rad/s]
* Return Value  : The Iq reference
***********************************************************************************************************************/
float motor_speed_pi_control(st_speed_control_t * p_st_sc, float f4_speed_rad)
{
    float f4_iq_ref_calc;

    p_st_sc->st_pi_speed.f4_err = p_st_sc->f4_ref_speed_rad_ctrl - f4_speed_rad;
    f4_iq_ref_calc = motor_pi_ctrl(&p_st_sc->st_pi_speed);

    return (f4_iq_ref_calc);
} /* End of function motor_speed_pi_control */

/***********************************************************************************************************************
* Function Name : motor_speed_ref_speed_set
* Description   : Updates the speed reference
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : Speed reference
***********************************************************************************************************************/
float motor_speed_ref_speed_set(st_speed_control_t * p_st_sc)
{
    float f4_speed_ref_calc_rad;

    switch (p_st_sc->u1_state_speed_ref)
    {
        case SPEED_STATE_ZERO_CONST:
            f4_speed_ref_calc_rad = 0.0f;
        break;

        case SPEED_STATE_POS_CONTROL_OUTPUT:
            /* Use inputs from ParameterSet */
            f4_speed_ref_calc_rad = p_st_sc->f4_ref_speed_rad;
        break;

        case SPEED_STATE_MANUAL:
            /* Limits the rate of change of speed reference */
        	 f4_speed_ref_calc_rad = dynamic_motor_speed_limit(p_st_sc);
        break;

        default:
            f4_speed_ref_calc_rad = 0.0f;
        break;
    }
    /* Speed reference limit */
    f4_speed_ref_calc_rad = motor_filter_limitf_abs(f4_speed_ref_calc_rad, p_st_sc->f4_max_speed_rad);

    return (f4_speed_ref_calc_rad);
} /* End of function motor_speed_ref_speed_set */

/***********************************************************************************************************************
* Function Name : motor_speed_flux_weakening
* Description   : Executes the flux-weakening
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_flux_weakening(st_speed_control_t * p_st_sc)
{
    float f4_idq_ref[2];
    float f4_idq_ad[2];
    float f4_speed_elec_rad;

    f4_idq_ref[0] = p_st_sc->f4_id_ref_output;
    f4_idq_ref[1] = p_st_sc->f4_iq_ref_output;

    /* This function will over-write the dq-axis current command */
    motor_speed_flux_weakn_vamax_set(&p_st_sc->st_fluxwkn, p_st_sc->f4_va_max);
    if (0.01f < fabs(p_st_sc->f4_speed_rad_ctrl))
    {
        /* Use the static maximum current instead of dynamic measured dq current
         *  to determine the voltage margin in flux-weakening calculation */
        f4_idq_ad[0] = 0.0f;
        f4_idq_ad[1] = p_st_sc->st_motor.f4_nominal_current_rms * MOTOR_COMMON_CFG_IA_MAX_CALC_MULT;

        f4_speed_elec_rad = p_st_sc->f4_speed_rad_ctrl * p_st_sc->st_motor.u2_mtr_pp;
        motor_speed_flux_weakn_start(&p_st_sc->st_fluxwkn, f4_speed_elec_rad, f4_idq_ad, f4_idq_ref);
    }
    else
    {
        /* Do Nothing */
    }

    if (f4_idq_ref[0] > 0.0f)
    {
        f4_idq_ref[0] = 0.0f;
    }
    else
    {
        /* Do Nothing */
    }

    p_st_sc->f4_id_ref_output = f4_idq_ref[0];
    p_st_sc->f4_iq_ref_output = f4_idq_ref[1];
} /* End of function motor_flux_speed_weakening */

/***********************************************************************************************************************
* Function Name : motor_speed_mtpa
* Description   : Executes the MTPA
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_mtpa(st_speed_control_t * p_st_sc)
{
    uint16_t u2_fw_status;
    float f4_idq_ref[2];
    float f4_idq_ref_temp[2];

    f4_idq_ref[0] = p_st_sc->f4_id_ref_output;
    f4_idq_ref[1] = p_st_sc->f4_iq_ref_output;

    f4_idq_ref_temp[0] = f4_idq_ref[0];
    f4_idq_ref_temp[1] = f4_idq_ref[1];

    /* MTPA */
    motor_speed_mtpa_run(&(p_st_sc->st_mtpa), &(f4_idq_ref_temp[0]));

    /* Get the status of flux-weakening module */
    u2_fw_status = motor_speed_flux_weakn_status_get(&(p_st_sc->st_fluxwkn));

    if ((FLUXWKN_STATE_FLUXWKN == u2_fw_status) && (MTR_FLG_SET == p_st_sc->u1_flag_fluxwkn_use))
    {
        /* Select MTPA or flux-weakening control */
        motor_speed_mtpa_wekn_judge(&(p_st_sc->st_mtpa),
                                    p_st_sc->f4_speed_rad_ctrl,
                                    p_st_sc->st_fluxwkn.f4_va_max,
                                    f4_idq_ref_temp[0],
                                    f4_idq_ref[0],
                                    &(f4_idq_ref[0]));
    }
    else
    {
        /* In case of Disabled Weak Control and Enabled MTPA */
        f4_idq_ref[0] = f4_idq_ref_temp[0];
        f4_idq_ref[1] = f4_idq_ref_temp[1];
    }

    if (f4_idq_ref[0] > 0.0f)
    {
        f4_idq_ref[0] = 0.0f;
    }
    else
    {
        /* Do Nothing */
    }

    p_st_sc->f4_id_ref_output = f4_idq_ref[0];
    p_st_sc->f4_iq_ref_output = f4_idq_ref[1];
} /* End of function motor_speed_mtpa */
