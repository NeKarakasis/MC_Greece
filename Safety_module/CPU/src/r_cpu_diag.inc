;/*******************************************************************************
;* DISCLAIMER
;* This software is supplied by Renesas Electronics Corporation and is only
;* intended for use with Renesas products. No other uses are authorized. This
;* software is owned by Renesas Electronics Corporation and is protected under
;* all applicable laws, including copyright laws.
;* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
;* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
;* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
;* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
;* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
;* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
;* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
;* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
;* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;* Renesas reserves the right, without notice, to make changes to this software
;* and to discontinue the availability of this software. By using this software,
;* you agree to the additional terms and conditions found by accessing the
;* following link:
;* http://www.renesas.com/disclaimer
;*******************************************************************************/
;/* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.   */
;/*******************************************************************************
;* File Name     : r_cpu_diag.inc
;* Version       : 1.00a
;* Device        : RX Family
;* Tool-Chain    : C/C++ Compiler Package for RX Family V3.05.00
;* Description   : CPU Test
;*******************************************************************************/
;/*******************************************************************************
;* History : DD.MM.YYYY Version Description
;*         : 09.06.2023 1.00    First Release
;*         : 03.04.2024 1.00a   Added target device.
;*******************************************************************************/

.INCLUDE r_cpu_diag_config.inc

;; ==== PSW Setting Macro ==== ;;
; SetPSW O, S, Z, C
_SetPSW .MACRO bO, bS, bZ, bC

    .IF bO == 1
        SETPSW O
    .ELSE
        CLRPSW O
    .ENDIF
    .IF bS == 1
        SETPSW S
    .ELSE
        CLRPSW S
    .ENDIF
    .IF bZ == 1
        SETPSW Z
    .ELSE
        CLRPSW Z
    .ENDIF
    .IF bC == 1
        SETPSW C
    .ELSE
        CLRPSW C
    .ENDIF

.ENDM

;; ==== FPSW Setting Macro ==== ;;
; SetFPSW FX, FU, FZ, FO, FV. DN = 1, RM = b'00
_SetFPSW .MACRO bFX, bFU, bFZ, bFO, bFV

    .IF bFX == 1
        .IF bFU == 1
            .IF bFZ == 1
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #7C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #78000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #74000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #70000100H, FPSW
                    .ENDIF
                .ENDIF
            .ELSE ; bFZ == 0
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #6C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #68000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #64000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #60000100H, FPSW
                    .ENDIF
                .ENDIF
            .ENDIF
        .ELSE ; bFU == 0
            .IF bFZ == 1
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #5C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #58000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #54000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #50000100H, FPSW
                    .ENDIF
                .ENDIF
            .ELSE ; bFZ == 0
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #4C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #48000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #44000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #40000100H, FPSW
                    .ENDIF
                .ENDIF
            .ENDIF
        .ENDIF
    .ELSE ; bFX == 0
        .IF bFU == 1
            .IF bFZ == 1
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #3C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #38000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #34000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #30000100H, FPSW
                    .ENDIF
                .ENDIF
            .ELSE ; bFZ == 0
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #2C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #28000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #24000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #20000100H, FPSW
                    .ENDIF
                .ENDIF
            .ENDIF
        .ELSE ; bFU == 0
            .IF bFZ == 1
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #1C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #18000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #14000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #10000100H, FPSW
                    .ENDIF
                .ENDIF
            .ELSE ; bFZ == 0
                .IF bFO == 1
                    .IF bFV == 1
                        MVTC #0C000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #08000100H, FPSW
                    .ENDIF
                .ELSE ; bFO == 0
                    .IF bFV == 1
                        MVTC #04000100H, FPSW
                    .ELSE ; bFV == 0
                        MVTC #00000100H, FPSW
                    .ENDIF
                .ENDIF
            .ENDIF
        .ENDIF
    .ENDIF

.ENDM

;; ==== DPSW Setting Macro ==== ;;
; SetDPSW DFX, DFU, DFZ, DFO, DFV. DDN = 1, DRM = b'00
_SetDPSW .MACRO bDFX, bDFU, bDFZ, bDFO, bDFV

    .IF bDFX == 1
        .IF bDFU == 1
            .IF bDFZ == 1
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #7C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #78000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #74000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #70000100H, R13
                    .ENDIF
                .ENDIF
            .ELSE ; DFZ == 0
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #6C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #68000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #64000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #60000100H, R13
                    .ENDIF
                .ENDIF
            .ENDIF
        .ELSE ; bDFU == 0
            .IF bDFZ == 1
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #5C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #58000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #54000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #50000100H, R13
                    .ENDIF
                .ENDIF
            .ELSE ; bDFZ == 0
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #4C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #48000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #44000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #40000100H, R13
                    .ENDIF
                .ENDIF
            .ENDIF
        .ENDIF
    .ELSE ; bDFX == 0
        .IF bDFU == 1
            .IF bDFZ == 1
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #3C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #38000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #34000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #30000100H, R13
                    .ENDIF
                .ENDIF
            .ELSE ; bDFZ == 0
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #2C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #28000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #24000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #20000100H, R13
                    .ENDIF
                .ENDIF
            .ENDIF
        .ELSE ; bDFU == 0
            .IF bDFZ == 1
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #1C000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #18000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #14000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #10000100H, R13
                    .ENDIF
                .ENDIF
            .ELSE ; bDFZ == 0
                .IF bDFO == 1
                    .IF bDFV == 1
                        MOV.L #0C000100H, R13
                    .ELSE ; bFV == 0
                        MOV.L #08000100H, R13
                    .ENDIF
                .ELSE ; bDFO == 0
                    .IF bDFV == 1
                        MOV.L #04000100H, R13
                    .ELSE ; bDFV == 0
                        MOV.L #00000100H, R13
                    .ENDIF
                .ENDIF
            .ENDIF
        .ENDIF
    .ENDIF
    MVTDC R13, DPSW

.ENDM

;; ==== Registers Save Macro ==== ;;
_SaveRegs .MACRO bRns, bPSW, bFPSW

    .IF bRns == 1
        PUSHM   R6-R13
    .ENDIF
    .IF bPSW == 1
        PUSHC   PSW
    .ENDIF
    .IF bFPSW == 1
        PUSHC   FPSW
    .ENDIF

.ENDM

;; ==== Registers Restore Macro ==== ;;
_RestRegs .MACRO bRns, bPSW, bFPSW

    .IF bFPSW == 1
        POPC    FPSW
    .ENDIF
    .IF bPSW == 1
        POPC    PSW
    .ENDIF
    .IF bRns == 1
        POPM    R6-R13
    .ENDIF

.ENDM

;; ==== Signature Creation Macro ==== ;;
_SetSig .MACRO

    MVFC    PSW, R13
    AND     #0000000FH, R13
    XOR     R13, R14

.ENDM

;; ==== Floating-point Signature Creation Macro ==== ;;
_SetSigFPU .MACRO

    MVFC    FPSW, R13
    AND     #7C000000H, R13
    XOR     R13, R14

.ENDM

;; ==== Double-precision Floating-point Signature Creation Macro ==== ;;
_SetSigDFPU .MACRO

    MVFDC   DPSW, R13
    AND     #7C000000H, R13
    XOR     R13, R14

.ENDM

;; ==== Finalizing Operation Macro (fault injection and creating result) ==== ;;
_FinOpe .MACRO

    CMP     #00000000H, R1
    BNE.B   noForceFail
    NOT     R15

noForceFail:
    CMP     R15, R14
    BNE.B   Error
    MOV.L   #00000001H, R13 ;Pass
    BRA.S   FinEnd

Error:
    MOV.L   #00000000H, R13 ;Fail

FinEnd:
    MOV.L   R13, [R2]

.ENDM

