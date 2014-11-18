/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_APIS_H
#define SHAREMIND_LIBFMODAPI_APIS_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/extern_c.h>
#include <stdbool.h>
#include "libfmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

typedef struct {

    bool (* const moduleLoad)(SharemindFacilityModule * m);

    void (* const moduleUnload)(SharemindFacilityModule * m);


    SharemindFacilityModuleApiError (* const moduleInit)(
            SharemindFacilityModule * m);

    void (* const moduleDeinit)(SharemindFacilityModule * m);


    #define SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(name,Name) \
        SharemindFacility const * (* const find ## Name ## Facility)( \
                const SharemindFacilityModule * m, \
                const char * signature)
    SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(module,Module);
    SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pd,Pd);
    SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pdpi,Pdpi);
    #undef SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS

} SharemindFacilityApi;

extern const SharemindFacilityApi gSharemindFacilityApis[];

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_APIS_H */
