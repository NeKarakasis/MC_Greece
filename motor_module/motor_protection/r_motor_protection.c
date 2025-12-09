#include "r_motor_protection.h"
#include <math.h>

/* ===========
 * Tables (2 motors) — built from the #defines in the header
 * =========== */
static const float    k_i_min_A[MP_NUM_MOTORS] =
{
    MP_I_MIN_A_MOTOR0,
    MP_I_MIN_A_MOTOR1
};

static const float    k_avg_cur_thr_A[MP_NUM_MOTORS] =
{
    MP_AVG_CUR_THR_A_MOTOR0,
    MP_AVG_CUR_THR_A_MOTOR1
};

static const uint32_t k_phl_cnt_max[MP_NUM_MOTORS] =
{
    MP_PHL_CNT_MAX_MOTOR0,
    MP_PHL_CNT_MAX_MOTOR1
};

static const float k_locked_rotor_time[MP_NUM_MOTORS] =
{
	MP_LOCKED_ROTOR_TIME_MOTOR0,
	MP_LOCKED_ROTOR_TIME_MOTOR1
};

static const float k_locked_rotor_theshold[MP_NUM_MOTORS] =
{
	MP_I2_LOCKED_A2_DEFAULT_MOTOR0,
	MP_I2_LOCKED_A2_DEFAULT_MOTOR1
};

/* Unitary Clarke (alpha = u, beta = (v - w)/sqrt(3)) */
static inline void clarke_unitary(float u, float v, float w, float *a, float *b)
{
    *a = u;
    *b = (v - w) * 0.5773502691896258f; /* 1/sqrt(3) */
}
void motor_protection_speed_init(motor_protection_t *mp, float Ts_slow_s)
{
	mp->Ts_slow_s = Ts_slow_s;
	motor_filter_first_order_lpff_init(&mp->lpf_filtered_iq_current);
	motor_filter_first_order_lpff_gain_calc(&mp->lpf_filtered_iq_current,
		2.0f * 3.14159265358979f * LOW_WATER_BW, Ts_slow_s);

	motor_filter_first_order_lpff_init(&mp->lpf_filtered_speed);
	motor_filter_first_order_lpff_gain_calc(&mp->lpf_filtered_speed,
		2.0f * 3.14159265358979f * LOW_WATER_BW, Ts_slow_s);
}

void motor_protection_init(motor_protection_t *mp, float Ts_fast_s)
{
    mp->Ts_fast_s = Ts_fast_s;

    /* --- LPFs --- */

    /* 0.73 Hz on sum(u+v+w) */
    motor_filter_first_order_lpff_init(&mp->lpf_sum_uvw);
    motor_filter_first_order_lpff_gain_calc(&mp->lpf_sum_uvw,
        2.0f * 3.14159265358979f * 0.73f, Ts_fast_s);
    mp->sum_uvw_f = 0.0f;

    /* 1.46 Hz on |u|, |v|, |w| */
    motor_filter_first_order_lpff_init(&mp->lpf_u_abs);
    motor_filter_first_order_lpff_init(&mp->lpf_v_abs);
    motor_filter_first_order_lpff_init(&mp->lpf_w_abs);
    {
        float omega_abs = 2.0f * 3.14159265358979f * 1.46f;
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_u_abs, omega_abs, Ts_fast_s);
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_v_abs, omega_abs, Ts_fast_s);
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_w_abs, omega_abs, Ts_fast_s);
    }
    mp->u_abs_f = 0.0f;
    mp->v_abs_f = 0.0f;
    mp->w_abs_f = 0.0f;

    /* 1.46 Hz on signed currents (for ±avg window) */
    motor_filter_first_order_lpff_init(&mp->lpf_u_sig);
    motor_filter_first_order_lpff_init(&mp->lpf_v_sig);
    motor_filter_first_order_lpff_init(&mp->lpf_w_sig);
    {
        float omega_sig = 2.0f * 3.14159265358979f * 1.46f;
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_u_sig, omega_sig, Ts_fast_s);
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_v_sig, omega_sig, Ts_fast_s);
        motor_filter_first_order_lpff_gain_calc(&mp->lpf_w_sig, omega_sig, Ts_fast_s);
    }
    mp->u_sig_f = 0.0f;
    mp->v_sig_f = 0.0f;
    mp->w_sig_f = 0.0f;

    /* 47 Hz on I^2 */
    motor_filter_first_order_lpff_init(&mp->lpf_I2_47Hz);
    motor_filter_first_order_lpff_gain_calc(&mp->lpf_I2_47Hz,
        2.0f * 3.14159265358979f * 47.0f, Ts_fast_s);
    mp->Ialpha = 0.0f;
    mp->Ibeta  = 0.0f;
    mp->I2_raw = 0.0f;
    mp->I2_f47 = 0.0f;

    /* --- Defaults (can be overridden) --- */
    mp->plaus_sum_abs_threshold = MP_PLAUS_THRESH_A_DEFAULT;
    mp->loss_threshold_ratio    = MP_LOSS_RATIO_DEFAULT;
    mp->unbalance_hold_time_s   = MP_UNBAL_HOLD_S_DEFAULT;

    mp->thr_running_overload_I2 = MP_I2_RUN_OVER_A2_DEFAULT;
    mp->thr_locked_rotor_I2     = MP_I2_LOCKED_A2_DEFAULT;
    mp->thr_current_max_I2      = MP_I2_CURR_MAX_A2_DEFAULT;

    mp->t_overload_s     = MP_T_OVERLOAD_S_DEFAULT;
    mp->t_locked_rotor_s = MP_T_LOCKED_S_DEFAULT;
    mp->t_current_max_s  = Ts_fast_s;     /* one PWM period */

    /* Table-selected values (set by motor_protection_select_motor) */
    mp->i_min_A         = 0.0f;
    mp->avg_cur_thr_A   = MP_SIGNED_CURRENT_AVARAGE;
    mp->phl_cnt_max     = 0u;

    /* Accumulators */
    mp->t_overload_acc_s    = 0.0f;
    mp->t_locked_acc_s      = 0.0f;
    mp->t_currentmax_acc_s  = 0.0f;
    mp->t_lossphase_acc_u_s = 0.0f;
    mp->t_lossphase_acc_v_s = 0.0f;
    mp->t_lossphase_acc_w_s = 0.0f;

    mp->phl_cnt    = 0u;
    mp->alarm_bits = MP_ALARM_NONE;
}

void motor_protection_select_motor(motor_protection_t *mp, uint8_t motor_index)
{
    uint8_t idx = motor_index;

    if (idx >= MP_NUM_MOTORS)
    {
        idx = (uint8_t)(MP_NUM_MOTORS - 1u);
    }

    mp->i_min_A       = k_i_min_A[idx];
    mp->avg_cur_thr_A = k_avg_cur_thr_A[idx];
    mp->phl_cnt_max   = k_phl_cnt_max[idx];
    mp->thr_locked_rotor_I2 = k_locked_rotor_theshold[idx];
    mp->t_locked_rotor_s = 	k_locked_rotor_time[idx];
}

void motor_protection_set_thresholds(motor_protection_t *mp,
                                     float plaus_sum_abs_threshold,
                                     float thr_running_overload_I2,
                                     float thr_locked_rotor_I2,
                                     float thr_current_max_I2,
                                     float t_locked_rotor_s,
                                     float unbalance_hold_time_s)
{
    mp->plaus_sum_abs_threshold = plaus_sum_abs_threshold;
    mp->thr_running_overload_I2 = thr_running_overload_I2;
    mp->thr_locked_rotor_I2     = thr_locked_rotor_I2;
    mp->thr_current_max_I2      = thr_current_max_I2;
    mp->t_locked_rotor_s        = t_locked_rotor_s;
    mp->unbalance_hold_time_s   = unbalance_hold_time_s;
}

void motor_protection_speed_update(motor_protection_t *mp, float speed, float iq_ref, float speed_ref)
{
	float speed_rpm;
	float speed_ref_rpm;
	float error_ratio;
	float abs_iq;
	float abs_speed;
	float abs_speed_ref;
	speed_rpm = speed * 60 / (2.0f * 3.14159265358979f);
	speed_ref_rpm = speed_ref * 60 / (2.0f * 3.14159265358979f);
	abs_iq =  fabsf(iq_ref);
	abs_speed = fabsf(speed_rpm);
	abs_speed_ref = fabsf(speed_ref_rpm);
	mp->f4_filtered_iq_current = motor_filter_first_order_lpff(&mp->lpf_filtered_iq_current, abs_iq);
	mp->f4_filtered_speed = motor_filter_first_order_lpff(&mp->lpf_filtered_speed, abs_speed);
	mp->iq_estimated = IQ_MODEL_K * abs_speed * abs_speed;
	// the error will be cleared only with reset
	if (mp->water_empty == 0)
		{
		// Avoid division by zero
		if (iq_ref == 0 || abs_speed_ref < SPEED_THRESHOLD)
		{
			mp->water_empty = 0;
			mp->water_empty_timer_s = 0;
		}
		else
		{
			// 2. Relative deviation
			error_ratio = fabsf((mp->iq_estimated - abs_iq)/ abs_iq);
			mp->UserVariable[5] = error_ratio;
			mp->UserVariable[6] = mp->iq_estimated;
			mp->UserVariable[7] = abs_iq;
			mp->UserVariable[8]++;


		}
		if (error_ratio > IQ_ERROR_THRESHOLD)
		{

			if (mp->water_empty_timer_s > WATER_EMPTY_DETECT_COUNT)
			{
				mp->water_empty = 1;
			}
			else
			{
				mp->water_empty_timer_s ++;
			}
		}
		else
		{
			if (mp->water_empty_timer_s > 0)
			{
				mp->water_empty_timer_s --;
			}
		}
	}
}


void motor_protection_update(motor_protection_t *mp, float u, float v, float w, float raw_u,float raw_v,float raw_w, float min_duty)
{
    /* 1) Plausibility: LPF(u+v+w) @ 0.73 Hz */ // todo add minumut duty case
	float sum_uvw = raw_u + raw_v + raw_w;
	if (min_duty > MP_MINIMUM_DUTY)
		{
			mp->sum_uvw_f = motor_filter_first_order_lpff(&mp->lpf_sum_uvw, sum_uvw);
			if (fabsf(mp->sum_uvw_f) > mp->plaus_sum_abs_threshold)
			{
				mp->alarm_bits |= MP_ALARM_PLAUSIBILITY;
			}
		}

    /* 2) Unbalance / Loss of phase
          - LPF(|u|,|v|,|w|) @ 1.46 Hz
          - LPF(signed u,v,w) @ 1.46 Hz
          - thr_unbal = max(0.75*avg(|i|_f), i_min_A)
          - Combined condition drives debounce counter (like legacy code) */
    {
        /* Absolute filters */
        mp->u_abs_f = motor_filter_first_order_lpff(&mp->lpf_u_abs, fabsf(u));
        mp->v_abs_f = motor_filter_first_order_lpff(&mp->lpf_v_abs, fabsf(v));
        mp->w_abs_f = motor_filter_first_order_lpff(&mp->lpf_w_abs, fabsf(w));

        /* Signed filters */
        mp->u_sig_f = motor_filter_first_order_lpff(&mp->lpf_u_sig, u);
        mp->v_sig_f = motor_filter_first_order_lpff(&mp->lpf_v_sig, v);
        mp->w_sig_f = motor_filter_first_order_lpff(&mp->lpf_w_sig, w);

        float sum_abs = mp->u_abs_f + mp->v_abs_f + mp->w_abs_f;
        float avg_abs = sum_abs * (1.0f / 3.0f);
        float thr_unbal = mp->loss_threshold_ratio * avg_abs;
        /* Condition A: any abs below threshold */
        bool bad_unbalance = false;

        if (mp->i_min_A < thr_unbal)
        {
            //thr_unbal = mp->i_min_A;
			if (mp->u_abs_f < thr_unbal || mp->v_abs_f < thr_unbal || mp->w_abs_f < thr_unbal)
			{
				bad_unbalance = true;
			}
        }
        /* Condition B: any signed magnitude exceeds ±avg_cur_thr_A */
        bool bad_signed = false;

        if (fabsf(mp->u_sig_f) > mp->avg_cur_thr_A ||
            fabsf(mp->v_sig_f) > mp->avg_cur_thr_A ||
            fabsf(mp->w_sig_f) > mp->avg_cur_thr_A)
        {
            bad_signed = true;
        }

        /* Debounce counter behavior (increment on bad, decrement on good) */
        if (bad_unbalance || bad_signed)
        {
            if (mp->phl_cnt < mp->phl_cnt_max)
            {
                mp->phl_cnt++;
            }
            else
            {
                mp->alarm_bits |= (MP_ALARM_LOSS_OF_PHASE | MP_ALARM_UNBALANCE);
            }
        }
        else
        {
            if (mp->phl_cnt > 0u)
            {
                mp->phl_cnt--;
            }
        }
    }

    /* 3) I^2 path: Clarke -> squared -> LPF(47 Hz) */
    {
        clarke_unitary(u, v, w, &mp->Ialpha, &mp->Ibeta);
        mp->I2_raw = (mp->Ialpha * mp->Ialpha) + (mp->Ibeta * mp->Ibeta);
        mp->I2_f47 = motor_filter_first_order_lpff(&mp->lpf_I2_47Hz, mp->I2_raw);
    }

    /* 4) Overcurrent classes (all use I2_f47 per your spec) */
    {
        /* Running Overload (120 s) */
        if (mp->I2_f47 > mp->thr_running_overload_I2)
        {
            mp->t_overload_acc_s += mp->Ts_fast_s;

            if (mp->t_overload_acc_s >= mp->t_overload_s)
            {
                mp->alarm_bits |= MP_ALARM_RUNNING_OVERLOAD;
            }
        }
        else
        {
            mp->t_overload_acc_s = 0.0f;
        }

        /* Locked Rotor (5 ms or 200 ms) */
        if (mp->I2_f47 > mp->thr_locked_rotor_I2)
        {
            mp->t_locked_acc_s += mp->Ts_fast_s;

            if (mp->t_locked_acc_s >= mp->t_locked_rotor_s)
            {
                mp->alarm_bits |= MP_ALARM_LOCKED_ROTOR;
            }
        }
        else
        {
            mp->t_locked_acc_s = 0.0f;
        }

        /* Current Max (persist for one PWM period) */
        if (mp->I2_f47 > mp->thr_current_max_I2)
        {
            mp->t_currentmax_acc_s += mp->Ts_fast_s;

            if (mp->t_currentmax_acc_s >= mp->t_current_max_s)
            {
                mp->alarm_bits |= MP_ALARM_CURRENT_MAX;
            }
        }
        else
        {
            mp->t_currentmax_acc_s = 0.0f;
        }
    }
}

void Protect_Init(motor_protection_t *mp, uint8_t motor_index)
{
    const float Ts_fast = 1.0f / MP_PERIOD;
    const float Ts_slow = 1.0f/ MP_SPEED_PERIOD;
    if (motor_index == 0) // it will run only for ciclulation motor
    {
    	motor_protection_speed_init(mp, Ts_slow);
    }
    motor_protection_init(mp, Ts_fast);
    motor_protection_select_motor(mp, motor_index);

    /* Optional overrides */
/*    motor_protection_set_thresholds(&g_mp,
        MP_PLAUS_THRESH_A_DEFAULT,
        MP_I2_RUN_OVER_A2_DEFAULT,
        MP_I2_LOCKED_A2_DEFAULT,
        MP_I2_CURR_MAX_A2_DEFAULT,
		locked_rotor[motor_index - 1],
        MP_UNBAL_HOLD_S_DEFAULT);*/
}

void motor_protection_enter_idle(motor_protection_t *mp, bool clear_alarms)
{
    mp->t_overload_acc_s     = 0.0f;
    mp->t_locked_acc_s       = 0.0f;
    mp->t_currentmax_acc_s   = 0.0f;
    mp->t_lossphase_acc_u_s  = 0.0f;
    mp->t_lossphase_acc_v_s  = 0.0f;
    mp->t_lossphase_acc_w_s  = 0.0f;
    mp->phl_cnt              = 0u;

    if (clear_alarms)
    {
        mp->alarm_bits = MP_ALARM_NONE;
    }
}
