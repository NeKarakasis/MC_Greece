/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_SHARED_CCAL_TYPES_H
#define RM_MOTOR_SHARED_CCAL_TYPES_H
#include <stdint.h>

#ifdef CCAL_FLOAT
typedef float ccal_num_t;
#elif defined(CCAL_QINT)
typedef int32_t ccal_num_t;
#else
 #error "No valid numerical type is defined. Define either CCAL_FLOAT or CCAL_QINT."
#endif

#endif
