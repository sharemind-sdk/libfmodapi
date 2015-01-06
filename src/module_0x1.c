/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#define SHAREMIND_INTERNAL__

#include "module_0x1.h"

#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include "modapi.h"
#include "module.h"


typedef SharemindFacilityModuleApi0x1ModuleContext ModuleContext;
typedef SharemindFacilityModuleApi0x1Initializer ModuleInitializer;
typedef SharemindFacilityModuleApi0x1Deinitializer ModuleDeinitializer;
typedef SharemindFacilityModuleApi0x1FacilityGetter FacilityGetter;

typedef struct {
    SharemindFacilityModuleApi * fmodapi;
    ModuleContext moduleContext;
} ApiContext;

static inline ApiContext * ModuleContext_apiContext(ModuleContext * m)
{ return (ApiContext *) (((char *) m) - offsetof(ApiContext,moduleContext)); }

#define MODULE_CONTEXT_GETTER_DEFINE(Name) \
    static SharemindFacility const * ModuleContext_find ## Name ## Facility( \
             ModuleContext * m, \
             const char * signature) \
    { \
        return SharemindFacilityModuleApi_find ## Name ## Facility( \
                ModuleContext_apiContext(m)->fmodapi, \
                signature); \
    }
MODULE_CONTEXT_GETTER_DEFINE(Module)
MODULE_CONTEXT_GETTER_DEFINE(Pd)
MODULE_CONTEXT_GETTER_DEFINE(Pdpi)

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

    ApiContext apiContext = {
        m->modapi,
        {
            .moduleHandle = NULL, /* Just in case */
            .conf = m->conf,
            .findModuleFacility = &ModuleContext_findModuleFacility,
            .findPdFacility = &ModuleContext_findPdFacility,
            .findPdpiFacility = &ModuleContext_findPdpiFacility
        }
    };
    char const * errorStr = NULL;
    switch (apiData->initializer(&apiContext.moduleContext, &errorStr)) {
        case SHAREMIND_FACILITY_MODULE_API_0x1_OK:
            if (!apiContext.moduleContext.moduleHandle) {
                apiData->deinitializer(&apiContext.moduleContext);
                SharemindFacilityModule_setError(
                            m,
                            SHAREMIND_FACILITY_MODULE_API_API_ERROR,
                            "Facility module handle was not initialized!");
                return SHAREMIND_FACILITY_MODULE_API_API_ERROR;
            }
            m->moduleHandle = apiContext.moduleContext.moduleHandle;
            return SHAREMIND_FACILITY_MODULE_API_OK;
        #define SHAREMIND_EC(theirs,ours) \
            case SHAREMIND_FACILITY_MODULE_API_0x1_ ## theirs: \
                SharemindFacilityModule_setError( \
                        m, \
                        SHAREMIND_FACILITY_MODULE_API_ ## ours, \
                        errorStr \
                        ? errorStr \
                        : "Facility module returned " #theirs "!"); \
                return SHAREMIND_FACILITY_MODULE_API_ ## ours
        #define SHAREMIND_EC2(e) SHAREMIND_EC(e,e)
        SHAREMIND_EC2(OUT_OF_MEMORY);
        SHAREMIND_EC2(IMPLEMENTATION_LIMITS_REACHED);
        SHAREMIND_EC2(SHAREMIND_ERROR);
        SHAREMIND_EC2(MODULE_ERROR);
        SHAREMIND_EC(GENERAL_ERROR, MODULE_ERROR);
        SHAREMIND_EC2(INVALID_CONFIGURATION);
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

    ApiContext apiContext = {
        m->modapi,
        {
            .moduleHandle = m->moduleHandle,
            .conf = m->conf,
            .findModuleFacility = &ModuleContext_findModuleFacility,
            .findPdFacility = &ModuleContext_findPdFacility,
            .findPdpiFacility = &ModuleContext_findPdpiFacility
        }
    };
    apiData->deinitializer(&apiContext.moduleContext);
}

#define SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(name,Name) \
   SharemindFacility const * \
    SharemindFacilityModule_find ## Name ## Facility_0x1( \
            const SharemindFacilityModule * m, \
            const char * signature) \
    { \
        ApiData * const apiData = (ApiData *) m->apiData; \
        ApiContext apiContext = { \
            m->modapi, \
            { \
                .moduleHandle = m->moduleHandle, \
                .conf = m->conf, \
                .findModuleFacility = NULL, \
                .findPdFacility = NULL, \
                .findPdpiFacility = NULL \
            } \
        }; \
        return apiData->name ## FacilityGetter \
               ? (*(apiData->name ## FacilityGetter))( \
                         &apiContext.moduleContext, \
                         signature) \
               : NULL; \
    }
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(module,Module)
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(pd,Pd)
SHAREMIND_LIBFMODAPI_MODULE_0x1_DEFINE_METHODS(pdpi,Pdpi)
