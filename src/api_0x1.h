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

#ifndef SHAREMIND_LIBFMODAPI_API_0x1_H
#define SHAREMIND_LIBFMODAPI_API_0x1_H

#include <sharemind/extern_c.h>
#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/libprocessfacility.h>
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
    SHAREMIND_FACILITY_MODULE_API_0x1_INVALID_CONFIGURATION

} SharemindFacilityModuleApi0x1Error;
#endif

#define SHAREMIND_FACILITY_MODULE_API_0x1_ERROR_ENUM \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_OK, = 0)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_OUT_OF_MEMORY,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_IMPLEMENTATION_LIMITS_REACHED,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_SHAREMIND_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_MODULE_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_GENERAL_ERROR,)) \
    ((SHAREMIND_FACILITY_MODULE_API_0x1_INVALID_CONFIGURATION,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindFacilityModuleApi0x1Error,
                             SHAREMIND_FACILITY_MODULE_API_0x1_ERROR_ENUM);


/*******************************************************************************
  MODULE
*******************************************************************************/

/** Environment passed to a Sharemind module initializer and deinitializer: */
typedef struct SharemindFacilityModuleApi0x1ModuleContext_
        SharemindFacilityModuleApi0x1ModuleContext;

/** Module facility getter signatures: */
typedef SharemindModuleApi0x1Facility const *
(* SharemindFacilityModuleApi0x1FacilityGetter)(
                SharemindFacilityModuleApi0x1ModuleContext * c,
                const char * signature);

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

    /**
      \brief Looks up a module facility with the given signature.
      \warning This field is only non-NULL in the *_init() and *_deinit()
               functions of the facility module.
    */
    SharemindFacilityModuleApi0x1FacilityGetter SHAREMIND_ICONST
            findModuleFacility;

    /**
      \brief Looks up a protection domain facility with the given signature.
      \warning This field is only non-NULL in the *_init() and *_deinit()
               functions of the facility module.
    */
    SharemindFacilityModuleApi0x1FacilityGetter SHAREMIND_ICONST findPdFacility;

    /**
      \brief Looks up a protection domain process instance facility with the
             given signature.
      \warning This field is only non-NULL in the *_init() and *_deinit()
               functions of the facility module.
    */
    SharemindFacilityModuleApi0x1FacilityGetter SHAREMIND_ICONST
            findPdpiFacility;

};


/** Module initializer function signature: */
typedef SharemindFacilityModuleApi0x1Error
(* SharemindFacilityModuleApi0x1Initializer)(
                SharemindFacilityModuleApi0x1ModuleContext * c,
                char const ** errorStr);

#define SHAREMIND_FACILITY_MODULE_API_0x1_INITIALIZER(c,errorStr) \
    SharemindFacilityModuleApi0x1Error SharemindFacilityModuleApi0x1_init( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            char const ** errorStr)


/** Module deinitializer function signature: */
typedef void (* SharemindFacilityModuleApi0x1Deinitializer)(
                SharemindFacilityModuleApi0x1ModuleContext * c);

#define SHAREMIND_FACILITY_MODULE_API_0x1_DEINITIALIZER(c) \
    void SharemindFacilityModuleApi0x1_deinit( \
            SharemindFacilityModuleApi0x1ModuleContext * c)


#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_MODULE_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility const * \
    SharemindFacilityModuleApi0x1_findModuleFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_PD_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility const * \
    SharemindFacilityModuleApi0x1_findPdFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

#define SHAREMIND_FACILITY_MODULE_API_0x1_FIND_PDPI_FACILITY(c, signature) \
    SharemindModuleApi0x1Facility const * \
    SharemindFacilityModuleApi0x1_findPdpiFacility( \
            SharemindFacilityModuleApi0x1ModuleContext * c, \
            const char * signature)

/*******************************************************************************
  Process specific instance
*******************************************************************************/

typedef struct SharemindFacilityModuleApi0x1PiWrapper_
        SharemindFacilityModuleApi0x1PiWrapper;

struct SharemindFacilityModuleApi0x1PiWrapper_ {

    /**
      A handle for module instance data. Inside SHAREMIND_syscall_context and
      others, this handle is also passed to facilities provided by this module.
    */
    void * SHAREMIND_ICONST moduleHandle;

    /**
      The module configuration string.
      \note Might be NULL if empty.
    */
    const char * SHAREMIND_ICONST conf;

    /** A handle for facility module per-process data. */
    void * processHandle;

    /** Sharemind process Id */
    SHAREMIND_ICONST SharemindProcessId processId;
};

/** Process initialization function signature */
typedef SharemindFacilityModuleApi0x1Error
(* SharemindFacilityModuleApi0x1PiStartup)(
                SharemindFacilityModuleApi0x1PiWrapper *,
                char const ** errorStr);

#define SHAREMIND_FACILITY_MODULE_API_0x1_PI_STARTUP(c,errorStr) \
    SharemindFacilityModuleApi0x1Error \
        SharemindFacilityModuleApi0x1_Pi_startup( \
            SharemindFacilityModuleApi0x1PiWrapper * c, \
            char const ** errorStr)


/** Process deinitialization function signature */
typedef void (* SharemindFacilityModuleApi0x1PiShutdown)(
        SharemindFacilityModuleApi0x1PiWrapper *);

#define SHAREMIND_FACILITY_MODULE_API_0x1_PI_SHUTDOWN(c) \
    void SharemindFacilityModuleApi0x1_Pi_shutdown( \
            SharemindFacilityModuleApi0x1PiWrapper * c)

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_API_0x1_H */
