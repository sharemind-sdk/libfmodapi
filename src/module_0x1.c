/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "module_0x1.h"

#include <assert.h>
#include <dlfcn.h>
#include "modapi.h"
#include "module.h"


typedef SharemindFacilityModuleApi0x1ModuleContext ModuleContext;
typedef SharemindFacilityModuleApi0x1Initializer ModuleInitializer;
typedef SharemindFacilityModuleApi0x1Deinitializer ModuleDeinitializer;
typedef SharemindFacilityModuleApi0x1FacilityGetter FacilityGetter;

/** API 0x1 data */
typedef struct {
    ModuleInitializer initializer;
    ModuleDeinitializer deinitializer;
    FacilityGetter moduleFacilityGetter;
    FacilityGetter pdFacilityGetter;
    FacilityGetter pdpiFacilityGetter;
} ApiData;

bool SharemindFacilityModule_load_0x1(SharemindFacilityModule * m) {
    assert(m);

    ApiData * const apiData = (ApiData *) malloc(sizeof(ApiData));
    if (unlikely(!apiData)) {
        SharemindFacilityModuleApi_setErrorOom(m->modapi);
        goto loadModule_0x1_fail_0;
    }

    /* Handle loader function: */
    apiData->initializer =
            (ModuleInitializer) dlsym(m->libHandle,
                                      "SharemindFacilityModuleApi0x1_init");
    if (unlikely(!apiData->initializer)) {
        SharemindFacilityModuleApi_setError(
                    m->modapi,
                    SHAREMIND_FACILITY_MODULE_API_API_ERROR,
                    "Missing required symbol \"SharemindFacilityModuleApi0x1_"
                    "init\"!");
        goto loadModule_0x1_fail_1;
    }

    /* Handle unloader function: */
    apiData->deinitializer =
            (ModuleDeinitializer) dlsym(m->libHandle,
                                        "SharemindFacilityModuleApi0x1_deinit");
    if (unlikely(!apiData->deinitializer)) {
        SharemindFacilityModuleApi_setError(
                    m->modapi,
                    SHAREMIND_FACILITY_MODULE_API_API_ERROR,
                    "Missing required symbol \"SharemindFacilityModuleApi0x1_"
                    "deinit\"!");
        goto loadModule_0x1_fail_1;
    }

    /* Handle facility getter functions: */
    #define FINDGETTER(name,Name) \
        apiData->name ## FacilityGetter = \
                (FacilityGetter) dlsym( \
                    m->libHandle, \
                    "SharemindFacilityModuleApi0x1_find" #Name "Facility")
    FINDGETTER(module,Module);
    FINDGETTER(pd,Pd);
    FINDGETTER(pdpi,Pdpi);
    #undef FINDGETTER
    m->apiData = (void *) apiData;
    return true;

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return false;
}

void SharemindFacilityModule_unload_0x1(SharemindFacilityModule * const m) {
    assert(m);
    assert(m->apiData);

    ApiData * const apiData = (ApiData *) m->apiData;
    free(apiData);
}

SharemindFacilityModuleApiError SharemindFacilityModule_init_0x1(
        SharemindFacilityModule * const m)
{
    ApiData * const apiData = (ApiData *) m->apiData;

    ModuleContext context = {
        .moduleHandle = NULL, /* Just in case */
        .conf = m->conf
    };
    switch (apiData->initializer(&context)) {
        case SHAREMIND_FACILITY_MODULE_API_0x1_OK:
            if (!context.moduleHandle) {
                apiData->deinitializer(&context);
                SharemindFacilityModule_setError(
                            m,
                            SHAREMIND_FACILITY_MODULE_API_API_ERROR,
                            "Facility module handle was not initialized!");
                return SHAREMIND_FACILITY_MODULE_API_API_ERROR;
            }
            m->moduleHandle = context.moduleHandle;
            return SHAREMIND_FACILITY_MODULE_API_OK;
        #define SHAREMIND_EC(theirs,ours) \
            case SHAREMIND_FACILITY_MODULE_API_0x1_ ## theirs: \
                SharemindFacilityModule_setError( \
                        m, \
                        SHAREMIND_FACILITY_MODULE_API_ ## ours, \
                        "Facility module returned " #theirs "!"); \
                return SHAREMIND_FACILITY_MODULE_API_ ## ours
        #define SHAREMIND_EC2(e) SHAREMIND_EC(e,e)
        SHAREMIND_EC2(OUT_OF_MEMORY);
        SHAREMIND_EC2(IMPLEMENTATION_LIMITS_REACHED);
        SHAREMIND_EC2(SHAREMIND_ERROR);
        SHAREMIND_EC2(MODULE_ERROR);
        SHAREMIND_EC(GENERAL_ERROR, MODULE_ERROR);
        SHAREMIND_EC(INVALID_MODULE_CONFIGURATION, MODULE_ERROR);
        #undef SHAREMIND_EC2
        #undef SHAREMIND_EC
        default:
            SharemindFacilityModule_setError(
                        m,
                        SHAREMIND_FACILITY_MODULE_API_API_ERROR,
                        "Facility module returned an unexpected error!");
            return SHAREMIND_FACILITY_MODULE_API_API_ERROR;
    }
}

void SharemindFacilityModule_deinit_0x1(SharemindFacilityModule * const m) {
    ApiData * const apiData = (ApiData *) m->apiData;
    ModuleContext context = {
        .moduleHandle = m->moduleHandle,
        .conf = m->conf
    };
    apiData->deinitializer(&context);
}

#define SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(name,Name) \
   SharemindFacility const * \
    SharemindFacilityModule_find ## Name ## Facility_0x1( \
            const SharemindFacilityModule * m, \
            const char * signature) \
    { \
        ApiData * const apiData = (ApiData *) m->apiData; \
        ModuleContext context = { \
            .moduleHandle = m->moduleHandle, \
            .conf = m->conf \
        }; \
        return apiData->name ## FacilityGetter \
               ? (*(apiData->name ## FacilityGetter))(&context, signature) \
               : NULL; \
    }
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(module,Module)
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(pd,Pd)
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(pdpi,Pdpi)