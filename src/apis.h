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

#ifndef SHAREMIND_LIBFMODAPI_APIS_H
#define SHAREMIND_LIBFMODAPI_APIS_H

#ifndef SHAREMIND_INTERNAL_
#error SHAREMIND_INTERNAL_
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

    SharemindFacilityModuleApiError (* const modulePiStartup)(
            SharemindFacilityModule * m,
            SharemindFacilityModuleApi0x1PiWrapper * wrapper);

    void (* const modulePiShutdown)(
            SharemindFacilityModule * m,
            void * processHandle);

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
