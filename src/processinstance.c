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

#define SHAREMIND_INTERNAL_

#include <assert.h>
#include <sharemind/comma.h>
#include "processinstance.h"
#include "modapi.h"
#include "module.h"
#include "module_0x1.h"

static inline void WrapperContext_free(WrapperContext * context);

static inline WrapperContext * piWrapperToWrapperContext(
        SharemindFacilityModuleApi0x1PiWrapper * w)
{
    return (WrapperContext *)(void *)(((char *) w)
                                      - offsetof(WrapperContext, wrapper));
}

static bool setProcessFacility_(SharemindFacilityModuleApi0x1PiWrapper * w,
                                char const * name,
                                void * facility)
{
    assert(w);
    assert(name);
    assert(facility);
    SharemindFacilityModulePisContext * const initCtx =
            piWrapperToWrapperContext(w)->initCtx;
    if (!initCtx || !initCtx->setProcessFacility)
        return false;
    return initCtx->setProcessFacility(initCtx, name, facility);
}

SHAREMIND_VECTOR_DEFINE_INIT(SharemindFacilityModulesPiVector, static inline)
SHAREMIND_VECTOR_DEFINE_REVERSE_DESTROY_WITH(
        SharemindFacilityModulesPiVector,
        static inline,,
        free,
        WrapperContext_free(*value);)
SHAREMIND_VECTOR_DEFINE_FORCE_RESIZE(SharemindFacilityModulesPiVector,
                                     static inline,
                                     realloc)
SHAREMIND_VECTOR_DEFINE_PUSH(SharemindFacilityModulesPiVector,)

SHAREMIND_VECTOR_DEFINE_FOREACH(
        SharemindFacilityModulesVector,
        initPis,
        static inline SharemindFacilityModuleApiError,,
        SHAREMIND_COMMA SharemindFacilityModulePis * modApis
        SHAREMIND_COMMA SharemindFacilityModulePisContext * initCtx,,
        SHAREMIND_FACILITY_MODULE_API_OK,
        /* the inside of the loop */
        SharemindFacilityModule * m = *value;
        assert(m);

        WrapperContext * const context =
            (WrapperContext *) malloc(sizeof(WrapperContext));

        if (unlikely(!context))
            return SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY;

        context->module = m;
        context->initCtx = initCtx;
        context->wrapper.moduleHandle = m->moduleHandle;
        context->wrapper.conf = m->conf;
        context->wrapper.processHandle = NULL;
        context->wrapper.setProcessFacility = &setProcessFacility_;

        SharemindFacilityModuleApiError er =
            m->api->modulePiStartup(m, &context->wrapper);

        if (er != SHAREMIND_FACILITY_MODULE_API_OK) {
            free(context);
            return er;
        }

        WrapperContext ** const p = 
            SharemindFacilityModulesPiVector_push(&modApis->instances);
        if (unlikely(!p)) {
            m->api->modulePiShutdown(m, context->wrapper.processHandle);
            free(context);
            return SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY;
        }
        (*p) = context;
        )

SharemindFacilityModulePis * SharemindFacilityModuleApi_newProcessInstance(
        SharemindFacilityModuleApi * modapi,
        SharemindFacilityModulePisContext * context,
        SharemindFacilityModuleApiError * error,
        const char ** errorStr)
{
    assert(modapi);

    SharemindFacilityModulePis * const modPis =
            (SharemindFacilityModulePis *) malloc(
                sizeof(SharemindFacilityModulePis));
    if (unlikely(!modPis)) {
        if (error)
            (*error) = SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY;
        if (errorStr)
            (*errorStr) = "Out of memory!";
        return NULL;
    }

    SharemindFacilityModulesPiVector_init(&modPis->instances);
    SharemindFacilityModuleApiError err = 
        SharemindFacilityModulesVector_initPis(
            &modapi->modules,
            modPis,
            context);
    if (err != SHAREMIND_FACILITY_MODULE_API_OK) {
        if (error)
            (*error) = err;
        goto newProcessInstances_fail;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(modPis)) {
        free(modPis);
        if (error)
            (*error) = SHAREMIND_FACILITY_MODULE_API_MUTEX_ERROR;
        if (errorStr)
            (*errorStr) = "Mutex initialization error!";
        goto newProcessInstances_fail;
    }

    SHAREMIND_LIBFMODAPI_LASTERROR_INIT(modPis);
    SHAREMIND_TAG_INIT(modPis);
    if (error)
        (*error) = SHAREMIND_FACILITY_MODULE_API_OK;

    return modPis;

newProcessInstances_fail:

    SharemindFacilityModulesPiVector_destroy(&modPis->instances);
    free(modPis);

    return NULL;
}

void SharemindFacilityModulePis_destroy(
        SharemindFacilityModulePis * modPis)
{
    assert(modPis);

    SHAREMIND_TAG_DESTROY(modPis);

    SharemindFacilityModulesPiVector_destroy(&modPis->instances);

    SHAREMIND_RECURSIVE_LOCK_DEINIT(modPis);

    free(modPis);
}

static inline void WrapperContext_free(WrapperContext * context) {
    assert(context);
    assert(context->module);
    context->module->api->modulePiShutdown(context->module,
                                           context->wrapper.processHandle);
    free(context);
}

SHAREMIND_LIBFMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindFacilityModulePis)
SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindFacilityModulePis,)
