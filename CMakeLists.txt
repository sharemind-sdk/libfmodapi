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
PROJECT(SharemindLibFModapi VERSION 0.4.0 LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

FIND_PACKAGE(SharemindCHeaders 1.3.0 REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders 0.8.0 REQUIRED)
FIND_PACKAGE(SharemindFacilityModuleApis 2.1.0 REQUIRED)


# LibFModapi:
FILE(GLOB_RECURSE SharemindLibFModapi_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
INSTALL(FILES ${SharemindLibFModapi_HEADERS}
        DESTINATION "include/sharemind/libfmodapi"
        COMPONENT "dev")
FILE(GLOB_RECURSE SharemindLibFModapi_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
SharemindSetCxx14CompileOptions()
SharemindAddSharedLibrary(LibFModapi
    OUTPUT_NAME "sharemind_fmodapi"
    SOURCES
        ${SharemindLibFModapi_SOURCES}
        ${SharemindLibFModapi_HEADERS}
)
TARGET_INCLUDE_DIRECTORIES(LibFModapi
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    )
TARGET_COMPILE_DEFINITIONS(LibFModapi
    PRIVATE
        "SHAREMIND_INTERNAL_"
    PUBLIC
        "__STDC_LIMIT_MACROS"
        "_XOPEN_SOURCE=700"
    )
TARGET_LINK_LIBRARIES(LibFModapi
    PRIVATE
        "dl"
    PUBLIC
        "Sharemind::CHeaders"
        "Sharemind::CxxHeaders"
        "Sharemind::FacilityModuleApis"
    )
SharemindCreateCMakeFindFilesForTarget(LibFModapi
    DEPENDENCIES
        "SharemindCHeaders 1.3.0"
        "SharemindCxxHeaders 0.8.0"
        "SharemindFacilityModuleApis 2.1.0"
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
        "libsharemind-cxxheaders-dev (>= 0.8.0)"
        "sharemind-facility-module-apis-dev (>= 2.1.0)"
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
