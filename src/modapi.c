/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "modapi.h"

#include <stdlib.h>


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindFacilityModuleApiError,
                                      SHAREMIND_FACILITY_MODULE_API_ERROR_ENUM)


SharemindFacilityModuleApi * SharemindFacilityModuleApi_new(
        SharemindFacilityModuleApiError * error,
        const char ** errorStr)
{
    SharemindFacilityModuleApi * const fmodapi =
            (SharemindFacilityModuleApi *) malloc(
                sizeof(SharemindFacilityModuleApi));
    if (unlikely(!fmodapi)) {
        if (error)
            (*error) = SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY;
        if (errorStr)
            (*errorStr) = "Out of memory!";
        return NULL;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(fmodapi)) {
        free(fmodapi);
        if (error)
            (*error) = SHAREMIND_FACILITY_MODULE_API_MUTEX_ERROR;
        if (errorStr)
            (*errorStr) = "Mutex initialization error!";
        return NULL;
    }

    SHAREMIND_LIBFMODAPI_LASTERROR_INIT(fmodapi);
    SHAREMIND_TAG_INIT(fmodapi);
    SharemindFacilityModulesSet_init(&fmodapi->modules);
    return fmodapi;
}

void SharemindFacilityModuleApi_free(SharemindFacilityModuleApi * fmodapi) {
    assert(fmodapi);

    SHAREMIND_TAG_DESTROY(fmodapi);

    while (fmodapi->modules.size > 0u)
        SharemindFacilityModule_free(
                    *SharemindFacilityModulesSet_at(&fmodapi->modules, 0u));

    SHAREMIND_RECURSIVE_LOCK_DEINIT(fmodapi);

    free(fmodapi);
}

#define SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(name,Name) \
    SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE( \
            static inline SharemindFacility const * , \
            SharemindFacilityModulesSet, \
            find ## Name ## Facility, \
            SHAREMIND_COMMA const char * signature,, \
            NULL, \
            assert(item->key); \
            SharemindFacility const * const result = \
                    SharemindFacilityModule_find ## Name ## Facility( \
                            item->key, \
                            signature); \
            if (result) \
                return result;) \
    SharemindFacility const * \
    SharemindFacilityModuleApi_find ## Name ## Facility( \
            const SharemindFacilityModuleApi * m,\
            const char * signature) \
    { \
        assert(m); \
        SharemindFacilityModuleApi_lockConst(m); \
        SharemindFacility const * const sc = \
                SharemindFacilityModulesSet_foreach_with_find ## Name ## \
                                Facility(&m->modules, signature); \
        SharemindFacilityModuleApi_unlockConst(m); \
        return sc; \
    }

SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(module,Module)
SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(pd,Pd)
SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(pdpi,Pdpi)


SHAREMIND_LIBFMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindFacilityModuleApi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindFacilityModuleApi,)
