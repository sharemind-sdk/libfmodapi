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

#ifndef SHAREMIND_LIBFMODAPI_LASTERROR_H
#define SHAREMIND_LIBFMODAPI_LASTERROR_H

#ifndef SHAREMIND_INTERNAL_
#error SHAREMIND_INTERNAL_
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

