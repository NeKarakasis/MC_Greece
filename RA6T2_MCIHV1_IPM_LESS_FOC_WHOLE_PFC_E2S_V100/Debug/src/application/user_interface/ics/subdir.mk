################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/application/user_interface/ics/ICS2_RA6T2.o \
../src/application/user_interface/ics/ICS2_RA6T2_Built_in.o 

C_SRCS += \
../src/application/user_interface/ics/r_mtr_ics.c 

C_DEPS += \
./src/application/user_interface/ics/r_mtr_ics.d 

OBJS += \
./src/application/user_interface/ics/r_mtr_ics.o 

SREC += \
RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100.srec 

MAP += \
RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100.map 


# Each subdirectory must supply rules for building sources it contributes
src/application/user_interface/ics/%.o: ../src/application/user_interface/ics/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -D_RA_ORDINAL=1 -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra/fsp/inc" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra/fsp/inc/api" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra/fsp/inc/instances" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra_gen" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra_cfg/fsp_cfg/bsp" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra_cfg/fsp_cfg" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/main" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/cfg" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/current" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/driver" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/general" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/sensorless_vector" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/motor_module/speed" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/pfc_module/pfc_cfg" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/pfc_module/pfc_ctrl" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/pfc_module/pfc_driver" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/pfc_module/pfc_general" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/pfc_module/pfc_systask" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/system_module/system_manager" -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/src/application/user_interface/ics" -I"." -I"C:/Users/a5143778/Documents/MC_Greece/MC_Greece/RA6T2_MCIHV1_IPM_LESS_FOC_WHOLE_PFC_E2S_V100/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

