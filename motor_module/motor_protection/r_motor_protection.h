#ifndef R_MOTOR_PROTECTION_H
#define R_MOTOR_PROTECTION_H

#include <stdint.h>
#include <stdbool.h>
#include "r_motor_filter.h"


/* =========================
 * Configuration tables (2 motors) — EDIT THESE VALUES
 * ========================= */
#define MP_NUM_MOTORS                (2u)

/* Minimum current clamp for unbalance threshold (A) */
#define MP_I_MIN_A_MOTOR0           (0.05f)
#define MP_I_MIN_A_MOTOR1           (0.01f)

/* Signed average current window (A): |i_filt| must be <= this */
#define MP_AVG_CUR_THR_A_MOTOR0     (1.20f)
#define MP_AVG_CUR_THR_A_MOTOR1     (0.45f)

/* Debounce max count for loss/unbalance combined condition (samples @ 6 kHz) */
#define MP_PHL_CNT_MAX_MOTOR0       (90000u)   /* 1.5sec at 6 kHz */
#define MP_PHL_CNT_MAX_MOTOR1       (300u)

/* Other defaults (you can override at runtime if needed) */
#define MP_PLAUS_THRESH_A_DEFAULT   (0.03f)  /* |LPF(u+v+w)| alarm threshold (A) */
#define MP_UNBAL_HOLD_S_DEFAULT     (0.50f)  /* seconds; persistence time */
#define MP_LOSS_RATIO_DEFAULT       (0.75f)  /* 75% of average abs */
#define MP_SIGNED_CURRENT_AVARAGE   (0.100f) /* the maximum avarage singed value of current waveform */

/* I^2 thresholds (A^2) — tune to your machine or override via setter */
#define MP_I2_RUN_OVER_A2_DEFAULT   (0.7f)  /* Running Overload */
#define MP_I2_LOCKED_A2_DEFAULT     (0.8f)  /* Locked Rotor */
#define MP_I2_CURR_MAX_A2_DEFAULT   (64.0f)  /* Current Max */

/* Class timings */
#define MP_T_OVERLOAD_S_DEFAULT     (120.0f)
#define MP_T_LOCKED_S_DEFAULT       (1.5f)  /* set to 0.2f for drain pump */

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
    uint16_t phl_cnt_max;            /* from table: cu16_max_phl_cnt_sr[] */

    /* --- Accumulators --- */
    float    t_overload_acc_s;
    float    t_locked_acc_s;
    float    t_currentmax_acc_s;
    float    t_lossphase_acc_u_s;
    float    t_lossphase_acc_v_s;
    float    t_lossphase_acc_w_s;

    uint16_t phl_cnt;                /* debounce counter (samples) */

    /* Outputs */
    uint32_t alarm_bits;
} motor_protection_t;

/* Initialize everything (sets filter gains for 0.73 Hz, 1.46 Hz, 47 Hz) */
void motor_protection_init(motor_protection_t *mp, float Ts_fast_s);

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
void motor_protection_update(motor_protection_t *mp, float u, float v, float w);

void Protect_Init(bool is_drain_pump, uint8_t motor_index ); /* 0 or 1 */

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
