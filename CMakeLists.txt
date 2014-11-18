#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBFMODAPI" "C")
SET(LIBFMODAPI_VERSION "0.1.0.0")
SET(LIBFMODAPI_COMPAT_VERSION "0.1.0.0")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindVmGlobals REQUIRED)
FIND_PACKAGE(SharemindLibmodapi 0.3.0 REQUIRED)
INCLUDE_DIRECTORIES(
    ${SharemindLibmodapi_INCLUDE_DIRS}
    ${SharemindVmGlobals_INCLUDE_DIRS}
)

ADD_COMPILE_OPTIONS(
    "-std=c99" "-O2" "-fwrapv" "-Wall" "-Wextra" "-Wunused" "-Wabi"
    "-Wstrict-aliasing=1" "-Wbad-function-cast" "-Wformat" "-Wswitch-default"
    "-Wfloat-equal" "-Wshadow" "-Wpointer-arith" "-Wcast-qual"
    "-Wstrict-prototypes" "-Wconversion" "-Wsign-conversion" "-Wunused-macros"
    "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
    "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
    "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
    "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

INCLUDE(CheckCCompilerFlag)
FUNCTION(CHECK_ADD_COMPILER_FLAG flag)
    STRING(SUBSTRING "${flag}" 1 -1 FlagName)
    CHECK_C_COMPILER_FLAG("${flag}" C_COMPILER_HAS_${FlagName}_FLAG)
    IF(C_COMPILER_HAS_${FlagName}_FLAG)
        ADD_COMPILE_OPTIONS("${flag}")
    ENDIF()
ENDFUNCTION()
CHECK_ADD_COMPILER_FLAG("-Weverything")
CHECK_ADD_COMPILER_FLAG("-Wlogical-op")
CHECK_ADD_COMPILER_FLAG("-Wno-covered-switch-default")
CHECK_ADD_COMPILER_FLAG("-Wno-padded")

SET(LIBFMODAPI_INSTALL_DEFINITIONS "-D__STDC_LIMIT_MACROS"
                                   "-D_XOPEN_SOURCE=700")
ADD_DEFINITIONS(
    ${SharemindLibmodapi_DEFINITIONS}
    ${SharemindVmGlobals_DEFINITIONS}
    ${LIBFMODAPI_INSTALL_DEFINITIONS})



################################################################################
# libfmodapi
################################################################################

SET(SHAREMIND_LIBFMODAPI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libfmodapi.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api_0x1.h"
)

FILE(GLOB_RECURSE SHAREMIND_LIBFMODAPI_HEADERS_P
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
LIST(REMOVE_ITEM SHAREMIND_LIBFMODAPI_HEADERS_P ${SHAREMIND_LIBFMODAPI_HEADERS})

IF(NOT SHAREMIND_LIBFMODAPI_LINKING_MODE)
    SET(SHAREMIND_LIBFMODAPI_LINKING_MODE SHARED)
ENDIF()

FILE(GLOB_RECURSE SHAREMIND_LIBFMODAPI_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
ADD_LIBRARY("libfmodapi" ${SHAREMIND_LIBFMODAPI_LINKING_MODE}
            ${SHAREMIND_LIBFMODAPI_SOURCES}
            ${SHAREMIND_LIBFMODAPI_HEADERS}
            ${SHAREMIND_LIBFMODAPI_HEADERS_P}
)
STRING(REGEX REPLACE "(([^\\.]+\\.[^\\.]+)\\.).*" "\\2" LIBFMODAPI_SOVERSION
                     "${LIBFMODAPI_VERSION}")
SET_TARGET_PROPERTIES("libfmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_fmodapi"
                      VERSION "${LIBFMODAPI_VERSION}"
                      SOVERSION "${LIBFMODAPI_SOVERSION}")
TARGET_LINK_LIBRARIES("libfmodapi" "-ldl")

INSTALL(TARGETS "libfmodapi"
        LIBRARY DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBFMODAPI_HEADERS}
        DESTINATION "include/sharemind/libfmodapi")

# Install cmake files:
FILE(GLOB_RECURSE sharemind_LIBFMODAPI_in_files
     "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libfmodapi_in_files_in_IDE"
                  SOURCES ${sharemind_LIBFMODAPI_in_files})
SET(LIBFMODAPI_INSTALL_LINK_LIBRARIES "-L${CMAKE_INSTALL_PREFIX}/lib"
                                      "-lsharemind_fmodapi")
SET(LIBFMODAPI_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibfmodapiConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibfmodapiConfig.cmake"
               @ONLY)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibfmodapiConfigVersion.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibfmodapiConfigVersion.cmake" @ONLY)
INSTALL(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibfmodapiConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibfmodapiConfigVersion.cmake"
        DESTINATION "lib/SharemindLibfmodapi")
