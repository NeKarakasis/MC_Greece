/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_SHARED_CCAL_MACROS_H
#define RM_MOTOR_SHARED_CCAL_MACROS_H

#include "rm_motor_shared_ccal_types.h"

#define     SQRT_2_F    (1.41421356F)  /* Sqrt(2) */
#define     SQRT_3_F    (1.7320508F)   /* Sqrt(3) */
#define     ZERO_F      (0.0F)

#ifdef CCAL_FLOAT
 #define CCAL_ZERO      ZERO_F
 #define CCAL_1(n)            (1.0F)
 #define CCAL_DIV_BY_2(x)     ((x) * 0.5F)
 #define CCAL_MUL_BY_2(x)     ((x) * 2.0F)
 #define CCAL_MUL(x, y, n)    ((x) * (y))  /* n: Do not use it in floating point. */
 #define CCAL_DIV(x, y, n)    ((x) / (y))  /* n: Do not use it in floating point. */
 #define CCAL_RECI(x, n)      (1.0F / (x)) /* n: Do not use it in floating point. */
#elif defined(CCAL_QINT)
 #define CCAL_ZERO    0
 #define CCAL_1(n)            (1 << (n))
 #define CCAL_DIV_BY_2(x)     ((x) >> 1)
 #define CCAL_MUL_BY_2(x)     ((x) << 1)
 #define CCAL_MUL(x, y, n)    FIX_mul32_rs(x, y, n)           /* n: decimal right shift size after calculation */
 #define CCAL_DIV(x, y, n)    FIX_div_rs(x, y, n)             /* n: decimal right shift size after calculation */
 #define CCAL_RECI(x, n)      FIX_div_rs(1 << (n), x, 32 - n) /* n: decimal point of x */
#endif

#endif
