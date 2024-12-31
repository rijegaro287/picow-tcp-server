# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picow-tcp-server C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# pico_set_program_name(picow-tcp-server "picow-tcp-server")
# pico_set_program_version(picow-tcp-server "0.1")

set(SOURCES src/main.c
            src/tcp_server.c
            src/processing.c
)

set(HEADERS include)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
                           ${HEADERS}
                           ${CMAKE_CURRENT_LIST_DIR}
                           ${CMAKE_CURRENT_LIST_DIR}/..
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(picow-tcp-server 1)
pico_enable_stdio_usb(picow-tcp-server 0)

# Add the standard library to the build
target_link_libraries(${CMAKE_PROJECT_NAME}
                      pico_cyw43_arch_lwip_threadsafe_background
                      pico_stdlib
)

# Add the standard include files to the build
target_include_directories(picow-tcp-server PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(picow-tcp-server 
        
        )

pico_add_extra_outputs(picow-tcp-server)

