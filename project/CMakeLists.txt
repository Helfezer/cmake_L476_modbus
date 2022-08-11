cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../tools)
set(FETCHCONTENT_QUIET FALSE)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)

FetchContent_MakeAvailable(gcc-arm-none-eabi-10.3-2021.10-x86_64-linux)
set(STM32_TOOLCHAIN_PATH ${gcc-arm-none-eabi-10.3-2021.10-x86_64-linux_SOURCE_DIR})

FetchContent_MakeAvailable(stm32-cmake)
set(CMAKE_TOOLCHAIN_FILE ${stm32-cmake_SOURCE_DIR}/cmake/stm32_gcc.cmake)

# FetchContent_MakeAvailable(cmsis-svd)

set(CMAKE_BUILD_TYPE Debug)

project (l476_modbus C ASM)

stm32_fetch_cube(L4)

find_package(CMSIS COMPONENTS STM32L476RG REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)

# load dependencies
FetchContent_MakeAvailable(liblightmodbus)
FetchContent_MakeAvailable(generic-linked-list)

FetchContent_Populate(UML-State-Machine-in-C)
set(UMLSMC_SRCS
${uml-state-machine-in-c_SOURCE_DIR}/src/hsm.c
)

set(PROJECT_SOURCES
    l476_modbus/Core/Src/main.c
    l476_modbus/Core/Src/stm32l4xx_hal_msp.c
    l476_modbus/Core/Src/stm32l4xx_it.c
)

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

include_directories(${CMAKE_PROJECT_NAME}
    l476_modbus/Core/Inc
    ${HAL_INCLUDE_DIRS}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${liblightmodbus_SOURCE_DIR}/include/lightmodbus
    ${uml-state-machine-in-c_SOURCE_DIR}/src
)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    CMSIS::STM32::L476xx
    HAL::STM32::L4::RCC
    HAL::STM32::L4::GPIO
    HAL::STM32::L4::UART
    HAL::STM32::L4::PWREx
    HAL::STM32::L4::CORTEX
    HAL::STM32::L4::RCCEx
    STM32::NoSys
)
stm32_print_size_of_target(${CMAKE_PROJECT_NAME})
stm32_generate_hex_file(${CMAKE_PROJECT_NAME})
stm32_add_linker_script(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/l476_modbus/STM32L476RGTx_FLASH.ld)