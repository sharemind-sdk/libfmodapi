/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_MODAPI_H
#define SHAREMIND_LIBFMODAPI_MODAPI_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/tag.h>
#include <sharemind/vector.h>
#include <stdbool.h>
#include "lasterror.h"
#include "libfmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

SHAREMIND_VECTOR_DEFINE_BODY(SharemindFacilityModulesVector,
                             SharemindFacilityModule *,)
SHAREMIND_VECTOR_DECLARE_PUSH(SharemindFacilityModulesVector,,
                              SharemindFacilityModule *,
                              SHAREMIND_COMMA visibility("internal"))

struct SharemindFacilityModuleApi_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBFMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    SharemindFacilityModulesVector modules;

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindFacilityModuleApi,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBFMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(
        SharemindFacilityModuleApi)


SHAREMIND_EXTERN_C_BEGIN

#endif /* SHAREMIND_LIBFMODAPI_MODAPI_H */

