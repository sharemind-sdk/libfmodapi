#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
PROJECT(SharemindLibFModapi VERSION "0.1.0" LANGUAGES "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()


FIND_PACKAGE(SharemindCHeaders REQUIRED)
FIND_PACKAGE(SharemindLibModapi 0.3.0 REQUIRED)
FIND_PACKAGE(SharemindLibProcessFacility REQUIRED)
FIND_PACKAGE(SharemindModuleApis REQUIRED)


################################################################################
# libfmodapi
################################################################################

# Headers:
SET(SharemindLibFModapi_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/libfmodapi.h")
INSTALL(FILES ${SharemindLibFModapi_HEADERS}
        DESTINATION "include/sharemind/libfmodapi"
        COMPONENT "dev")

# The library:
FILE(GLOB_RECURSE SharemindLibFModapi_HEADERS_P
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
LIST(REMOVE_ITEM SharemindLibFModapi_HEADERS_P
    ${SharemindLibFModapi_HEADERS}
)
FILE(GLOB_RECURSE SharemindLibFModapi_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
SharemindSetC99CompileOptions(COMPILE_FLAGS "-fwrapv")
SharemindAddSharedLibrary("libfmodapi"
    SOURCES
        ${SharemindLibFModapi_SOURCES}
        ${SharemindLibFModapi_HEADERS}
        ${SharemindLibFModapi_HEADERS_P}
    OUTPUT_NAME "sharemind_fmodapi"
    INCLUDE_DIRECTORIES
        ${SharemindCHeaders_INCLUDE_DIRS}
        ${SharemindLibModapi_INCLUDE_DIRS}
        ${SharemindLibProcessFacility_INCLUDE_DIRS}
        ${SharemindModuleApis_INCLUDE_DIRS}
    LEGACY_DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
        ${SharemindLibModapi_DEFINITIONS}
        ${SharemindLibProcessFacility_DEFINITIONS}
    COMPILE_DEFINITIONS
        "_XOPEN_SOURCE=700"
    LINK_LIBRARIES
        "-ldl"
)

# Install cmake files:
SharemindCreateCMakeFindFiles(
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
    DEFINITIONS
        "-D__STDC_LIMIT_MACROS"
        "-D_XOPEN_SOURCE=700"
    LIBRARIES
        "-L${CMAKE_INSTALL_PREFIX}/lib"
        "-lsharemind_fmodapi"
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-fmodapi"
    DESCRIPTION "Sharemind Facility Module API library"
    DEB_SECTION "libs"
    DEB_DEPENDS "libc6 (>= 2.19)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-fmodapi-dev"
    DESCRIPTION "Sharemind Facility Module API library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-fmodapi (= ${SharemindLibFModapi_VERSION})"
        "libsharemind-fmodapi-api-dev (= ${SharemindLibFModapi_VERSION})"
        "libsharemind-modapi-dev (>= 0.3.0)"
        "libsharemind-processfacility-api-dev"
        "libsharemind-cheaders-dev (>= 0.1.0)"
        "sharemind-facility-module-apis-dev (>= 1.0.0)"
        "libc6-dev (>= 2.19)"
        "libgcc-4.8-dev"
        "| libgcc-4.9-dev"
        "| libgcc-5-dev"
        "| libgcc-6-dev"
        "| libgcc-7-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-fmodapi-dbg"
    DESCRIPTION "Sharemind Facility Module API library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-fmodapi (= ${SharemindLibFModapi_VERSION})"
)
SharemindPackagingFinalize()
