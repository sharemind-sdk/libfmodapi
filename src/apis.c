/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "apis.h"

#include "module_0x1.h"


#define SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(name,Name) \
    .find ## Name ## Facility = \
            &SharemindFacilityModule_find ## Name ## Facility_0x1

const SharemindFacilityApi gSharemindFacilityApis[] = {
    { .moduleLoad = &SharemindFacilityModule_load_0x1,
      .moduleUnload = &SharemindFacilityModule_unload_0x1,
      .moduleInit = &SharemindFacilityModule_init_0x1,
      .moduleDeinit = &SharemindFacilityModule_deinit_0x1,

      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(module,Module),
      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pd,Pd),
      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pdpi,Pdpi)
    }
};
