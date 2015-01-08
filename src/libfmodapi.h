/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_LIBFMODAPI_H
#define SHAREMIND_LIBFMODAPI_LIBFMODAPI_H

#include <sharemind/extern_c.h>
#include <sharemind/lasterror.h>
#include <sharemind/libmodapi/libmodapi.h>
#include <sharemind/preprocessor.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "api_0x1.h"


SHAREMIND_EXTERN_C_BEGIN


/*******************************************************************************
  Module API compatibility macros
*******************************************************************************/

#define SHAREMIND_FACILITY_MODULE_API_API_VERSION     1u
#define SHAREMIND_FACILITY_MODULE_API_API_MIN_VERSION 1u


/*******************************************************************************
  Helper macros
*******************************************************************************/

#define SHAREMIND_LIBFMODAPI_DECLARE_ERROR_FUNCTIONS(ClassName) \
    SHAREMIND_LASTERROR_PUBLIC_FUNCTIONS_DECLARE( \
            ClassName,, \
            SharemindFacilityModuleApiError,)

#define SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName,Name) \
    SharemindFacility const * ClassName ## _find ## Name ## Facility( \
            const ClassName * m, \
            const char * signature) __attribute__ ((nonnull(1, 2)))

#define SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS(ClassName) \
    SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName,Module);\
    SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName,Pd); \
    SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName,Pdpi)


/*******************************************************************************
  Types
*******************************************************************************/

struct SharemindFacilityModuleApi_;
typedef struct SharemindFacilityModuleApi_ SharemindFacilityModuleApi;

struct SharemindFacilityModule_;
typedef struct SharemindFacilityModule_ SharemindFacilityModule;

#define SHAREMIND_FACILITY_MODULE_API_ERROR_ENUM \
    ((SHAREMIND_FACILITY_MODULE_API_OK, = 0)) \
    ((SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_IMPLEMENTATION_LIMITS_REACHED,)) \
    ((SHAREMIND_FACILITY_MODULE_API_MUTEX_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_UNABLE_TO_OPEN_MODULE,)) \
    ((SHAREMIND_FACILITY_MODULE_API_INVALID_MODULE,)) \
    ((SHAREMIND_FACILITY_MODULE_API_API_NOT_SUPPORTED,)) \
    ((SHAREMIND_FACILITY_MODULE_API_API_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_DUPLICATE_MODULE_FACILITY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_DUPLICATE_PD_FACILITY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_DUPLICATE_PDPI_FACILITY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_SHAREMIND_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_MODULE_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_INVALID_CONFIGURATION,)) \
    ((SHAREMIND_FACILITY_MODULE_API_ERROR_COUNT,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindFacilityModuleApiError,
                             SHAREMIND_FACILITY_MODULE_API_ERROR_ENUM);
SHAREMIND_ENUM_DECLARE_TOSTRING(SharemindFacilityModuleApiError);


/*******************************************************************************
  SharemindFacilityModuleApi
*******************************************************************************/

SharemindFacilityModuleApi * SharemindFacilityModuleApi_new(
        SharemindFacilityModuleApiError * error,
        const char ** errorStr);

void SharemindFacilityModuleApi_free(SharemindFacilityModuleApi * modapi)
        __attribute__ ((nonnull(1)));


SHAREMIND_LIBFMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindFacilityModuleApi)
SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindFacilityModuleApi);
SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindFacilityModuleApi,,)


SharemindFacilityModule * SharemindFacilityModuleApi_newModule(
        SharemindFacilityModuleApi * modapi,
        const char * filename,
        const char * conf) __attribute__ ((nonnull(1, 2)));


/*******************************************************************************
  SharemindFacilityModule
*******************************************************************************/

SHAREMIND_LIBFMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindFacilityModule)
SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindFacilityModule);
SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindFacilityModule,,)


SharemindFacilityModuleApiError SharemindFacilityModule_init(
        SharemindFacilityModule * m) __attribute__ ((nonnull(1)));

void SharemindFacilityModule_deinit(SharemindFacilityModule * m)
        __attribute__ ((nonnull(1)));

bool SharemindFacilityModule_isInitialized(const SharemindFacilityModule * m)
        __attribute__ ((nonnull(1)));


const char * SharemindFacilityModule_filename(const SharemindFacilityModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindFacilityModule_name(const SharemindFacilityModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindFacilityModule_conf(const SharemindFacilityModule * m)
        __attribute__ ((nonnull(1)));

uint32_t SharemindFacilityModule_apiVersionInUse(
        const SharemindFacilityModule * m) __attribute__ ((nonnull(1)));

SharemindFacilityModuleApi * SharemindFacilityModule_facilityModuleApi(
        const SharemindFacilityModule * m) __attribute__ ((nonnull(1)));


/*******************************************************************************
  Clean up local macros
*******************************************************************************/

#undef SHAREMIND_LIBFMODAPI_DECLARE_ERROR_FUNCTIONS
#undef SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBFMODAPI_DECLARE_FACILITY_FUNCTIONS__


SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_LIBFMODAPI_H */
