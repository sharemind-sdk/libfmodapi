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

#ifndef SHAREMIND_LIBFMODAPI_PROCESSINSTANCE_H
#define SHAREMIND_LIBFMODAPI_PROCESSINSTANCE_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/tag.h>
#include <sharemind/vector.h>
#include <stdbool.h>
#include "lasterror.h"
#include "libfmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

struct WrapperContext_;
typedef struct WrapperContext_ WrapperContext;
struct WrapperContext_ {
    SharemindFacilityModule * module;
    SharemindFacilityModuleApi0x1PiWrapper wrapper;
};

SHAREMIND_VECTOR_DECLARE_BODY(SharemindFacilityModulesPiVector,
                              WrapperContext *)
SHAREMIND_VECTOR_DEFINE_BODY(SharemindFacilityModulesPiVector,)
SHAREMIND_VECTOR_DECLARE_PUSH(SharemindFacilityModulesPiVector,,
                              SHAREMIND_COMMA visibility("internal"))

struct SharemindFacilityModulePis_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBFMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    SharemindFacilityModulesPiVector instances;
};

void SharemindFacilityModulePis_destroy(
        SharemindFacilityModulePis * modPis)
            __attribute__((nonnull(1)));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindFacilityModulePis,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBFMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(
        SharemindFacilityModulePis)

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_PROCESSINSTANCE_H */

