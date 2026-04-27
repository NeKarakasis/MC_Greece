/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "rm_motor_tuner_rls.h"

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls_init
 * Description  : Initializes and reset dual input 2nd order RLS module
 * Arguments    : ini_p
 *                   - The multiplier to initialize diagonal parameter covariance matrix
 *                ini_k_foget
 *                   - The forgetting factor within (0, 1], usually 0.98 ~ 1.0, 1.0 means never forget
 * Return Value : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls_init (motor_tuner_rls_t * p_rls, float ini_p, float ini_k_foget)
{
    /* Initialization of RLS */
    p_rls->y         = 0;
    p_rls->u         = 0;
    p_rls->a         = 0;
    p_rls->b11       = 0;
    p_rls->b12       = 0;
    p_rls->b21       = 0;
    p_rls->b22       = 0;
    p_rls->p11       = ini_p;
    p_rls->p12       = 0;
    p_rls->p21       = 0;
    p_rls->p22       = ini_p;
    p_rls->y_pre     = 0;
    p_rls->u_pre     = 0;
    p_rls->p11_pre   = ini_p;
    p_rls->p12_pre   = 0;
    p_rls->p21_pre   = 0;
    p_rls->p22_pre   = ini_p;
    p_rls->k_forget  = ini_k_foget;
    p_rls->pu_1      = 0;
    p_rls->pu_2      = 0;
    p_rls->a_est     = 0;
    p_rls->b_est     = 0;
    p_rls->a_est_pre = 0;
    p_rls->b_est_pre = 0;
}

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls_exec
 * Description  : Executes single input 2nd order RLS module to estimate weight coefficients
 * Arguments    : u
 *                   - The first input, corresponding to the coefficient a,b
 *                y
 *                   - The measured output
 *                a_est
 *                   - Where to store the newest estimated coefficient a
 *                b_est
 *                   - Where to store the newest estimated coefficient b
 * Return Value : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls_exec (motor_tuner_rls_t * p_rls, float u, float y, float * a_est, float * b_est)
{
    /* RLS filter */
    p_rls->u = u;
    p_rls->y = y;

    p_rls->a = (p_rls->y_pre * p_rls->y_pre * p_rls->p11_pre) - (p_rls->y_pre * p_rls->u_pre * p_rls->p12_pre) -
               (p_rls->y_pre * p_rls->u_pre * p_rls->p21_pre) + (p_rls->u_pre * p_rls->u_pre * p_rls->p22_pre);
    p_rls->b11 = (p_rls->y_pre * p_rls->y_pre * p_rls->p11_pre * p_rls->p11_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p11_pre * p_rls->p12_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p11_pre * p_rls->p21_pre) +
                 (p_rls->u_pre * p_rls->u_pre * p_rls->p12_pre * p_rls->p21_pre);
    p_rls->b12 = (p_rls->y_pre * p_rls->y_pre * p_rls->p11_pre * p_rls->p12_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p12_pre * p_rls->p12_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p11_pre * p_rls->p22_pre) +
                 (p_rls->u_pre * p_rls->u_pre * p_rls->p12_pre * p_rls->p22_pre);
    p_rls->b21 = (p_rls->y_pre * p_rls->y_pre * p_rls->p11_pre * p_rls->p21_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p11_pre * p_rls->p22_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p21_pre * p_rls->p21_pre) +
                 (p_rls->u_pre * p_rls->u_pre * p_rls->p21_pre * p_rls->p22_pre);
    p_rls->b22 = (p_rls->y_pre * p_rls->y_pre * p_rls->p12_pre * p_rls->p21_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p12_pre * p_rls->p22_pre) -
                 (p_rls->y_pre * p_rls->u_pre * p_rls->p21_pre * p_rls->p22_pre) +
                 (p_rls->u_pre * p_rls->u_pre * p_rls->p22_pre * p_rls->p22_pre);
    p_rls->p11 = (p_rls->p11_pre - (p_rls->b11 / (p_rls->k_forget + p_rls->a))) / p_rls->k_forget;
    p_rls->p12 = (p_rls->p12_pre - (p_rls->b12 / (p_rls->k_forget + p_rls->a))) / p_rls->k_forget;
    p_rls->p21 = (p_rls->p21_pre - (p_rls->b21 / (p_rls->k_forget + p_rls->a))) / p_rls->k_forget;
    p_rls->p22 = (p_rls->p22_pre - (p_rls->b22 / (p_rls->k_forget + p_rls->a))) / p_rls->k_forget;

    p_rls->pu_1 = ((-p_rls->y_pre) * p_rls->p11_pre) + (p_rls->u_pre * p_rls->p12_pre);
    p_rls->pu_2 = ((-p_rls->y_pre) * p_rls->p21_pre) + (p_rls->u_pre * p_rls->p22_pre);

    p_rls->y_est = (p_rls->u_pre * p_rls->b_est_pre) - (p_rls->y_pre * p_rls->a_est_pre);
    p_rls->a_est = p_rls->a_est_pre + ((p_rls->pu_1 / (p_rls->k_forget + p_rls->a)) *
                                       ((p_rls->y + (p_rls->y_pre * p_rls->a_est_pre)) -
                                        (p_rls->u_pre * p_rls->b_est_pre)));
    p_rls->b_est = p_rls->b_est_pre + ((p_rls->pu_2 / (p_rls->k_forget + p_rls->a)) *
                                       ((p_rls->y + (p_rls->y_pre * p_rls->a_est_pre)) -
                                        (p_rls->u_pre * p_rls->b_est_pre)));

    p_rls->u_pre = p_rls->u;
    p_rls->y_pre = p_rls->y;

    p_rls->p11_pre   = p_rls->p11;
    p_rls->p12_pre   = p_rls->p12;
    p_rls->p21_pre   = p_rls->p21;
    p_rls->p22_pre   = p_rls->p22;
    p_rls->a_est_pre = p_rls->a_est;
    p_rls->b_est_pre = p_rls->b_est;

    *a_est = p_rls->a_est;
    *b_est = p_rls->b_est;
}

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls2_init
 * Description  : Does an example task. Making this longer just to see how it wraps. Making this long just to see how it
 *                wraps.
 * Arguments    : index -
 *                    Where to start looking
 *                p_output -
 *                    Pointer of where to put the output data
 * Return Value : count -
 *                    How many entries were found
 **********************************************************************************************************************/
void rm_motor_tuner_rls2_init (motor_tuner_rls_t * p_rls,
                               float               ini_p_a,
                               float               int_p_b,
                               float               ini_k_foget,
                               float               a_init,
                               float               b_init)
{
    /* Initialization of RLS */
    p_rls->y         = 0;
    p_rls->u         = 0;
    p_rls->v         = 0;
    p_rls->a         = 0;
    p_rls->b11       = 0;
    p_rls->b12       = 0;
    p_rls->b21       = 0;
    p_rls->b22       = 0;
    p_rls->p11       = ini_p_a;
    p_rls->p12       = 0;
    p_rls->p21       = 0;
    p_rls->p22       = int_p_b;
    p_rls->y_pre     = 0;
    p_rls->u_pre     = 0;
    p_rls->p11_pre   = ini_p_a;
    p_rls->p12_pre   = 0;
    p_rls->p21_pre   = 0;
    p_rls->p22_pre   = int_p_b;
    p_rls->k_forget  = ini_k_foget;
    p_rls->pu_1      = 0;
    p_rls->pu_2      = 0;
    p_rls->a_est     = 0;
    p_rls->b_est     = 0;
    p_rls->a_est_pre = a_init;
    p_rls->b_est_pre = b_init;
}

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls2_exec
 * Description  : Executes dual input 2nd order RLS module to estimate weight coefficients
 * Arguments    : a_in
 *                   - The 1st input, corresponding to the coefficient a
 *                b_in
 *                   - The 2nd input, corresponding to the coefficient b
 *                y
 *                   - The measured output
 *                a_est
 *                   - Where to store the newest estimated weight coefficient a
 *                b_est
 *                   - Where to store the newest estimated weight coefficient b
 * Return Value : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls2_exec (motor_tuner_rls_t * p_rls, float a_in, float b_in, float y, float * a_est, float * b_est)
{
    float upu;
    float up_1;
    float up_2;
    float temp;
    float one_div_fgt;

    /* RLS identification
     * Input vector                     : u(n) = [a_in; b_in]
     * Desired output(measured output)  : d(n) = p_rls->y
     * Estimated output                 : y(n) = p_rls->y_est
     * See https://jp.mathworks.com/help/dsp/ref/dsp.rlsfilter-system-object.html for detailed algorithm
     * */
    p_rls->v = a_in;
    p_rls->u = b_in;

    p_rls->y = y;

    /* Pu(n) = P(n - 1) * u(n) */
    p_rls->pu_1 = (p_rls->p11 * p_rls->v) + (p_rls->p12 * p_rls->u);
    p_rls->pu_2 = (p_rls->p21 * p_rls->v) + (p_rls->p22 * p_rls->u);

    /* uPu(n) = u^H(n) * Pu(n), u(n) is all real number, Hermitian transpose = regular transpose  */
    upu = (p_rls->v * p_rls->pu_1) + (p_rls->u * p_rls->pu_2);

    /* k(n) = Pu(n) / (forgetting factor + uPu(n)) , temp = 1 / (forgetting factor + uPu(n))*/
    temp      = 1.0F / (p_rls->k_forget + upu);
    p_rls->k1 = p_rls->pu_1 * temp;
    p_rls->k2 = p_rls->pu_2 * temp;

    /* y(n) = w^T(n - 1) * u(n) */
    p_rls->y_est = (p_rls->v * p_rls->a_est_pre) + (p_rls->u * p_rls->b_est_pre);

    /* e(n) = d(n) - y(n) */
    p_rls->err_est = p_rls->y - p_rls->y_est;

    /* w(n) = w(n-1) + k(n)  * (d(n) - y(n)) */
    p_rls->a_est = p_rls->a_est_pre + (p_rls->k1 * p_rls->err_est);
    p_rls->b_est = p_rls->b_est_pre + (p_rls->k2 * p_rls->err_est);

    /* P(n) = (1 / forgetting factor) * (P(n-1) - k(n) * u^H(n) * P(n-1)) */
    one_div_fgt = 1.0F / p_rls->k_forget;
    up_1        = (p_rls->v * p_rls->p11) + (p_rls->u * p_rls->p21);
    up_2        = (p_rls->v * p_rls->p12) + (p_rls->u * p_rls->p22);

    p_rls->p11 = one_div_fgt * (p_rls->p11 - (p_rls->k1 * up_1));
    p_rls->p12 = one_div_fgt * (p_rls->p12 - (p_rls->k1 * up_2));
    p_rls->p21 = one_div_fgt * (p_rls->p21 - (p_rls->k2 * up_1));
    p_rls->p22 = one_div_fgt * (p_rls->p22 - (p_rls->k2 * up_2));

    p_rls->u_pre = p_rls->u;

    p_rls->a_est_pre = p_rls->a_est;
    p_rls->b_est_pre = p_rls->b_est;

    *a_est = p_rls->a_est;
    *b_est = p_rls->b_est;
}

/***********************************************************************************************************************
 * Function Name : rm_motor_tuner_rls_set_init_ab
 * Description   : Set the initial weight coefficient of RLS module
 * Arguments     : a_init - Initial value of coefficient a
 *                 b_init - Initial value of coefficient b
 * Return Value  : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls_set_init_ab (motor_tuner_rls_t * p_rls, float a_init, float b_init)
{
    p_rls->a_est_pre = a_init;
    p_rls->b_est_pre = b_init;
}

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls3_init
 * Description  : Executes triple input 3nd order RLS module to estimate weight coefficients
 * Arguments    : a_in
 *                   - The 1st input, corresponding to the coefficient a
 *                b_in
 *                   - The 2nd input, corresponding to the coefficient b
 *                y
 *                   - The measured output
 *                a_est
 *                   - Where to store the newest estimated weight coefficient a
 *                b_est
 *                   - Where to store the newest estimated weight coefficient b
 * Return Value : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls3_init (motor_tuner_rls3_t * rls3,
                               float                ini_p_1,
                               float                int_p_2,
                               float                int_p_3,
                               float                ini_k_forget,
                               float                in_1_init,
                               float                in_2_init,
                               float                in_3_init)
{
    rls3->p[0][0] = ini_p_1;
    rls3->p[0][1] = 0.0F;
    rls3->p[0][2] = 0.0F;
    rls3->p[1][0] = 0.0F;
    rls3->p[1][1] = int_p_2;
    rls3->p[1][2] = 0.0F;
    rls3->p[2][0] = 0.0F;
    rls3->p[2][1] = 0.0F;
    rls3->p[2][2] = int_p_3;

    rls3->y_est    = 0.0F;
    rls3->err      = 0.0F;
    rls3->k[0]     = 0.0F;
    rls3->k[1]     = 0.0F;
    rls3->k[2]     = 0.0F;
    rls3->w[0]     = in_1_init;
    rls3->w[1]     = in_2_init;
    rls3->w[2]     = in_3_init;
    rls3->k_forget = ini_k_forget;
}

/***********************************************************************************************************************
 * Function Name: rm_motor_tuner_rls3_exec
 * Description  : Executes dual input 2nd order RLS module to estimate weight coefficients
 * Arguments    : a_in
 *                   - The 1st input, corresponding to the coefficient a
 *                b_in
 *                   - The 2nd input, corresponding to the coefficient b
 *                y
 *                   - The measured output
 *                a_est
 *                   - Where to store the newest estimated weight coefficient a
 *                b_est
 *                   - Where to store the newest estimated weight coefficient b
 * Return Value : None
 ***********************************************************************************************************************/
void rm_motor_tuner_rls3_exec (motor_tuner_rls3_t * rls3, float * in, float y, float * weight)
{
    float in_1;
    float in_2;
    float in_3;
    float upu;
    float up_1;
    float up_2;
    float up_3;
    float temp;
    float one_div_fgt;
    float pu_1;
    float pu_2;
    float pu_3;

    /* RLS identification
     * Input vector                     : u(n) = in
     * Desired output(measured output)  : d(n) = y
     * Estimated output                 : y(n) = rls3->y_est
     * See https://jp.mathworks.com/help/dsp/ref/dsp.rlsfilter-system-object.html for detailed algorithm
     * */
    in_1 = in[0];
    in_2 = in[1];
    in_3 = in[2];

    /* Pu(n) = P(n - 1) * u(n) */
    pu_1 = (rls3->p[0][0] * in_1) + (rls3->p[0][1] * in_2) + (rls3->p[0][2] * in_3);
    pu_2 = (rls3->p[1][0] * in_1) + (rls3->p[1][1] * in_2) + (rls3->p[1][2] * in_3);
    pu_3 = (rls3->p[2][0] * in_1) + (rls3->p[2][1] * in_2) + (rls3->p[2][2] * in_3);

    /* uPu(n) = u^H(n) * Pu(n), u(n) is all real number, Hermitian transpose = regular transpose  */
    upu = (in_1 * pu_1) + (in_2 * pu_2) + (in_3 * pu_3);

    /* k(n) = Pu(n) / (forgetting factor + uPu(n)) , temp = 1 / (forgetting factor + uPu(n))*/
    temp       = 1.0F / (rls3->k_forget + upu);
    rls3->k[0] = pu_1 * temp;
    rls3->k[1] = pu_2 * temp;
    rls3->k[2] = pu_3 * temp;

    /* y(n) = w^T(n - 1) * u(n) */
    rls3->y_est = (rls3->w[0] * in[0]) + (rls3->w[1] * in[1]) + (rls3->w[2] * in[2]);

    /* e(n) = d(n) - y(n) */
    rls3->err = y - rls3->y_est;

    /* w(n) = w(n-1) + k(n)  * (d(n) - y(n)) */
    rls3->w[0] = rls3->w[0] + (rls3->k[0] * rls3->err);
    rls3->w[1] = rls3->w[1] + (rls3->k[1] * rls3->err);
    rls3->w[2] = rls3->w[2] + (rls3->k[2] * rls3->err);

    /* P(n) = (1 / forgetting factor) * (P(n-1) - k(n) * u^H(n) * P(n-1)) */
    one_div_fgt = 1.0F / rls3->k_forget;
    up_1        = (in_1 * rls3->p[0][0]) + (in_2 * rls3->p[1][0]) + (in_3 * rls3->p[2][0]);
    up_2        = (in_1 * rls3->p[0][1]) + (in_2 * rls3->p[1][1]) + (in_3 * rls3->p[2][1]);
    up_3        = (in_1 * rls3->p[0][2]) + (in_2 * rls3->p[1][2]) + (in_3 * rls3->p[2][2]);

    rls3->p[0][0] = one_div_fgt * (rls3->p[0][0] - (rls3->k[0] * up_1));
    rls3->p[0][1] = one_div_fgt * (rls3->p[0][1] - (rls3->k[0] * up_2));
    rls3->p[0][2] = one_div_fgt * (rls3->p[0][2] - (rls3->k[0] * up_3));
    rls3->p[1][0] = one_div_fgt * (rls3->p[1][0] - (rls3->k[1] * up_1));
    rls3->p[1][1] = one_div_fgt * (rls3->p[1][1] - (rls3->k[1] * up_2));
    rls3->p[1][2] = one_div_fgt * (rls3->p[1][2] - (rls3->k[1] * up_3));
    rls3->p[2][0] = one_div_fgt * (rls3->p[2][0] - (rls3->k[2] * up_1));
    rls3->p[2][1] = one_div_fgt * (rls3->p[2][1] - (rls3->k[2] * up_2));
    rls3->p[2][2] = one_div_fgt * (rls3->p[2][2] - (rls3->k[2] * up_3));

    /* Output to the specified address */
    weight[0] = rls3->w[0];
    weight[1] = rls3->w[1];
    weight[2] = rls3->w[2];
}
