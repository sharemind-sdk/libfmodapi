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

#ifndef SHAREMIND_LIBFMODAPI_API_H
#define SHAREMIND_LIBFMODAPI_API_H

#include <sharemind/extern_c.h>
#include <stdint.h>


SHAREMIND_EXTERN_C_BEGIN

/**
  \note This struct needs a name, because otherwise we might get this warning
        for modules written in C:
            warning: non-local variable 'sharemindFacilityModuleInfo' with
            anonymous type is questionable in C++
*/
typedef struct SharemindFacilityModuleInfo_ {

    /** Unique non-empty name of the module (optionally zero-terminated): */
    const char moduleName[64];

    /** Version of the module: */
    const uint32_t moduleVersion;

    /** Non-empty zero-terminated array of up to 15 supported API versions: */
    const uint32_t supportedVersions[16];

} const SharemindFacilityModuleInfo;

#define SHAREMIND_FACILITY_MODULE_API_MODULE_INFO(name, version, ...) \
    extern const SharemindFacilityModuleInfo sharemindFacilityModuleInfo = { \
        (name), (version), { __VA_ARGS__, 0x0 } \
    }

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBFMODAPI_API_H */
