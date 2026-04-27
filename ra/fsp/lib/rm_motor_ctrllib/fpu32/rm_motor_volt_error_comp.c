/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdint.h>
#include "rm_motor_volt_error_comp.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define MTR_VOLT_COMP_LIMIT          (20.0F * 2.0F / 1000.0F) ///< Compensation limit voltage factor, PWM carrier 20kHz * deadtime 2us / 1000Hz = 0.04 -> 4% duty
#define MTR_VOLT_ERR_COMP_ENABLE     (1)
#define MTR_VOLT_COMP_REF_VOLTAGE    (24.0F)

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief Calculates compensation voltage from three-phase motor currents.
 *
 * This function computes the compensation voltage for each of the U, V, and W motor phases based on the input
 * current array and the provided voltage compensation structure. It calculates the absolute values and signs
 * of the phase currents, determines the appropriate compensation index using linear interpolation, applies
 * scaling by the DC bus voltage, and limits the compensation voltage as necessary.
 *
 * @param[in]  p_f4_i_array  Pointer to array of three-phase motor currents (U, V, W).
 * @param[in]  st_volt_comp  Pointer to the voltage error compensation structure.
 *
 * @return None
 ***********************************************************************************************************************/
inline void rm_motor_volt_error_comp_calc (float * p_f4_i_array, motor_volt_error_comp_t * st_volt_comp)
{
    float   f4_i_sign[3];
    float   f4_i_vec[3];
    float   f4_temp[3];
    uint8_t index;

    /* Calculate absolute value and direction of the u phase current */
    if (p_f4_i_array[0] >= 0)
    {
        f4_i_sign[0] = 1.0F;
        f4_i_vec[0]  = p_f4_i_array[0];
    }
    else
    {
        f4_i_sign[0] = -1.0F;
        f4_i_vec[0]  = -p_f4_i_array[0];
    }

    /* Calculate absolute value and direction of the v phase current */
    if (p_f4_i_array[1] >= 0)
    {
        f4_i_sign[1] = 1.0F;
        f4_i_vec[1]  = p_f4_i_array[1];
    }
    else
    {
        f4_i_sign[1] = -1.0F;
        f4_i_vec[1]  = -p_f4_i_array[1];
    }

    /* Calculate absolute value and direction of the w phase current */
    if (p_f4_i_array[2] >= 0)
    {
        f4_i_sign[2] = 1.0F;
        f4_i_vec[2]  = p_f4_i_array[2];
    }
    else
    {
        f4_i_sign[2] = -1.0F;
        f4_i_vec[2]  = -p_f4_i_array[2];
    }

    /*============================================*/
    /*  U phase compensation voltage calculation  */
    /*============================================*/
    if (f4_i_vec[0] < st_volt_comp->f_comp_i[2])
    {
        if (f4_i_vec[0] >= st_volt_comp->f_comp_i[1])
        {
            index = 2;
        }
        else if (f4_i_vec[0] >= st_volt_comp->f_comp_i[0])
        {
            index = 1;
        }
        else
        {
            index = 0;
        }
    }
    else
    {
        if (f4_i_vec[0] <= st_volt_comp->f_comp_i[3])
        {
            index = 3;
        }
        else if (f4_i_vec[0] <= st_volt_comp->f_comp_i[4])
        {
            index = 4;
        }
        else
        {
            index = 5;
        }
    }

    f4_temp[0] = ((f4_i_vec[0] * st_volt_comp->f_slope[index]) + st_volt_comp->f_intcept[index]);

    /* Scale the table by DC bus voltage  */
    f4_temp[0] = (st_volt_comp->f_vdc * f4_temp[0]) * (1.0F / st_volt_comp->f_comp_vdc_ref);

    if (f4_temp[0] > (st_volt_comp->f_volt_comp_limit))
    {
        f4_temp[0] = st_volt_comp->f_volt_comp_limit;
    }

    st_volt_comp->f_volt_comp_array[0] = f4_i_sign[0] * f4_temp[0];

    /*============================================*/
    /*  V phase compensation voltage calculation  */
    /*============================================*/
    if (f4_i_vec[1] < st_volt_comp->f_comp_i[2])
    {
        if (f4_i_vec[1] >= st_volt_comp->f_comp_i[1])
        {
            index = 2;
        }
        else if (f4_i_vec[1] >= st_volt_comp->f_comp_i[0])
        {
            index = 1;
        }
        else
        {
            index = 0;
        }
    }
    else
    {
        if (f4_i_vec[1] <= st_volt_comp->f_comp_i[3])
        {
            index = 3;
        }
        else if (f4_i_vec[1] <= st_volt_comp->f_comp_i[4])
        {
            index = 4;
        }
        else
        {
            index = 5;
        }
    }

    f4_temp[1] = ((f4_i_vec[1] * st_volt_comp->f_slope[index]) + st_volt_comp->f_intcept[index]);

    /* Scale the table by DC bus voltage  */
    f4_temp[1] = (st_volt_comp->f_vdc * f4_temp[1]) * (1.0F / st_volt_comp->f_comp_vdc_ref);

    if (f4_temp[1] > (st_volt_comp->f_volt_comp_limit))
    {
        f4_temp[1] = st_volt_comp->f_volt_comp_limit;
    }

    st_volt_comp->f_volt_comp_array[1] = f4_i_sign[1] * f4_temp[1];

    /*============================================*/
    /*  W phase compensation voltage calculation  */
    /*============================================*/
    if (f4_i_vec[2] < st_volt_comp->f_comp_i[2])
    {
        if (f4_i_vec[2] >= st_volt_comp->f_comp_i[1])
        {
            index = 2;
        }
        else if (f4_i_vec[2] >= st_volt_comp->f_comp_i[0])
        {
            index = 1;
        }
        else
        {
            index = 0;
        }
    }
    else
    {
        if (f4_i_vec[2] <= st_volt_comp->f_comp_i[3])
        {
            index = 3;
        }
        else if (f4_i_vec[2] <= st_volt_comp->f_comp_i[4])
        {
            index = 4;
        }
        else
        {
            index = 5;
        }
    }

    f4_temp[2] = ((f4_i_vec[2] * st_volt_comp->f_slope[index]) + st_volt_comp->f_intcept[index]);

    /*  Scale the table by DC bus voltage  */
    f4_temp[2] = (st_volt_comp->f_vdc * f4_temp[2]) * (1.0F / st_volt_comp->f_comp_vdc_ref);

    if (f4_temp[2] > (st_volt_comp->f_volt_comp_limit))
    {
        f4_temp[2] = st_volt_comp->f_volt_comp_limit;
    }

    st_volt_comp->f_volt_comp_array[2] = f4_i_sign[2] * f4_temp[2];
}                                      /* End of function rm_motor_volt_comp_calc */

/***********************************************************************************************************************
 * @brief Initializes the voltage error compensation module.
 *
 * This function initializes the provided voltage compensation structure by resetting the compensation voltage array
 * to zero for all three phases. It also sets up the linear interpolation table for voltage compensation using the
 * current and voltage error reference tables. For each table entry, the slope and intercept are calculated to be
 * used in later compensation calculations. The function ensures the voltage compensation structure is properly
 * configured for subsequent compensation operations.
 *
 * @param[in,out] st_volt_comp Pointer to the voltage error compensation structure.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_volt_error_comp_init (motor_volt_error_comp_t * st_volt_comp)
{
    uint8_t i;
    float   f4_temp0;
    float   f4_temp1;

    for (i = 0; i < 3; i++)
    {
        st_volt_comp->f_volt_comp_array[i] = 0.0F;
    }

    /* clear vdc reference for compensation table*/
    st_volt_comp->f_comp_vdc_ref = MTR_VOLT_COMP_REF_VOLTAGE;

    /* set compensation limit factor */
    st_volt_comp->f_comp_limit_factor = MTR_VOLT_COMP_LIMIT;

    /* linear interpolation table */
    for (i = 0; i < (MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE + 1); i++)
    {
        if (0 == i)
        {
            f4_temp0                   = st_volt_comp->f_comp_v[i];
            f4_temp1                   = st_volt_comp->f_comp_i[i];
            st_volt_comp->f_slope[i]   = f4_temp0 / f4_temp1;
            st_volt_comp->f_intcept[i] = 0.0F;
        }
        else if (MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE == i)
        {
            st_volt_comp->f_slope[i]   = 0.0F;
            st_volt_comp->f_intcept[i] =
                st_volt_comp->f_comp_v[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE - 1];
        }
        else
        {
            f4_temp0                   = st_volt_comp->f_comp_v[i] - st_volt_comp->f_comp_v[i - 1];
            f4_temp1                   = st_volt_comp->f_comp_i[i] - st_volt_comp->f_comp_i[i - 1];
            st_volt_comp->f_slope[i]   = f4_temp0 / f4_temp1;
            st_volt_comp->f_intcept[i] =
                st_volt_comp->f_comp_v[i] - (st_volt_comp->f_slope[i] * st_volt_comp->f_comp_i[i]);
        }
    }
}                                      /* End of function rm_motor_voltage_error_compensation_init */

/***********************************************************************************************************************
 * @brief Resets the compensation voltage error values to zero for all three phases.
 *
 * This function sets all elements of the compensation voltage array within the provided
 * voltage compensation structure to zero, effectively clearing any previously calculated
 * compensation voltages. It is typically used to initialize or reset the voltage error
 * compensation state before starting new calculations.
 *
 * @param[in,out] st_volt_comp Pointer to the voltage error compensation structure.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_volt_error_comp_reset (motor_volt_error_comp_t * st_volt_comp)
{
    uint8_t i;

    for (i = 0; i < 3; i++)
    {
        st_volt_comp->f_volt_comp_array[i] = 0.0F;
    }
}                                      /* End of function rm_motor_voltage_error_compensation_reset */

/***********************************************************************************************************************
 * @brief Main function for voltage error compensation.
 *
 * This function applies voltage error compensation to the three-phase motor voltage array
 * based on the provided current array and inverter bus voltage. If compensation is enabled,
 * it updates the internal VDC value, calculates the compensation voltage limit, computes
 * the compensation voltages for each phase, and adds the calculated compensation to the
 * corresponding elements in the voltage array. If compensation is not enabled, the
 * compensation voltage array is reset to zero.
 *
 * @param[in,out] st_volt_comp Pointer to the voltage error compensation structure.
 * @param[in,out] p_f4_v_array Pointer to the three-phase voltage array to be compensated.
 * @param[in]     p_f4_i_array Pointer to the three-phase current array.
 * @param[in]     f4_vdc       Inverter bus voltage.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_volt_error_comp_main (motor_volt_error_comp_t * st_volt_comp,
                                    float                   * p_f4_v_array,
                                    float                   * p_f4_i_array,
                                    float                     f4_vdc)
{
    if (MTR_VOLT_ERR_COMP_ENABLE == st_volt_comp->u1_volt_err_comp_enable)
    {
        st_volt_comp->f_vdc = f4_vdc;

        /*===============================================*/
        /* Calculate compensation voltage limit          */
        /*===============================================*/
        st_volt_comp->f_volt_comp_limit = st_volt_comp->f_comp_limit_factor * st_volt_comp->f_vdc;

        /*===============================================*/
        /*      Calculate compensation voltage           */
        /*===============================================*/
        rm_motor_volt_error_comp_calc(p_f4_i_array, st_volt_comp);

        p_f4_v_array[0] += st_volt_comp->f_volt_comp_array[0];
        p_f4_v_array[1] += st_volt_comp->f_volt_comp_array[1];
        p_f4_v_array[2] += st_volt_comp->f_volt_comp_array[2];
    }
    else
    {
        st_volt_comp->f_volt_comp_array[0] = 0.0F;
        st_volt_comp->f_volt_comp_array[1] = 0.0F;
        st_volt_comp->f_volt_comp_array[2] = 0.0F;
    }
}                                      /* End of function rm_motor_voltage_error_compensation_main */

/***********************************************************************************************************************
 * @brief Sets up the voltage error compensation module with a given voltage error table.
 *
 * This function initializes the voltage compensation structure with specified current and voltage error tables.
 * It normalizes the voltage error table by the reference DC bus voltage, sets up the current table, and resets
 * the compensation voltage array. It then computes the linear interpolation table (slope and intercept arrays)
 * for later compensation calculations. This function configures the lookup tables required for voltage error
 * compensation operations.
 *
 * @param[in,out] st_volt_comp      Pointer to the voltage error compensation structure.
 * @param[in]     f4_current_table  Pointer to the current table array.
 * @param[in]     f4_volterr_table  Pointer to the voltage error table array.
 * @param[in]     f4_ref_vdc        Reference DC bus voltage.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_volt_error_comp_set_table (motor_volt_error_comp_t * st_volt_comp,
                                         const float             * f4_current_table,
                                         const float             * f4_volterr_table,
                                         float                     f4_ref_vdc)
{
    uint8_t i;
    float   f4_temp0;
    float   f4_temp1;

    for (i = 0; i < MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE; i++)
    {
        st_volt_comp->f_comp_v[i] = f4_volterr_table[i] / f4_ref_vdc;
        st_volt_comp->f_comp_i[i] = f4_current_table[i];
    }

    st_volt_comp->f_comp_vdc_ref = 1.0f;

    for (i = 0; i < 3; i++)
    {
        st_volt_comp->f_volt_comp_array[i] = 0.0f;
    }

    /* linear interpolation table */
    for (i = 0; i < (MOTOR_VOLT_ERR_COMP_SLOPE_ARRAY_SIZE); i++)
    {
        if (0 == i)
        {
            f4_temp0                   = st_volt_comp->f_comp_v[i];
            f4_temp1                   = st_volt_comp->f_comp_i[i];
            st_volt_comp->f_slope[i]   = f4_temp0 / f4_temp1;
            st_volt_comp->f_intcept[i] = 0.0f;
        }
        else if (MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE == i)
        {
            st_volt_comp->f_slope[i]   = 0.0f;
            st_volt_comp->f_intcept[i] = st_volt_comp->f_comp_v[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE - 1];
        }
        else
        {
            f4_temp0                   = st_volt_comp->f_comp_v[i] - st_volt_comp->f_comp_v[i - 1];
            f4_temp1                   = st_volt_comp->f_comp_i[i] - st_volt_comp->f_comp_i[i - 1];
            st_volt_comp->f_slope[i]   = f4_temp0 / f4_temp1;
            st_volt_comp->f_intcept[i] =
                st_volt_comp->f_comp_v[i] - (st_volt_comp->f_slope[i] * st_volt_comp->f_comp_i[i]);
        }
    }
}                                      /* End of function rm_motor_volt_error_comp_set_table */
