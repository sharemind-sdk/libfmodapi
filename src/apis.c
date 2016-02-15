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

#define SHAREMIND_INTERNAL_

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
      .modulePiStartup = &SharemindFacilityModule_Pi_startup_0x1,
      .modulePiShutdown = &SharemindFacilityModule_Pi_shutdown_0x1,

      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(module,Module),
      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pd,Pd),
      SHAREMIND_LIBFMODAPI_APIS_DEFINE_METHODS(pdpi,Pdpi)
    }
};
