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

#ifndef SHAREMIND_LIBFMODAPI_MODULE_0x1_H
#define SHAREMIND_LIBFMODAPI_MODULE_0x1_H

#ifndef SHAREMIND_INTERNAL_
#error SHAREMIND_INTERNAL_
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

SharemindFacilityModuleApiError SharemindFacilityModule_Pi_startup_0x1(
        SharemindFacilityModule * m,
        SharemindFacilityModuleApi0x1PiWrapper * wrapper)
        __attribute__ ((nonnull(1,2), visibility("internal")));

void SharemindFacilityModule_Pi_shutdown_0x1(
        SharemindFacilityModule * m,
        SharemindFacilityModuleApi0x1PiWrapper * wrapper)
        __attribute__ ((nonnull(1,2), visibility("internal")));

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

