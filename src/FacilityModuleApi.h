/*
 * Copyright (C) Cybernetica
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

#ifndef SHAREMIND_LIBFMODAPI_FACILITYMODULEAPI_H
#define SHAREMIND_LIBFMODAPI_FACILITYMODULEAPI_H

#include <memory>
#include <sharemind/Exception.h>
#include <sharemind/ExceptionMacros.h>
#include <sharemind/facility-module-apis/api.h>
#include <sharemind/facility-module-apis/api_0x2.h>
#include <sharemind/SimpleUnorderedStringMap.h>
#include <string>
#include <type_traits>
#include <vector>


namespace sharemind {

class FacilityModuleApi {

private: /* Types: */

    class Inner;

public: /* Types: */

    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(sharemind::Exception, Exception);

    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            Exception,
            FailedToCreateFacilityException);
    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(Exception,
                                         ModuleLoadingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            FailedToDynamicallyLinkFacilityModuleException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            MissingSymbolException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            InvalidModuleNameException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            InvalidSupportedApiListInModuleException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            ApiNotSupportedException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            DuplicateFacilityException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            ModuleLoadingException,
            ApiErrorException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            ModuleLoadingException,
            FailedToInitializeModuleException);

    /** Any exceptions thrown by module initialization functions are translated
        into ModuleInitializationExceptions. */
    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(ModuleLoadingException,
                                         ModuleInitializationException);

    using ModuleVersion =
            std::remove_const<
                decltype(SharemindFacilityModuleInfo::moduleVersion)>::type;

    using ApiVersion =
            std::remove_cv<
                std::remove_extent<
                    decltype(SharemindFacilityModuleInfo::supportedVersions)
                >::type
            >::type;

    struct ModuleInfo {

        virtual ~ModuleInfo() noexcept;

        virtual std::string const & filename() const noexcept = 0;
        virtual std::string const & configuration() const noexcept = 0;
        virtual std::string const & name() const noexcept = 0;
        virtual ModuleVersion version() const noexcept = 0;
        virtual std::vector<ApiVersion> const & supportedApiVersions()
                const noexcept = 0;
        virtual ApiVersion usedApiVersion() const noexcept = 0;
    };

    #define SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(name) \
        virtual SimpleUnorderedStringMap<std::shared_ptr<void> > & \
            name ## FacilityMap() noexcept = 0; \
        virtual SimpleUnorderedStringMap<std::shared_ptr<void> > const & \
            name ## FacilityMap() const noexcept = 0

    struct ModuleFacilityFinder {
        virtual ~ModuleFacilityFinder() noexcept;
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(module);
    };

    struct PdFacilityFinder {
        virtual ~PdFacilityFinder() noexcept;
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(module);
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(pd);
    };

    struct PdpiFacilityFinder {
        virtual ~PdpiFacilityFinder() noexcept;
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(module);
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(pd);
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(pdpi);
    };

    struct ProcessFacilityFinder {
        virtual ~ProcessFacilityFinder() noexcept;
        SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTERS(process);
    };

    #undef SHAREMIND_LIBFMODAPI_DECLARE_MAP_GETTER

    using FacilitySharedPtr =
            FacilityModuleApis::v2::FacilitySharedPtr;

    using FacilityModuleFacilityFactoryContext =
            FacilityModuleApis::v2::FacilityModuleFacilityFactoryContext;
    using ModuleFacilityFactoryContext =
            FacilityModuleApis::v2::ModuleFacilityFactoryContext;
    using PdFacilityFactoryContext =
            FacilityModuleApis::v2::PdFacilityFactoryContext;
    using PdpiFacilityFactoryContext =
            FacilityModuleApis::v2::PdpiFacilityFactoryContext;
    using ProcessFacilityFactoryContext =
            FacilityModuleApis::v2::ProcessFacilityFactoryContext;

    using FacilityModuleFacilityFactory =
            FacilityModuleApis::v2::FacilityModuleFacilityFactory;
    using ModuleFacilityFactory =
            FacilityModuleApis::v2::ModuleFacilityFactory;
    using PdFacilityFactory =
            FacilityModuleApis::v2::PdFacilityFactory;
    using PdpiFacilityFactory =
            FacilityModuleApis::v2::PdpiFacilityFactory;
    using ProcessFacilityFactory =
            FacilityModuleApis::v2::ProcessFacilityFactory;

public: /* Methods: */

    FacilityModuleApi();
    FacilityModuleApi(FacilityModuleApi &&) noexcept;
    FacilityModuleApi(FacilityModuleApi const &) = delete;
    virtual ~FacilityModuleApi() noexcept;

    FacilityModuleApi & operator=(FacilityModuleApi &&) noexcept;
    FacilityModuleApi & operator=(FacilityModuleApi const &) = delete;

    void registerFacilityModuleFacility(std::string facilityName,
                                        FacilitySharedPtr facility);
    void registerFacilityModuleFacilityFactory(
            std::string facilityName,
            std::shared_ptr<FacilityModuleFacilityFactory> facilityFactory);

    void registerModuleFacility(std::string facilityName,
                                FacilitySharedPtr facility);
    void registerModuleFacilityFactory(
            std::string facilityName,
            std::shared_ptr<ModuleFacilityFactory> facilityFactory);

    void registerPdFacility(std::string facilityName,
                            FacilitySharedPtr facility);
    void registerPdFacilityFactory(
                std::string facilityName,
                std::shared_ptr<PdFacilityFactory> facilityFactory);

    void registerPdpiFacility(std::string facilityName,
                              FacilitySharedPtr facility);
    void registerPdpiFacilityFactory(
            std::string facilityName,
            std::shared_ptr<PdpiFacilityFactory> facilityFactory);

    void registerProcessFacility(std::string facilityName,
                                 FacilitySharedPtr facility);
    void registerProcessFacilityFactory(
            std::string facilityName,
            std::shared_ptr<ProcessFacilityFactory> facilityFactory);

    std::shared_ptr<ModuleInfo> addModule(std::string filename,
                                          std::string configuration);

    std::shared_ptr<ModuleFacilityFinder> createModuleFacilityFinder();

    std::shared_ptr<PdFacilityFinder> createPdFacilityFinder(
            std::shared_ptr<ModuleFacilityFinder> moduleFacilityFinder);

    std::shared_ptr<PdpiFacilityFinder> createPdpiFacilityFinder(
            std::shared_ptr<PdFacilityFinder> pdFacilityFinder);

    std::shared_ptr<ProcessFacilityFinder> createProcessFacilityFinder();

private: /* Fields: */

    std::shared_ptr<Inner> m_inner;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_LIBFMODAPI_FACILITYMODULEAPI_H */

