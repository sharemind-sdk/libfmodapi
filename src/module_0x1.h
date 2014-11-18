/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_MODULE_0x1_H
#define SHAREMIND_LIBFMODAPI_MODULE_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/extern_c.h>
#include <stdbool.h>
#include "libfmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

bool SharemindFacilityModule_load_0x1(SharemindFacilityModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindFacilityModule_unload_0x1(SharemindFacilityModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));


SharemindFacilityModuleApiError SharemindFacilityModule_init_0x1(
        SharemindFacilityModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindFacilityModule_deinit_0x1(SharemindFacilityModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

#define SHAREMIND_LIBFMODAPI_MODULE_0x1_DECLARE_METHODS(name,Name) \
    size_t SharemindFacilityModule_num ## Name ## Facilities_0x1( \
            const SharemindFacilityModule * m); \
    SharemindFacility const * SharemindFacilityModule_## name ## Facility_0x1( \
            const SharemindFacilityModule * m, \
            size_t index); \
    SharemindFacility const * \
    SharemindFacilityModule_find ## Name ## Facility_0x1( \
            const SharemindFacilityModule * m, \
            const char * signature)
SHAREMIND_LIBFMODAPI_MODULE_0x1_DECLARE_METHODS(module,Module);
SHAREMIND_LIBFMODAPI_MODULE_0x1_DECLARE_METHODS(pd,Pd);
SHAREMIND_LIBFMODAPI_MODULE_0x1_DECLARE_METHODS(pdpi,Pdpi);
#undef SHAREMIND_LIBFMODAPI_MODULE_0x1_DECLARE_METHODS

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_MODULE_0x1_H */

