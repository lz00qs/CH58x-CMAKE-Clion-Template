# No executable file is generated to verify, it needs to be added when cross-compiling.
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.20)

# Toolchain location
set(TOOLPATH /Users/Shared/MRS_Toolchain_MAC_V191/xpack-riscv-none-embed-gcc-8.2.0/bin)
set(TOOLCHAIN ${TOOLPATH}/riscv-none-embed-)

if (WIN32)
    MESSAGE(STATUS "Now is windows!")
    set(CMAKE_C_COMPILER ${TOOLCHAIN}gcc.exe)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++.exe)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN}gcc.exe)
    set(CMAKE_AR ${TOOLCHAIN}ar.exe)
    set(CMAKE_OBJCOPY ${TOOLCHAIN}objcopy.exe)
    set(CMAKE_OBJDUMP ${TOOLCHAIN}objdump.exe)
    set(SIZE ${TOOLCHAIN}size.exe)
elseif (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS!")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER ${TOOLCHAIN}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN}gcc)
    set(CMAKE_AR ${TOOLCHAIN}ar)
    set(CMAKE_OBJCOPY ${TOOLCHAIN}objcopy)
    set(CMAKE_OBJDUMP ${TOOLCHAIN}objdump)
    set(SIZE ${TOOLCHAIN}size)
else ()
    MESSAGE(STATUS "Unsupported system!")
endif ()

# Project name and language
project(ch58x-Template C CXX ASM)

# Set the compiler standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# Set the compiler flags
add_compile_options(-march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -std=gnu99)
add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common)

# Set compile optimization level (Level 3, maximum optimization)
add_compile_options(-O3)

# Set compile warning level (Generate all warnings)
add_compile_options(-Wall)

# add Definition DEBUG = 1, for files which have "#ifdef DEBUG"
# such as StdPeriphDriver/inc/CH58x_common.h to use printf
add_definitions(-DDEBUG=1)

# Include directories
include_directories(BLE_LIB
        USB_LIB
        RVMSIS
        StdPeriphDriver/inc
        printf
        APP/inc
        HAL/include)

# Source files
file(GLOB_RECURSE SOURCES
        "Startup/*.S"
        "USB_LIB/*.c"
        "RVMSIS/*.c"
        "StdPeriphDriver/*.c"
        "printf/printf.c"
        "APP/*.c"
        "HAL/*.c")


# FreeRTOS supoort
include_directories(FreeRTOS/include
        FreeRTOS/portable/GCC/RISC-V
        FreeRTOS/portable)

file(GLOB_RECURSE SOURCES
        "FreeRTOS/*.c"
        "FreeRTOS/portable/GCC/RISC-V/*.c"
        "FreeRTOS/portable/MemMang/heap_4.c")


# Link options
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Ld/Link.ld)
add_link_options(-nostartfiles
        -Xlinker --gc-sections
        -Wl,--print-memory-usage
        -Wl,-Map,${PROJECT_NAME}.map
        --specs=nano.specs
        --specs=nosys.specs)
add_link_options(-T ${LINKER_SCRIPT})

# Compile executable file
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# Link the static libraries
target_link_libraries(${PROJECT_NAME}.elf
        ${CMAKE_SOURCE_DIR}/StdPeriphDriver/libISP583.a
        ${CMAKE_SOURCE_DIR}/BLE_LIB/LIBCH58xBLE.a
        ${CMAKE_SOURCE_DIR}/USB_LIB/libRV3UFI.a)

# Generate binary and hex files
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        )
