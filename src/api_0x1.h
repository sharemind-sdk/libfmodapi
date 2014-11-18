/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBFMODAPI_API_0x1_H
#define SHAREMIND_LIBFMODAPI_API_0x1_H

#include <sharemind/extern_c.h>
#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/preprocessor.h>
#include "api.h"


SHAREMIND_EXTERN_C_BEGIN


#if 0
/**
  Possible return codes returned by the procedures in Sharemind facility
  modules:
*/
typedef enum {

    /** No error. */
    SHAREMIND_FACILITY_MODULE_API_0x1_OK = 0,

    /** A fatal out of memory condition occurred. */
    SHAREMIND_FACILITY_MODULE_API_0x1_OUT_OF_MEMORY,

    /** Module implementation limits reached. */
    SHAREMIND_FACILITY_MODULE_API_0x1_IMPLEMENTATION_LIMITS_REACHED,

    /** Sharemind acted in an unspecified manner. */
    SHAREMIND_FACILITY_MODULE_API_0x1_SHAREMIND_ERROR,

    /** Programming fault in the module. */
    SHAREMIND_FACILITY_MODULE_API_0x1_MODULE_ERROR,

    /** A general runtime error. */
    SHAREMIND_FACILITY_MODULE_API_0x1_GENERAL_ERROR,

    /** The module configuration given was invalid or erroneous. */
    SHAREMIND_FACILITY_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION

} SharemindFacilityModuleApi0x1Error;
#endif

#define SHAREMIND_FACILITY_MODULE_API_0x1_ERROR_ENUM \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_OK, = 0)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_OUT_OF_MEMORY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_IMPLEMENTATION_LIMITS_REACHED,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_SHAREMIND_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_MODULE_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_GENERAL_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindFacilityModuleApi0x1Error,
                             SHAREMIND_FACILITY_MODULE_API_0x1_ERROR_ENUM);


/*******************************************************************************
  MODULE
*******************************************************************************/

/** Environment passed to a Sharemind module initializer and deinitializer: */
typedef struct SharemindFacilityModuleApi0x1ModuleContext_
        SharemindFacilityModuleApi0x1ModuleContext;

struct SharemindFacilityModuleApi0x1ModuleContext_ {

    /**
      A handle for module instance data. Inside SHAREMIND_syscall_context and
      others, this handle is also passed to facilities provided by this module.
    */
    void * moduleHandle;

    /**
      The module configuration string.
      \note Might be NULL if empty.
    */
    const char * SHAREMIND_ICONST conf;

};


/** Module initializer function signature: */
typedef SharemindFacilityModuleApi0x1Error
(* SharemindFacilityModuleApi0x1Initializer)(
                SharemindFacilityModuleApi0x1ModuleContext * c);

#define SHAREMIND_FACILITY_MODULE_API_0x1_INITIALIZER(c) \
    SharemindFacilityModuleApi0x1Error SharemindFacilityModuleApi0x1_init( \
            SharemindFacilityModuleApi0x1ModuleContext * c)


/** Module deinitializer function signature: */
typedef void (* SharemindFacilityModuleApi0x1Deinitializer)(
                SharemindFacilityModuleApi0x1ModuleContext * c);

#define SHAREMIND_FACILITY_MODULE_API_0x1_DEINITIALIZER(c) \
    void SharemindFacilityModuleApi0x1_deinit( \
            SharemindFacilityModuleApi0x1ModuleContext * c)


/** Module facility getter signatures: */
typedef SharemindModuleApi0x1Facility *
(* SharemindFacilityModuleApi0x1FacilityGetter)(
                SharemindFacilityModuleApi0x1ModuleContext * c,
                const char * signature);

#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_MODULE_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility * \
    SharemindFacilityModuleApi0x1_findModuleFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_PD_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility * \
    SharemindFacilityModuleApi0x1_findPdFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_PDPI_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility * \
    SharemindFacilityModuleApi0x1_findPdpiFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_API_0x1_H */
