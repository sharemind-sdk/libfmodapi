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
PROJECT(SharemindLibFModapi VERSION "0.2.1" LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.3 REQUIRED)


FIND_PACKAGE(SharemindCxxHeaders 0.4.0 REQUIRED)
FIND_PACKAGE(SharemindFacilityModuleApis 2.0.0 REQUIRED)


################################################################################
# libfmodapi
################################################################################

ADD_DEFINITIONS("-DSHAREMIND_INTERNAL_")

# Headers:
FILE(GLOB_RECURSE SharemindLibFModapi_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
INSTALL(FILES ${SharemindLibFModapi_HEADERS}
        DESTINATION "include/sharemind/libfmodapi"
        COMPONENT "dev")

# The library:
FILE(GLOB_RECURSE SharemindLibFModapi_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
SharemindSetCxx11CompileOptions()
SharemindAddSharedLibrary("libfmodapi"
    SOURCES
        ${SharemindLibFModapi_SOURCES}
        ${SharemindLibFModapi_HEADERS}
    OUTPUT_NAME "sharemind_fmodapi"
    INCLUDE_DIRECTORIES
        ${SharemindCHeaders_INCLUDE_DIRS}
        ${SharemindLibModapi_INCLUDE_DIRS}
        ${SharemindLibProcessFacility_INCLUDE_DIRS}
        ${SharemindFacilityModuleApis_INCLUDE_DIRS}
    LEGACY_DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
        ${SharemindLibModapi_DEFINITIONS}
        ${SharemindLibProcessFacility_DEFINITIONS}
        ${SharemindFacilityModuleApis_DEFINITIONS}
    COMPILE_DEFINITIONS
        "_XOPEN_SOURCE=700"
    LINK_LIBRARIES
        ${SharemindCHeaders_LIBRARIES}
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
    DEB_DEPENDS
        "libc6 (>= 2.19)"
        "libstdc++6 (>= 4.8.0)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-fmodapi-dev"
    DESCRIPTION "Sharemind Facility Module API library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-fmodapi (= ${SharemindLibFModapi_DEB_lib_PACKAGE_VERSION})"
        "libsharemind-cxxheaders-dev (>= 0.4.0)"
        "sharemind-facility-module-apis-dev (>= 2.0.0)"
        "libc6-dev (>= 2.19)"
        "libstdc++-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-fmodapi-dbg"
    DESCRIPTION "Sharemind Facility Module API library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-fmodapi (= ${SharemindLibFModapi_DEB_lib_PACKAGE_VERSION})"
)
SharemindPackagingFinalize()
