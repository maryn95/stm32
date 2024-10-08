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

# Core project settings
project(cmake_template)                                         # Modified)

option(USE_UTEST "Enable unit tests" OFF)

if (USE_UTEST) ########################################################################################################
    message(STATUS "Compile for unit testing")

    enable_language(C CXX)

    set(EXECUTABLE ${CMAKE_PROJECT_NAME})
      
    file(GLOB sources_SRCS   
    ${PROJ_PATH}/tests/main_test.cpp                            # Select main file for testing    
    ${PROJ_PATH}/tests/gpio_test.cpp                   
    # ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/*.c         # Add another files for testing
    # ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c 
    ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c    
    )

    set(include_path_DIRS                                       # Modified
    ${PROJ_PATH}/Core/Inc
    ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Inc
    ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${PROJ_PATH}/Drivers/CMSIS/Include
    )

    set(symbols_SYMB                                            # Modified
    "DEBUG"
    "STM32L4Q5xx"
    "USE_HAL_DRIVER"
    )

    add_executable(${EXECUTABLE} ${sources_SRCS})

    target_include_directories(${EXECUTABLE} PRIVATE ${include_path_DIRS})

    target_compile_definitions(${EXECUTABLE} PRIVATE ${symbols_SYMB})

    target_compile_options(${EXECUTABLE} PRIVATE
    -Wall
    -Wextra
    -Wpedantic

    -Wno-int-to-pointer-cast
    # -Wno-pointer-to-int-cast
    # -Wno-error=int-conversion

    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Og -g3 -ggdb                                               # For debug configuration
    )

    add_subdirectory(Calculator)                                # Modified

    target_link_libraries(${EXECUTABLE} PUBLIC 
    Calculator                                                  # Modified
    gtest gmock
    )

    target_include_directories(${EXECUTABLE} PUBLIC "${PROJECT_SOURCE_DIR}/Calculator")
else() ################################################################################################################
    message(STATUS "Compile for normal build")

    enable_language(C CXX ASM)

    # Core MCU flags, CPU, instruction set and FPU setup
    set(CPU_PARAMETERS
    -mthumb
    -mcpu=cortex-m4                                             # Modified
    -mfpu=fpv4-sp-d16                                           # Modified
    -mfloat-abi=hard                                            # Modified
    )

    # Set linker script
    set(linker_script_SRC               ${PROJ_PATH}/STM32L4Q5CGUX_FLASH.ld)    # Modified
    set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})

    # List of source files to compile
    file(GLOB sources_SRCS                                      # Modified
    ${PROJ_PATH}/Core/Src/*.cpp
    ${PROJ_PATH}/Core/Src/*.c
    ${PROJ_PATH}/Core/Startup/startup_stm32l4q5cgux.s
    ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Src/*.c
    )

    # Include directories
    set(include_path_DIRS                                       # Modified
    ${PROJ_PATH}/Core/Inc
    ${PROJ_PATH}/Drivers/STM32L4xx_HAL_Driver/Inc
    ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${PROJ_PATH}/Drivers/CMSIS/Include
    )

    # Symbols definition
    set(symbols_SYMB                                            # Modified
    "DEBUG"
    "STM32L4Q5xx"
    "USE_HAL_DRIVER"
    )

    # Executable files
    add_executable(${EXECUTABLE} ${sources_SRCS})

    # Include paths
    target_include_directories(${EXECUTABLE} PRIVATE ${include_path_DIRS})

    # Project symbols
    target_compile_definitions(${EXECUTABLE} PRIVATE ${symbols_SYMB})

    # Compiler options
    target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Og -g3 -ggdb                                               # For debug configuration
    )

    # Linker options
    target_link_options(${EXECUTABLE} PRIVATE
    -T${linker_script_SRC}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -u _printf_float # STDIO float formatting support
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
