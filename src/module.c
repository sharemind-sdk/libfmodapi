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

#include "module.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "modapi.h"


SharemindFacilityModule * SharemindFacilityModuleApi_newModule(
        SharemindFacilityModuleApi * modapi,
        const char * filename,
        const char * conf)
{
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    const SharemindFacilityModuleInfo * moduleInfo;
    size_t i;

    SharemindFacilityModule * const m =
            (SharemindFacilityModule *) malloc(sizeof(SharemindFacilityModule));
    if (unlikely(!m)) {
        SharemindFacilityModuleApi_setErrorOom(modapi);
        goto SharemindFacilityModule_new_fail_0;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(m)) {
        SharemindFacilityModuleApi_setErrorMie(modapi);
        goto SharemindFacilityModule_new_fail_1;
    }

    SHAREMIND_LIBFMODAPI_LASTERROR_INIT(m);
    SHAREMIND_TAG_INIT(m);

    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        SharemindFacilityModuleApi_setErrorOom(modapi);
        goto SharemindFacilityModule_new_fail_2;
    }

    if (likely(conf && conf[0])) {
        m->conf = strdup(conf);
        if (!m->conf) {
            SharemindFacilityModuleApi_setErrorOom(modapi);
            goto SharemindFacilityModule_new_fail_3;
        }
    } else {
        m->conf = NULL;
    }

    /* Load module: */
    m->libHandle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (unlikely(!m->libHandle)) {
        SharemindFacilityModuleApi_setError(
                    modapi,
                    SHAREMIND_FACILITY_MODULE_API_UNABLE_TO_OPEN_MODULE,
                    "dlopen() failed!");
        goto SharemindFacilityModule_new_fail_4;
    }

    /* Read module info: */
    moduleInfo =
            (const SharemindFacilityModuleInfo *) dlsym(
                m->libHandle,
                "sharemindFacilityModuleInfo");
    if (unlikely(!moduleInfo)) {
        SharemindFacilityModuleApi_setError(
                    modapi,
                    SHAREMIND_FACILITY_MODULE_API_INVALID_MODULE,
                    "The \"sharemindFacilityModuleInfo\" symbol was not found!");
        goto SharemindFacilityModule_new_fail_5;
    }

    /* Verify module name: */
    if (unlikely(!moduleInfo->moduleName[0])) {
        SharemindFacilityModuleApi_setError(
                    modapi,
                    SHAREMIND_FACILITY_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid module name!");
        goto SharemindFacilityModule_new_fail_5;
    }

    /*
      Verify module supported versions, determine API version compatibility and
      select API version to use:
    */
    if (unlikely((moduleInfo->supportedVersions[0u] == 0u)
                 || (moduleInfo->supportedVersions[15u] != 0u)))
    {
        SharemindFacilityModuleApi_setError(
                    modapi,
                    SHAREMIND_FACILITY_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid supported API list!");
        goto SharemindFacilityModule_new_fail_5;
    }
    i = 0u;
    m->apiVersion = 0u;
    do {
        if (moduleInfo->supportedVersions[i] > m->apiVersion
            && moduleInfo->supportedVersions[i]
               >= SHAREMIND_FACILITY_MODULE_API_API_MIN_VERSION
            && moduleInfo->supportedVersions[i]
               <= SHAREMIND_FACILITY_MODULE_API_API_VERSION)
        {
            m->apiVersion = moduleInfo->supportedVersions[i];
        }
    } while (moduleInfo->supportedVersions[++i] != 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SharemindFacilityModuleApi_setError(
                    modapi,
                    SHAREMIND_FACILITY_MODULE_API_API_NOT_SUPPORTED,
                    "API not supported!");
        goto SharemindFacilityModule_new_fail_5;
    }
    m->api = &gSharemindFacilityApis[m->apiVersion - 1u];

    /* Read module name: */
    m->name = strndup(moduleInfo->moduleName, sizeof(moduleInfo->moduleName));
    if (unlikely(!m->name)) {
        SharemindFacilityModuleApi_setErrorOom(modapi);
        goto SharemindFacilityModule_new_fail_5;
    }

    /* Read module version: */
    m->version = moduleInfo->moduleVersion;

    #ifndef NDEBUG
    SHAREMIND_REFS_INIT(m);
    #endif

    /* Do API specific loading: */
    SharemindFacilityModuleApi_lock(modapi);
    if (unlikely(!(*(m->api->moduleLoad))(m)))
        goto SharemindFacilityModule_new_fail_6;

    /* Register with SharemindFacilityModuleApi: */
    SharemindFacilityModule ** const p =
            SharemindFacilityModulesVector_push(&modapi->modules);
    if (unlikely(!p)) {
        SharemindFacilityModuleApi_setErrorOom(modapi);
        goto SharemindFacilityModule_new_fail_7;
    }
    (*p) = m;
    SharemindFacilityModuleApi_unlock(modapi);
    return m;

SharemindFacilityModule_new_fail_7:

    (*(m->api->moduleUnload))(m);

SharemindFacilityModule_new_fail_6:

    SharemindFacilityModuleApi_unlock(modapi);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    free(m->name);

SharemindFacilityModule_new_fail_5:

    dlclose(m->libHandle);

SharemindFacilityModule_new_fail_4:

    free(m->conf);

SharemindFacilityModule_new_fail_3:

    free(m->filename);

SharemindFacilityModule_new_fail_2:

    SHAREMIND_RECURSIVE_LOCK_DEINIT(m);

SharemindFacilityModule_new_fail_1:

    free(m);

SharemindFacilityModule_new_fail_0:

    return NULL;
}

void SharemindFacilityModule_free(SharemindFacilityModule * m) {
    assert(m);

    if (likely(m->isInitialized))
        SharemindFacilityModule_deinit(m);

    (*(m->api->moduleUnload))(m);

    SHAREMIND_TAG_DESTROY(m);
    #ifndef NDEBUG
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    #endif
    free(m->name);
    dlclose(m->libHandle);
    if (likely(m->conf))
        free(m->conf);
    free(m->filename);

    SHAREMIND_RECURSIVE_LOCK_DEINIT(m);
    free(m);
}

SharemindFacilityModuleApiError SharemindFacilityModule_init(
        SharemindFacilityModule * m)
{
    assert(m);
    SharemindFacilityModule_lock(m);
    if (m->isInitialized) {
        SharemindFacilityModule_unlock(m);
        return SHAREMIND_FACILITY_MODULE_API_OK;
    }
    const SharemindFacilityModuleApiError e = (*(m->api->moduleInit))(m);
    if (likely(e == SHAREMIND_FACILITY_MODULE_API_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }
    SharemindFacilityModule_unlock(m);
    return e;
}

void SharemindFacilityModule_deinit(SharemindFacilityModule * m) {
    assert(m);
    SharemindFacilityModule_lock(m);
    (*(m->api->moduleDeinit))(m);
    m->isInitialized = false;
    SharemindFacilityModule_unlock(m);
}

bool SharemindFacilityModule_isInitialized(const SharemindFacilityModule * m) {
    assert(m);
    SharemindFacilityModule_lockConst(m);
    const bool r = m->isInitialized;
    SharemindFacilityModule_unlockConst(m);
    return r;
}

SharemindFacilityModuleApi * SharemindFacilityModule_facilityModuleApi(
        const SharemindFacilityModule * m)
{
    assert(m);
    return m->modapi; // No locking: const after SharemindFacilityModuleApi_newModule.
}

const char * SharemindFacilityModule_filename(const SharemindFacilityModule * m)
{
    assert(m);
    return m->filename; // No locking: const after SharemindFacilityModuleApi_newModule.
}

const char * SharemindFacilityModule_name(const SharemindFacilityModule * m) {
    assert(m);
    return m->name; // No locking: const after SharemindFacilityModuleApi_newModule.
}

const char * SharemindFacilityModule_conf(const SharemindFacilityModule * m) {
    assert(m);
    assert(m->conf);
    assert(m->conf[0]);
    return m->conf; // No locking: const after SharemindFacilityModuleApi_newModule.
}

uint32_t SharemindFacilityModule_apiVersionInUse(
        const SharemindFacilityModule * m)
{
    assert(m);
    // No locking: const after SharemindFacilityModuleApi_newModule:
    return m->apiVersion;
}

#define SHAREMIND_LIBFMODAPI_MODULE_DEFINE_METHODS(name,Name) \
    SharemindFacility const * SharemindFacilityModule_find ## Name ## Facility(\
            const SharemindFacilityModule * m, \
            const char * signature) \
    { /* No locking: const after SharemindFacilityModuleApi_newModule: */ \
        assert(m); \
        return m->isInitialized \
               ? (*(m->api->find ## Name ## Facility))(m, signature) \
               : NULL; \
    }
SHAREMIND_LIBFMODAPI_MODULE_DEFINE_METHODS(module,Module)
SHAREMIND_LIBFMODAPI_MODULE_DEFINE_METHODS(pd,Pd)
SHAREMIND_LIBFMODAPI_MODULE_DEFINE_METHODS(pdpi,Pdpi)

SHAREMIND_LIBFMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindFacilityModule)
SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindFacilityModule,)

#ifndef NDEBUG
SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindFacilityModule)
#endif
