cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
message("Build type: "              ${CMAKE_BUILD_TYPE})

# project name
project(l4q5_boot)                                         # Modified)

add_library(project_compiler_flags INTERFACE)

# gtest configuration
option(GTEST_BUILD "Gtest configuration" OFF)

# bootloader configuration
option(BOOTLOADER_BUILD "Bootloader configuration" OFF)

if (GTEST_BUILD) ########################################################################################################

    message(STATUS "Compile for unit testing")

    enable_language(C CXX)

    set(EXECUTABLE ${CMAKE_PROJECT_NAME})
      
    file(GLOB PROJECT_SOURCES  
        ${PROJ_PATH}/tests/main_test.cpp                            # Select main file for testing    
        ${PROJ_PATH}/tests/gpio_test.cpp                   
        # ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/*.c
        ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c    
    )

    set(PROJECT_INCLUDES                                        # Modified
        ${PROJ_PATH}/Core/Inc
        ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Inc
        ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
        ${PROJ_PATH}/Drivers/CMSIS/Include
    )

    set(PROJECT_DEFINES                                         # Modified
        "DEBUG"
        "STM32L4Q5xx"
        "USE_HAL_DRIVER"
    )

    add_executable(${EXECUTABLE} ${PROJECT_SOURCES})

    target_include_directories(${EXECUTABLE} PRIVATE ${PROJECT_INCLUDES})

    target_compile_definitions(${EXECUTABLE} PRIVATE ${PROJECT_DEFINES})

    target_compile_options(${EXECUTABLE} PRIVATE
        -Wall
        -Wextra
        -Wpedantic

        -Wno-int-to-pointer-cast
        # -Wno-pointer-to-int-cast
        # -Wno-error=int-conversion

        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Og -g3 -ggdb
    )

    add_subdirectory(Calculator)                                # Modified

    target_link_libraries(${EXECUTABLE} PUBLIC 
        Calculator                                                  # Modified
        gtest gmock
    )
else() ################################################################################################################

    message(STATUS "Compile for normal build")

    get_filename_component(STM_LIB_PATH ${PROJ_PATH}/../../Libs ABSOLUTE)     # PATH TO LIBS

    enable_language(C CXX ASM)

    # Set linker script
    if (BOOTLOADER_BUILD)
        set(PROJECT_LINKER_SCRIPT           ${PROJ_PATH}/STM32L4Q5CGUX_BOOT_FLASH.ld)    # Modified
    else()
        set(PROJECT_LINKER_SCRIPT           ${PROJ_PATH}/STM32L4Q5CGUX_FLASH.ld)         # Modified
    endif()

    set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})

    # Core MCU flags, CPU, instruction set and FPU setup
    set(CPU_PARAMETERS
        -mthumb
        -mcpu=cortex-m4                                             # Modified
        -mfpu=fpv4-sp-d16                                           # Modified
        -mfloat-abi=hard                                            # Modified
    )

    # List of source files to compile
    file(GLOB GLOB_SOURCES
        ${PROJ_PATH}/Core/Src/*.cpp
        ${PROJ_PATH}/Core/Src/*.c
        ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/*.c
    )
    set(PROJECT_SOURCES                                         # Modified
        ${PROJ_PATH}/Core/Startup/startup_stm32l4q5cgux.s
        ${GLOB_SOURCES}
        # ${STM_LIB_PATH}/Crc/crc.c
        # ${STM_LIB_PATH}/Flash/FlashStorage.cpp
        ${STM_LIB_PATH}/Flash/Bootloader.cpp
        ${STM_LIB_PATH}/Protocol/PacketBase/PacketBase.cpp
        ${STM_LIB_PATH}/Protocol/CircularByteBuffer/CircularByteBuffer.cpp
        ${STM_LIB_PATH}/Protocol/Physics/UARTDMAPhysics/UARTDMAPhysics.cpp
        ${STM_LIB_PATH}/Protocol/Transport/TransportBase.cpp
        ${STM_LIB_PATH}/Protocol/Parser/ParserBase.cpp
        ${STM_LIB_PATH}/Protocol/Parser/PacketParser.cpp
    )

    # Include directories
    set(PROJECT_INCLUDES                                       # Modified
        ${PROJ_PATH}/Core/Inc
        ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Inc
        ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
        ${PROJ_PATH}/Drivers/CMSIS/Include
        # ${STM_LIB_PATH}/Crc
        ${STM_LIB_PATH}/Protocol
        ${STM_LIB_PATH}/Flash
    )

    # Symbols definition
    set(PROJECT_DEFINES                                            # Modified
        "DEBUG"
        "STM32L4Q5xx"
        "USE_HAL_DRIVER"
    )

    # Subdirectories
    add_subdirectory(${STM_LIB_PATH}/Crc "crc")
    add_subdirectory(${STM_LIB_PATH}/Flash/FlashStorage "FlashStorage")
    
    # Executable files
    add_executable(${EXECUTABLE} ${PROJECT_SOURCES})

    # Include paths
    target_include_directories(${EXECUTABLE} PRIVATE ${PROJECT_INCLUDES})

    # Project symbols
    target_compile_definitions(${EXECUTABLE} PRIVATE ${PROJECT_DEFINES})

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(PROJ_OPTIMIZATION -Og -g3 -ggdb)
    # elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    #     set(PROJ_OPTIMIZATION -Os)
    else()
        set(PROJ_OPTIMIZATION -Os)
    endif()

    # Compiler options
    target_compile_options(project_compiler_flags INTERFACE
        ${CPU_PARAMETERS}
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        ${PROJ_OPTIMIZATION}
    )

    target_link_libraries(${EXECUTABLE} PUBLIC 
        crc 
        FlashStorage 
        project_compiler_flags
    )

    # Linker options
    target_link_options(${EXECUTABLE} PRIVATE
        -T${PROJECT_LINKER_SCRIPT}
        ${CPU_PARAMETERS}
        -Wl,-Map=${CMAKE_PROJECT_NAME}.map
        --specs=nano.specs
        -Wl,--start-group
        -lc
        -lm
        -lstdc++
        -lsupc++
        -Wl,--end-group
        -Wl,--print-memory-usage
    )

    # Execute post-build to print size
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
    )

    # Convert output to hex
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
    )

    # Convert to bin file
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
    )
endif() ###############################################################################################################
