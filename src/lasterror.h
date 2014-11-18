/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_LASTERROR_H
#define SHAREMIND_LIBFMODAPI_LASTERROR_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include <sharemind/comma.h>
#include <sharemind/lasterror.h>
#include "libfmodapi.h"


#define SHAREMIND_LIBFMODAPI_LASTERROR_FIELDS \
    SHAREMIND_LASTERROR_DECLARE_FIELDS(SharemindFacilityModuleApiError)

#define SHAREMIND_LIBFMODAPI_LASTERROR_INIT(className) \
    SHAREMIND_LASTERROR_INIT(className, SHAREMIND_FACILITY_MODULE_API_OK)

#define SHAREMIND_LIBFMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(ClassName) \
    SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE( \
            ClassName,, \
            SharemindFacilityModuleApiError, \
            SHAREMIND_COMMA visibility("internal")) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE( \
            ClassName, \
            Oom,, \
            SHAREMIND_COMMA visibility("internal")) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE( \
            ClassName, \
            Oor,, \
            SHAREMIND_COMMA visibility("internal")) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE( \
            ClassName, \
            Mie,, \
            SHAREMIND_COMMA visibility("internal"))

#define SHAREMIND_LIBFMODAPI_LASTERROR_FUNCTIONS_DEFINE(ClassName) \
    SHAREMIND_LASTERROR_PUBLIC_FUNCTIONS_DEFINE( \
            ClassName,, \
            SharemindFacilityModuleApiError, \
            SHAREMIND_FACILITY_MODULE_API_OK) \
    SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DEFINE( \
            ClassName,, \
            SharemindFacilityModuleApiError, \
            SHAREMIND_FACILITY_MODULE_API_OK) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DEFINE( \
            ClassName, \
            Oom,, \
            SHAREMIND_FACILITY_MODULE_API_OUT_OF_MEMORY, \
            "Out of memory!") \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DEFINE( \
            ClassName, \
            Oor,, \
            SHAREMIND_FACILITY_MODULE_API_IMPLEMENTATION_LIMITS_REACHED, \
            "Implementation limits reached!") \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DEFINE( \
            ClassName, \
            Mie,, \
            SHAREMIND_FACILITY_MODULE_API_MUTEX_ERROR, \
            "Mutex initialization error!")

#endif /* SHAREMIND_LIBFMODAPI_LASTERROR_H */

