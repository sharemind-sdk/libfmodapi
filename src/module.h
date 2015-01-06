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
