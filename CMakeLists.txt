# Please see README.md for build instructions

cmake_minimum_required(VERSION 3.16.4)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)

if(BUILD_KIT)
	message("Build Kit is defined as ${BUILD_KIT}")
endif()

message("CMAKE_CURRENT_BINARY_DIR is defined as ${CMAKE_CURRENT_BINARY_DIR}")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig)
    message("Multi-config generator(${CMAKE_GENERATOR}) is active")
else()
    message("Single-config generator(${CMAKE_GENERATOR}) is active")
	if(DEFINED CMAKE_BUILD_TYPE)
	 	message("CMAKE_BUILD_TYPE is defined as ${CMAKE_BUILD_TYPE}")
	elseif(DEFINED BUILD_TYPE)
		set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
		message("CMAKE_BUILD_TYPE is set to BUILD_TYPE ENV ${CMAKE_BUILD_TYPE}")
	else()
		message(FATAL_ERROR "Please specify the CMAKE_BUILD_TYPE with -DCMAKE_BUILD_TYPE=")
	endif()
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	message("CMAKE_RUNTIME_OUTPUT_DIRECTORY is defined as ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

cmake_minimum_required(VERSION 3.16.4)

# Project configuration
project(Vorwerk_HFI_version062026
	VERSION 1.0.0
	LANGUAGES C CXX ASM)

set(CMAKE_CROSSCOMPILING "TRUE")
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedCfg.cmake)

# If required user can amend RASC generated flags here, e.g. list(REMOVE_ITEM RASC_CMAKE_C_FLAGS "-Wall")

# Use an upper-case base name for all generated MAP files (linker .map and ICS _conv.map)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(REPLACE
    "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map"
    "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_UPPER}.map"
    RASC_CMAKE_EXE_LINKER_FLAGS "${RASC_CMAKE_EXE_LINKER_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedSrc.cmake)

# -----------------------------------------------------------------------------
# User-added include directories: every subfolder under src/application/
# so headers placed anywhere in the application tree resolve without errors.
# -----------------------------------------------------------------------------
file(GLOB_RECURSE APP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application/*.hpp
)
set(APP_INCLUDE_DIRS "")
foreach(_hdr ${APP_HEADERS})
    get_filename_component(_dir ${_hdr} DIRECTORY)
    list(APPEND APP_INCLUDE_DIRS ${_dir})
endforeach()
# Always include the application root itself, even if it has no header
list(APPEND APP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/application)
list(REMOVE_DUPLICATES APP_INCLUDE_DIRS)

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${APP_INCLUDE_DIRS})

# -----------------------------------------------------------------------------
# User-added prebuilt artifacts under src/application/
# (kept outside GeneratedSrc.cmake so RASC regeneration does not overwrite them)
# -----------------------------------------------------------------------------

# Prebuilt static libraries (.a) from motor_module
set(APP_MOTOR_CURRENT_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/src/application/motor_module/current)
set(APP_MOTOR_SENSORLESS_VEC_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/src/application/motor_module/sensorless_vector)

add_library(r_motor_current_bemf_observer       STATIC IMPORTED)
set_target_properties(r_motor_current_bemf_observer  PROPERTIES
    IMPORTED_LOCATION ${APP_MOTOR_CURRENT_DIR}/libr_motor_current_bemf_observer.a)

add_library(r_motor_current_stall_detection     STATIC IMPORTED)
set_target_properties(r_motor_current_stall_detection PROPERTIES
    IMPORTED_LOCATION ${APP_MOTOR_CURRENT_DIR}/libr_motor_current_stall_detection.a)

add_library(r_motor_current_trq_vib_comp        STATIC IMPORTED)
set_target_properties(r_motor_current_trq_vib_comp PROPERTIES
    IMPORTED_LOCATION ${APP_MOTOR_CURRENT_DIR}/libr_motor_current_trq_vib_comp.a)

add_library(r_motor_current_volt_err_comp       STATIC IMPORTED)
set_target_properties(r_motor_current_volt_err_comp PROPERTIES
    IMPORTED_LOCATION ${APP_MOTOR_CURRENT_DIR}/libr_motor_current_volt_err_comp.a)

add_library(r_motor_sensorless_vector_flyingstart STATIC IMPORTED)
set_target_properties(r_motor_sensorless_vector_flyingstart PROPERTIES
    IMPORTED_LOCATION ${APP_MOTOR_SENSORLESS_VEC_DIR}/libr_motor_sensorless_vector_flyingstart.a)

# Use a linker group so cross-references between these archives resolve regardless of order
target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    -Wl,--start-group
        r_motor_current_bemf_observer
        r_motor_current_stall_detection
        r_motor_current_trq_vib_comp
        r_motor_current_volt_err_comp
        r_motor_sensorless_vector_flyingstart
    -Wl,--end-group
)

# Prebuilt object files (.o) from user_interface/ics
# NOTE: ICS2_RA6T2.o and ICS2_RA6T2_Built_in.o are alternative variants of the same
# module. If you get multiple-definition link errors, comment out the one you don't need.
#set(APP_ICS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/application/user_interface/ics)
#set(APP_ICS_OBJECTS
  #  ${APP_ICS_DIR}/ICS2_RA6T2.o
 #   ${APP_ICS_DIR}/ICS2_RA6T2_Built_in.o
#)
#set_source_files_properties(${APP_ICS_OBJECTS} PROPERTIES EXTERNAL_OBJECT TRUE)
#target_sources(${PROJECT_NAME}.elf PRIVATE ${APP_ICS_OBJECTS})

# -----------------------------------------------------------------------------
# Post-build: run convert.bat from src/application/user_interface/ics to
# generate the ICS .rmap / _conv.map files from the linked ELF.
# The .bat is executed in the build output directory (where the .elf lives)
# and receives the elf basename (without extension) as its argument.
# -----------------------------------------------------------------------------
set(APP_ICS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/application/user_interface/ics)
# Resolve the objdump that matches our cross compiler. CMake's build commands
# call gcc/objcopy by absolute path, so the toolchain bin dir is NOT on PATH
# inside post-build steps. We must therefore hand convert.bat the explicit
# objdump executable; relying on PATH was the cause of the
# "'arm-none-eabi-objdump.exe' is not recognized" post-build failure.
# CMAKE_OBJDUMP is autodetected from the toolchain by CMake's compiler probe.
if(NOT CMAKE_OBJDUMP)
    message(FATAL_ERROR "CMAKE_OBJDUMP is not set; cannot run ICS post-build step")
endif()
# Pass: 1) ELF basename (matches the actual .elf produced by the linker)
#       2) Output basename for the generated MAP files (upper-case)
#       3) Full path to objdump (handed to convert.bat so PATH is irrelevant)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND cmd /c "${APP_ICS_DIR}/convert.bat" ${PROJECT_NAME} ${PROJECT_NAME_UPPER} "${CMAKE_OBJDUMP}"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}.elf>
    COMMENT "Running ICS convert.bat to generate ${PROJECT_NAME_UPPER}_conv.map"
    VERBATIM
)
