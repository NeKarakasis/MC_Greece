#ifndef R_MOTOR_PROTECTION_H
#define R_MOTOR_PROTECTION_H

#include <stdint.h>
#include <stdbool.h>
#include "r_motor_filter.h"

/* IN OUR CASE, MOTOR0 -> CIRCULATION MOTOR, MOTOR1 -> DRY MOTOR */

/* =========================
 * Configuration tables (2 motors) — EDIT THESE VALUES
 * ========================= */
#define MP_NUM_MOTORS                (2u)

/* The period of motor protection (Hz) */
#define MP_PERIOD					(6000.f)
#define	MP_SPEED_PERIOD				(1000.f)

/* locked rotor time (s) */
#define MP_LOCKED_ROTOR_TIME_MOTOR0 (0.05f)
#define MP_LOCKED_ROTOR_TIME_MOTOR1 (0.2f)

/* Minimum current clamp for unbalance threshold (A) */
#define MP_I_MIN_A_MOTOR0           (0.05f)
#define MP_I_MIN_A_MOTOR1           (0.01f)

/* Signed average current window (A): |i_filt| must be <= this */
#define MP_AVG_CUR_THR_A_MOTOR0     (0.1f)
#define MP_AVG_CUR_THR_A_MOTOR1     (0.1f)

/* Debounce max count for loss/unbalance combined condition (samples @ 6 kHz) */
#define MP_PHL_CNT_MAX_MOTOR0       (90000u)   /* 1.5sec at 6 kHz */
#define MP_PHL_CNT_MAX_MOTOR1       (90000u)

/* Other defaults (you can override at runtime if needed) */
#define MP_PLAUS_THRESH_A_DEFAULT   (0.1f)  /* |LPF(u+v+w)| alarm threshold (A) */
#define MP_UNBAL_HOLD_S_DEFAULT     (0.50f)  /* seconds; persistence time */
#define MP_LOSS_RATIO_DEFAULT       (0.75f)  /* 75% of average abs */
#define MP_SIGNED_CURRENT_AVARAGE   (0.100f) /* the maximum avarage singed value of current waveform */

/* I^2 thresholds (A^2) — tune to your machine or override via setter */
#define MP_I2_RUN_OVER_A2_DEFAULT_MOTOR0   (0.7f)  /* Running Overload */
#define MP_I2_LOCKED_A2_DEFAULT_MOTOR0     (0.8f)  /* Locked Rotor */

#define MP_I2_RUN_OVER_A2_DEFAULT_MOTOR1   (0.5f)  /* Running Overload */
#define MP_I2_LOCKED_A2_DEFAULT_MOTOR1     (0.6f)  /* Locked Rotor */

#define MP_I2_CURR_MAX_A2_DEFAULT   (1.44f)  /* Current Max */

#define MP_I2_LOCKED_A2_DEFAULT		(MP_I2_LOCKED_A2_DEFAULT_MOTOR0 * MP_I2_LOCKED_A2_DEFAULT_MOTOR0)
#define MP_I2_RUN_OVER_A2_DEFAULT	(MP_I2_RUN_OVER_A2_DEFAULT_MOTOR0 * MP_I2_RUN_OVER_A2_DEFAULT_MOTOR0)

/* Class timings */
#define MP_T_OVERLOAD_S_DEFAULT     (120.0f)
#define MP_T_LOCKED_S_DEFAULT       (0.005f)  /* set to 0.2f for drain pump */

#define MP_MINIMUM_DUTY				(0.2f)   /* persentage of minimum duty %, DEADTIME + MIN_DUTY_ADC (2us + 1.3us = 3.3us) 3.3/166.6 = 0.02 */

/* dry water motor protection configurations */
#define LOW_WATER_BW				(0.3f)  /* The BW of the low water indeticator, this will be used for speed and current filtering in Hz*/
#define IQ_MODEL_K					(9.15557e-08f) /* gain of the secont order load equation extracted experimentally */
#define SPEED_THRESHOLD				(2000.f) /* bellow that speed, we are not able to correctly detect if the water is enough in the tank */
#define IQ_ERROR_THRESHOLD			(4.0f)    /* The threshold of the error */
#define WATER_EMPTY_DETECT_COUNT    (300)	 /* The error will be triggered if it is exceest for 2 seconds*/

typedef enum
{
    MP_ALARM_NONE             = 0u,
    MP_ALARM_PLAUSIBILITY     = 1u << 0,
    MP_ALARM_UNBALANCE        = 1u << 1,
    MP_ALARM_LOSS_OF_PHASE    = 1u << 2,
    MP_ALARM_RUNNING_OVERLOAD = 1u << 3,
    MP_ALARM_LOCKED_ROTOR     = 1u << 4,
    MP_ALARM_CURRENT_MAX      = 1u << 5
} mp_alarm_bits_t;

typedef struct
{
    /* Timing */
    float Ts_fast_s;              /* e.g., 1/6000 */
    float Ts_slow_s;			/* speed controller Ts */

    /* --- Filters --- */
    /* Plausibility: LPF on sum(u+v+w) at 0.73 Hz */
    st_1st_order_lpf_t lpf_sum_uvw;
    float              sum_uvw_f;

    /* Unbalance/Loss: LPFs on |u|,|v|,|w| at 1.46 Hz */
    st_1st_order_lpf_t lpf_u_abs;
    st_1st_order_lpf_t lpf_v_abs;
    st_1st_order_lpf_t lpf_w_abs;
    float              u_abs_f;
    float              v_abs_f;
    float              w_abs_f;

    /* Signed current filters (for ±avg window) at 1.46 Hz */
    st_1st_order_lpf_t lpf_u_sig;
    st_1st_order_lpf_t lpf_v_sig;
    st_1st_order_lpf_t lpf_w_sig;
    float              u_sig_f;
    float              v_sig_f;
    float              w_sig_f;

    /* I^2 path: Clarke + LPF at 47 Hz */
    st_1st_order_lpf_t lpf_I2_47Hz;
    float              Ialpha;
    float              Ibeta;
    float              I2_raw;
    float              I2_f47;

    /* filters and variables for error generation of low water, uploaded at speed controller */
    st_1st_order_lpf_t	lpf_filtered_iq_current;	 /* LPF for iq current. it used for low water function */
    st_1st_order_lpf_t	lpf_filtered_speed;			 /* LPF for speed. it used for low water function */
    float 		  f4_filtered_iq_current;			 /* The filtered value of the current for the low water error function */
    float		  f4_filtered_speed;				 /* The filtered value of speed for the low water error function */
    float		  iq_estimated;						 /* The estimated value of the current if the tank has enough water (5Litters) */
    bool		  water_empty;						 /* If there is not enough water, this value will be 1 */
    uint32_t 	 water_empty_timer_s;   					// accumulates time with error

    /* --- Thresholds & times --- */
    float plaus_sum_abs_threshold;   /* A */
    float loss_threshold_ratio;      /* 0.75 */
    float unbalance_hold_time_s;     /* s */

    float thr_running_overload_I2;   /* A^2 */
    float thr_locked_rotor_I2;       /* A^2 */
    float thr_current_max_I2;        /* A^2 */

    float t_overload_s;              /* 120 s */
    float t_locked_rotor_s;          /* 0.005 or 0.2 s */
    float t_current_max_s;           /* = Ts_fast_s */

    /* --- Tables (selected per motor) --- */
    float    i_min_A;                /* from table: cs16_i_min_sr[] */
    float    avg_cur_thr_A;          /* from table: cs16_avecur_thr[] */
    uint32_t phl_cnt_max;            /* from table: cu16_max_phl_cnt_sr[] */

    /* --- Accumulators --- */
    float    t_overload_acc_s;
    float    t_locked_acc_s;
    float    t_currentmax_acc_s;
    float    t_lossphase_acc_u_s;
    float    t_lossphase_acc_v_s;
    float    t_lossphase_acc_w_s;
    uint32_t phl_cnt;                /* debounce counter (samples) */
    float	UserVariable[10];

    /* Outputs */
    uint32_t alarm_bits;
    float max_current[3];
} motor_protection_t;

/* Initialize everything (sets filter gains for 0.73 Hz, 1.46 Hz, 47 Hz) */
void motor_protection_init(motor_protection_t *mp, float Ts_fast_s);

/* Initialazation of the motor protection functions which will be run in speed controller */
void motor_protection_speed_init(motor_protection_t *mp, float Ts_slow_s);


/* Select which motor (0 or 1) to use from the 2-entry tables */
void motor_protection_select_motor(motor_protection_t *mp, uint8_t motor_index);

/* Optional: override thresholds / times at runtime (units noted) */
void motor_protection_set_thresholds(motor_protection_t *mp,
                                     float plaus_sum_abs_threshold,
                                     float thr_running_overload_I2,
                                     float thr_locked_rotor_I2,
                                     float thr_current_max_I2,
                                     float t_locked_rotor_s,
                                     float unbalance_hold_time_s);

/* Call every control interrupt (e.g., 6 kHz) with raw phase currents (A) */
void motor_protection_update(motor_protection_t *mp, float u, float v, float w, float raw_u,float raw_v,float raw_w, float min_duty);

/* Call every speed interrupt (e.g., 1 kHz) with speed and iq current of speed output */
void motor_protection_speed_update(motor_protection_t *mp, float speed, float iq_ref, float speed_ref);

void Protect_Init(motor_protection_t *mp, uint8_t motor_index); /* 0 or 1 */

/* Zero all time accumulators and the debounce counter.
   If clear_alarms is true, also clear any latched alarms. */
void motor_protection_enter_idle(motor_protection_t *mp, bool clear_alarms);



/* Utilities */
static inline uint32_t motor_protection_get_alarms(const motor_protection_t *mp)
{
    return mp->alarm_bits;
}

static inline float motor_protection_get_I2_filtered(const motor_protection_t *mp)
{
    return mp->I2_f47;
}

static inline void motor_protection_clear_alarms(motor_protection_t *mp)
{
    mp->alarm_bits = MP_ALARM_NONE;
}

#endif /* R_MOTOR_PROTECTION_H */
