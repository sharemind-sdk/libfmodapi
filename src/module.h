/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_MODULE_H
#define SHAREMIND_LIBFMODAPI_MODULE_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/refs.h>
#include <sharemind/tag.h>
#include <stdint.h>
#include "apis.h"
#include "lasterror.h"
#include "libfmodapi.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindFacilityModule_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBFMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    void * libHandle;
    char * filename;
    char * name;
    char * conf;
    uint32_t apiVersion;
    uint32_t version;
    const SharemindFacilityApi * api;

    void * apiData;
    void * moduleHandle;

    bool isInitialized;
    SharemindFacilityModuleApi * modapi;

    #ifndef NDEBUG
    SHAREMIND_REFS_DECLARE_FIELDS
    #endif

};

void SharemindFacilityModule_free(SharemindFacilityModule * m)
        __attribute__((nonnull(1), visibility("internal")));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindFacilityModule,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBFMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindFacilityModule)

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindFacilityModule)
#endif

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_MODULE_H */
