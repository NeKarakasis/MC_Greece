/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_VOLT_ERR_COMP_LIBRARY_H
#define RM_MOTOR_VOLT_ERR_COMP_LIBRARY_H

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE    (5)
#define MOTOR_VOLT_ERR_COMP_SLOPE_ARRAY_SIZE    (6)
#define MOTOR_VOLT_ERR_COMP_PHASE_ARRAY_SIZE    (3)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef enum  e_motor_volt_error_comp_select
{
    MOTOR_CURRENT_VOLTAGE_COMPENSATION_SELECT_DISABLE = 0,
    MOTOR_CURRENT_VOLTAGE_COMPENSATION_SELECT_ENABLE  = 1
} motor_volt_error_comp_select_t;

typedef struct st_motor_volt_error_comp
{
    float f_comp_v[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE];
    float f_comp_i[MOTOR_VOLT_ERR_COMP_TABLE_ARRAY_SIZE];
    float f_slope[MOTOR_VOLT_ERR_COMP_SLOPE_ARRAY_SIZE];
    float f_intcept[MOTOR_VOLT_ERR_COMP_SLOPE_ARRAY_SIZE];
    float f_volt_comp_array[MOTOR_VOLT_ERR_COMP_PHASE_ARRAY_SIZE];
    float f_vdc;
    float f_comp_limit_factor;
    float f_volt_comp_limit;
    float f_comp_vdc_ref;
    motor_volt_error_comp_select_t u1_volt_err_comp_enable;
} motor_volt_error_comp_t;

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/
void rm_motor_volt_error_comp_calc(float * p_f4_i_array, motor_volt_error_comp_t * st_volt_comp);
void rm_motor_volt_error_comp_init(motor_volt_error_comp_t * st_volt_comp);
void rm_motor_volt_error_comp_reset(motor_volt_error_comp_t * st_volt_comp);
void rm_motor_volt_error_comp_main(motor_volt_error_comp_t * st_volt_comp,
                                   float                   * p_f4_v_array,
                                   float                   * p_f4_i_array,
                                   float                     f4_vdc);
void rm_motor_volt_error_comp_set_table(motor_volt_error_comp_t * st_volt_comp,
                                        const float             * f4_current_table,
                                        const float             * f4_volterr_table,
                                        float                     f4_ref_vdc);

#endif                                 /* RM_MOTOR_VOLT_ERR_COMP_LIBRARY_H */
