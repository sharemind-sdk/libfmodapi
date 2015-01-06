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

#include "modapi.h"

#include <stdlib.h>
#include "module.h"


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindFacilityModuleApiError,
                                      SHAREMIND_FACILITY_MODULE_API_ERROR_ENUM)

SHAREMIND_VECTOR_DEFINE_INIT(SharemindFacilityModulesVector, static inline)
SHAREMIND_VECTOR_DEFINE_REVERSE_DESTROY_WITH(
        SharemindFacilityModulesVector,
        static inline,,
        free,
        SharemindFacilityModule_free(*value);)
SHAREMIND_VECTOR_DEFINE_FORCE_RESIZE(SharemindFacilityModulesVector,
                                     static inline,
                                     realloc)
SHAREMIND_VECTOR_DEFINE_PUSH(SharemindFacilityModulesVector,)

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
    SharemindFacilityModulesVector_init(&fmodapi->modules);
    return fmodapi;
}

void SharemindFacilityModuleApi_free(SharemindFacilityModuleApi * fmodapi) {
    assert(fmodapi);

    SHAREMIND_TAG_DESTROY(fmodapi);

    SharemindFacilityModulesVector_destroy(&fmodapi->modules);

    SHAREMIND_RECURSIVE_LOCK_DEINIT(fmodapi);

    free(fmodapi);
}

#define SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(name,Name) \
    SHAREMIND_VECTOR_DEFINE_FOREACH( \
            SharemindFacilityModulesVector, \
            find ## Name ## Facility, \
            static inline SharemindFacility const * , \
            const, \
            SHAREMIND_COMMA const char * signature,, \
            NULL, \
            assert(*value); \
            SharemindFacility const * const result = \
                    SharemindFacilityModule_find ## Name ## Facility( \
                            *value, \
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
                SharemindFacilityModulesVector_find ## Name ## Facility( \
                        &m->modules, \
                        signature); \
        SharemindFacilityModuleApi_unlockConst(m); \
        return sc; \
    }

SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(module,Module)
SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(pd,Pd)
SHAREMIND_LIBFMODAPI_MODAPI_DEFINE_FUNCTIONS(pdpi,Pdpi)


SHAREMIND_LIBFMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindFacilityModuleApi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindFacilityModuleApi,)
