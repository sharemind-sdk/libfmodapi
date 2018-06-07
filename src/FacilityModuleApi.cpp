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

#include "FacilityModuleApi.h"


#include <algorithm>
#include <cassert>
#include <dlfcn.h>
#include <iterator>
#include <memory>
#include <sharemind/Concat.h>
#include <sharemind/compiler-support/GccNoreturn.h>
#include <sharemind/DebugOnly.h>
#include <sharemind/likely.h>
#include <sharemind/facility-module-apis/api.h>
#include <sharemind/SimpleUnorderedStringMap.h>
#include <sharemind/TemplateTypeList.h>
#include <string>
#include <type_traits>
#include <vector>


namespace sharemind {
namespace {

using FMA = FacilityModuleApi;
namespace AV = FacilityModuleApis::v2;
using AV::FacilitySharedPtr;

/**
  \return a shared_ptr<T> to target, which shares ownership with both target and
          other.
  \warning This function may throw, because it allocates a shared structure
           containing shared pointers to both.
*/
template <typename T>
std::shared_ptr<T> mergeOwnership(std::shared_ptr<T> target,
                                  std::shared_ptr<void> other)
{
    struct MergedOwnerShipContainer {
        MergedOwnerShipContainer(std::shared_ptr<T> ptr1,
                                 std::shared_ptr<void> ptr2)
            : m_ptr1(std::move(ptr1))
            , m_ptr2(std::move(ptr2))
        {}

        std::shared_ptr<T> const m_ptr1;
        std::shared_ptr<void> const m_ptr2;
    };
    auto * rPtr = target.get();
    return std::shared_ptr<T>(
                std::make_shared<MergedOwnerShipContainer>(std::move(target),
                                                           std::move(other)),
                rPtr);
}

/**
   \brief Caps the size of a std::vector to newSize using only pop_back().

   In contrast to resize() or erase() this does not put any additional
   requirements on the value_type of the vector.
*/
template <typename ... Ts>
inline void capSizeOfVector(std::vector<Ts...> & vector,
                            typename std::vector<Ts...>::size_type newSize)
{
    auto size(vector.size());
    for (; size > newSize; --size)
        vector.pop_back();
}

/** Wraps all the current (nested) exceptions so that they share ownership with
    libraryHandle, and rethrows them. */
SHAREMIND_GCC_NORETURN_PART1
void translateModuleInitializationExceptions(
        std::shared_ptr<void> libraryHandle) SHAREMIND_GCC_NORETURN_PART2
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused"
    #pragma GCC diagnostic ignored "-Wunused-function"
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-member-function"
    #endif
    class ModuleInitializationExceptionImpl final
            : public FMA::ModuleInitializationException
    {

    public: /* Methods: */

        ModuleInitializationExceptionImpl(std::shared_ptr<void> libraryHandle,
                                          std::exception_ptr exceptionPtr)
            : m_libraryHandle(std::move(libraryHandle))
            , m_exceptionPtr(std::move(exceptionPtr))
        {}

        ModuleInitializationExceptionImpl(ModuleInitializationExceptionImpl &&)
                noexcept = default;

        ModuleInitializationExceptionImpl(
                    ModuleInitializationExceptionImpl const &) noexcept
                = default;

        ModuleInitializationExceptionImpl & operator=(
                    ModuleInitializationExceptionImpl &&) noexcept = default;

        ModuleInitializationExceptionImpl & operator=(
                    ModuleInitializationExceptionImpl const &) noexcept
                = default;

        char const * what() const noexcept final override {
            try {
                std::rethrow_exception(m_exceptionPtr);
            } catch (std::exception const & e) {
                return e.what();
            } catch (...) {
                return "<non-std::exception type exception>";
            }
        }

    private: /* Fields: */

        std::shared_ptr<void> m_libraryHandle;
        std::exception_ptr m_exceptionPtr;

    }; /* class ModuleInitializationExceptionImpl */
    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
    #pragma GCC diagnostic pop

    static auto const translateCurrentException =
            [](std::shared_ptr<void> libraryHandle) {
                try {
                    throw;
                } catch (...) {
                    return ModuleInitializationExceptionImpl(
                                std::move(libraryHandle),
                                std::current_exception());
                }
            };

    try {
        throw;
    } catch (std::nested_exception const & e) {
        if (!e.nested_ptr())
            throw translateCurrentException(std::move(libraryHandle));
        auto translated(translateCurrentException(libraryHandle));
        try {
            e.rethrow_nested();
        } catch (...) {
            try {
                translateModuleInitializationExceptions(
                            std::move(libraryHandle));
            } catch (...) {
                std::throw_with_nested(std::move(translated));
            }
        }
    } catch (...) {
        throw translateCurrentException(std::move(libraryHandle));
    }
}

/** dlopen()s a shared library, wrapping it into a smart pointer which
    automatically dlclose()s it when needed. */
std::shared_ptr<void> loadModule(std::string const & filename) {
    if (auto * r = ::dlopen(filename.c_str(), RTLD_NOW | RTLD_LOCAL))
        return std::shared_ptr<void>(r,
                                     [](decltype(r) libraryHandle) noexcept
                                     { ::dlclose(libraryHandle); });
    throw FMA::FailedToDynamicallyLinkFacilityModuleException(
            concat("Failed to dlopen() facility module from file\"", filename,
                   "\"!"));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Wunused-function"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-member-function"
#endif
/** A facility factory which always returns the same facility. */
template <typename Context>
class DummyFacilityFactory final: public AV::FacilityFactory<Context> {

public: /* Methods: */

    DummyFacilityFactory(FacilitySharedPtr facility)
        : m_facility(std::move(facility))
    {}

    DummyFacilityFactory(DummyFacilityFactory &&) noexcept = default;
    DummyFacilityFactory(DummyFacilityFactory const &) noexcept = default;

    DummyFacilityFactory & operator=(DummyFacilityFactory &&) noexcept
            = default;
    DummyFacilityFactory & operator=(DummyFacilityFactory const &) noexcept
            = default;

    FacilitySharedPtr createFacility(Context const &) final override
    { return m_facility; }

private: /* Types: */

    FacilitySharedPtr m_facility;

}; /* DummyFacilityFactory */
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop

template <typename Context>
struct FacilityInfo final {

public: /* Methods: */

    FacilityInfo(std::string name,
                 std::shared_ptr<AV::FacilityFactory<Context> > facilityFactory)
            noexcept
        : m_name(std::move(name))
        , m_facilityFactory(std::move(facilityFactory))
    {}

    FacilityInfo(FacilityInfo &&) noexcept = default;
    FacilityInfo(FacilityInfo const &) = delete;

    FacilityInfo & operator=(FacilityInfo &&) noexcept = delete;
    FacilityInfo & operator=(FacilityInfo const &) = delete;

    std::string const & name() const noexcept { return m_name; }

    FacilitySharedPtr createFacility(Context const & context) const
    { return m_facilityFactory->createFacility(context); }

private: /* Fields: */

    std::string m_name;
    std::shared_ptr<AV::FacilityFactory<Context> > m_facilityFactory;

}; /* struct FacilityInfo */

#define FACILITY_FINDER_IMPL_INIT(What, what, ...) \
    m_ ## what ## Facilities( \
        [this](){ \
            decltype(m_ ## what ## Facilities) r; \
            auto const & context = \
                    static_cast<AV::What ## FacilityFactoryContext const &>( \
                            *this); \
            for (auto const & facilityInfo  \
                 : m_innerBase->m_ ## what ## Facilities) \
            { \
                try { \
                    if (auto facility = facilityInfo.createFacility(context)) {\
                        SHAREMIND_DEBUG_ONLY(auto const r2 =) \
                                r.emplace(facilityInfo.name(), \
                                          std::move(facility)); \
                        assert(r2.second); \
                    } \
                } catch (...) { \
                    std::throw_with_nested( \
                                FMA::FailedToCreateFacilityException( \
                                    concat("Failed to create " __VA_ARGS__ \
                                           " facility \"", facilityInfo.name(),\
                                           "\"!"))); \
                } \
            } \
            return r; \
        }())

#define DEFINE_FINDER_IMPL_METHOD(Name, name, ...) \
    FacilitySharedPtr find ## Name ## Facility(__VA_ARGS__) \
            const noexcept final override \
    { \
        auto const it(m_ ## name ## Facilities.find(signature)); \
        return (it != m_ ## name ## Facilities.end()) ? it->second : nullptr; \
    }

struct InnerBase {

/* Types: */

    class InitContext final
            : public AV::ModuleInitContext
            , public AV::FacilityModuleFacilityFactoryContext
    {

    public: /* Methods: */

        InitContext(InnerBase & innerBase,
                    std::string const & conf,
                    std::shared_ptr<void> const & libraryHandle)
            : m_innerBase(&innerBase)
            , m_configuration(conf)
            , m_libraryHandle(libraryHandle)
            , FACILITY_FINDER_IMPL_INIT(FacilityModule,
                                        facilityModule,
                                        "facility module")
        {}

        std::string const & moduleConfigurationString()
                const noexcept final override
        { return m_configuration; }

        DEFINE_FINDER_IMPL_METHOD(FacilityModule,
                                  facilityModule,
                                  char const * signature)
        DEFINE_FINDER_IMPL_METHOD(FacilityModule,
                                  facilityModule,
                                  std::string const & signature)

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
        #define DEFINE_INITCONTEXT_REGISTER_METHODS(What,what,...) \
            void register ## What ## Facility( \
                        std::string facilityName, \
                        FacilitySharedPtr facility) final override \
            { \
                m_innerBase->register ## What ## Facility( \
                        std::move(facilityName), \
                        mergeOwnership(std::move(facility), m_libraryHandle)); \
                __VA_ARGS__ \
            } \
            void register ## What ## FacilityFactory( \
                    std::string facilityName, \
                    std::shared_ptr<AV::What ## FacilityFactory> factory) \
                final override \
            { \
                struct MergedOwnershipFactory: AV::What ## FacilityFactory { \
                    using F = AV::What ## FacilityFactory; \
                    MergedOwnershipFactory(std::shared_ptr<void> libraryHandle,\
                                           std::shared_ptr<F> factory) noexcept\
                        : m_libraryHandle(std::move(libraryHandle)) \
                        , m_factory(std::move(factory)) \
                    {} \
                    FacilitySharedPtr createFacility( \
                            AV::What ## FacilityFactoryContext const & context)\
                            final override \
                    { \
                        return mergeOwnership( \
                                    m_factory->createFacility(context), \
                                    m_libraryHandle); \
                    } \
                    std::shared_ptr<void> const m_libraryHandle; \
                    std::shared_ptr<F> const m_factory; \
                }; \
                m_innerBase->register ## What ## FacilityFactory( \
                        std::move(facilityName), \
                        std::make_shared<MergedOwnershipFactory>( \
                                m_libraryHandle, \
                                std::move(factory))); \
                __VA_ARGS__ \
            }
        DEFINE_INITCONTEXT_REGISTER_METHODS(FacilityModule,facilityModule,
            auto & container = m_innerBase->m_facilityModuleFacilities;
            try { // Create the facility for the current module as well:
                auto const & lastFactory = container.back();
                m_facilityModuleFacilities.emplace(
                        lastFactory.name(),
                        lastFactory.createFacility(*this));
            } catch (...) {
                container.pop_back();
            }
        )
        DEFINE_INITCONTEXT_REGISTER_METHODS(Module,module,)
        DEFINE_INITCONTEXT_REGISTER_METHODS(Pd,pd,)
        DEFINE_INITCONTEXT_REGISTER_METHODS(Pdpi,pdpi,)
        DEFINE_INITCONTEXT_REGISTER_METHODS(Process,process,)
        #undef DEFINE_INITCONTEXT_REGISTER_METHODS
        #pragma GCC diagnostic pop

    private: /* Fields: */

        InnerBase * const m_innerBase;
        std::string const & m_configuration;
        std::shared_ptr<void> const & m_libraryHandle;
        SimpleUnorderedStringMap<FacilitySharedPtr>
                m_facilityModuleFacilities;

    }; /* class InitContext */

    class FacilityModule final
            : public FacilityModuleApi::ModuleInfo
    {

    public: /* Methods: */

        FacilityModule(InnerBase & innerBase,
                       std::string filename,
                       std::string configuration)
            : m_filename(std::move(filename))
            , m_configuration(std::move(configuration))
            , m_libraryHandle(loadModule(m_filename))
            , m_moduleInfo(
                [this]() {
                    if (auto const * moduleInfo =
                            static_cast<::SharemindFacilityModuleInfo const *>(
                                ::dlsym(m_libraryHandle.get(),
                                        "sharemindFacilityModuleInfo")))
                        return moduleInfo;
                    throw FMA::MissingSymbolException(
                            concat("The required \""
                                   "sharemindFacilityModuleInfo\" symbol is "
                                   "missing in the module \"", m_filename,
                                   "\"!"));
                }())
            , m_moduleName(
                [this]() {
                    std::string r(m_moduleInfo->moduleName,
                                  std::find(m_moduleInfo->moduleName,
                                            m_moduleInfo->moduleName + 64u,
                                            '\0'));
                    if (unlikely(r.empty()))
                        throw FMA::InvalidModuleNameException(
                                concat("Invalid module name specified in the "
                                       "module info of the module \"",
                                       m_filename, "\"!"));
                    return r;
                }())
            , m_moduleSupportedVersions(
                [this]() {
                    auto const numSupportedVersionsInModule =
                        [this]() -> unsigned {
                            for (unsigned i = 0u; i < 16u; i++)
                                if (m_moduleInfo->supportedVersions[i] == 0u)
                                    return i;
                            return 16u;
                        }();
                    if (unlikely((numSupportedVersionsInModule == 0u)
                                 || (numSupportedVersionsInModule > 15u)))
                        throw FMA::InvalidSupportedApiListInModuleException(
                                concat("Invalid supported API list in module "
                                       "info of module \"", m_filename, "\"!"));
                    std::remove_const<decltype(m_moduleSupportedVersions)>::type
                            r(numSupportedVersionsInModule);
                    for (unsigned i = 0u; i < numSupportedVersionsInModule; ++i)
                        r[i] = m_moduleInfo->supportedVersions[i];
                    std::sort(r.begin(), r.end());
                    return r;
                }())
            , m_usedApiVersion(
                [this]() {
                    assert(!m_moduleSupportedVersions.empty());
                    for (auto const & version : m_moduleSupportedVersions) {
                        assert(version > 0u);
                        if (version == 0x2)
                            return version;
                    }
                    std::ostringstream oss;
                    oss << "None of the API versions advertised by the "
                           "module \"" << m_filename << "\" (";
                    std::copy(m_moduleSupportedVersions.begin(),
                              m_moduleSupportedVersions.end(),
                              std::ostream_iterator<FMA::ApiVersion>(oss,
                                                                     ", "));
                    oss << ") are supported by the implementation, which "
                           "only supports version 2!";
                    throw FMA::ApiNotSupportedException(oss.str());
                }())
        {
            auto * const moduleInfoV2 =
                    static_cast<AV::FacilityModuleInfo *>(
                        ::dlsym(m_libraryHandle.get(),
                                "sharemindFacilityModuleInfo_v2"));
            if (!moduleInfoV2)
                throw FMA::MissingSymbolException(
                        concat("The required \""
                               "sharemindFacilityModuleInfo_v2\" symbol is "
                               "missing in the module \"", m_filename, "\"!"));
            if (auto const initFun = moduleInfoV2->initializeModule) {
                auto const originalFacilityModuleFacilitiesSize(
                        innerBase.m_facilityModuleFacilities.size());
                auto const originalModuleFacilitiesSize(
                        innerBase.m_moduleFacilities.size());
                auto const originalPdFacilitiesSize(
                        innerBase.m_pdFacilities.size());
                auto const originalPdpiFacilitiesSize(
                        innerBase.m_pdpiFacilities.size());
                auto const originalProcessFacilitiesSize(
                        innerBase.m_processFacilities.size());
                try {
                    InitContext initContext(innerBase,
                                            m_configuration,
                                            m_libraryHandle);
                    try {
                        initFun(initContext);
                    } catch (...) {
                        /* The exceptions thrown by the module might get
                           handled AFTER the destruction of this class.
                           As long as any such exception is alive, the
                           dynamic library must not be unloaded. Otherwise,
                           we risk unmapping code and data areas used by
                           these exceptions, leading to undefined behavior
                           when the exceptions are used. Hence we wrap the
                           shared library handle with the exceptions: */
                        translateModuleInitializationExceptions(
                                    m_libraryHandle);
                    }
                } catch (...) {
                    // Rollback added facilities/facility factories:
                    capSizeOfVector(innerBase.m_facilityModuleFacilities,
                                    originalFacilityModuleFacilitiesSize);
                    capSizeOfVector(innerBase.m_moduleFacilities,
                                    originalModuleFacilitiesSize);
                    capSizeOfVector(innerBase.m_pdFacilities,
                                    originalPdFacilitiesSize);
                    capSizeOfVector(innerBase.m_pdpiFacilities,
                                    originalPdpiFacilitiesSize);
                    capSizeOfVector(innerBase.m_processFacilities,
                                    originalProcessFacilitiesSize);

                    std::throw_with_nested(
                                FMA::FailedToInitializeModuleException());
                }
            } else {
                throw FMA::ApiErrorException(
                            concat("Invalid (NULL) API version 2 module "
                                   "initializer given in the module \"",
                                   m_filename, "\"!"));
            }
        }

        FacilityModule(FacilityModule &&) = delete;
        FacilityModule(FacilityModule const &) = delete;

        ~FacilityModule() noexcept final override = default;

        FacilityModule & operator=(FacilityModule &&) = delete;
        FacilityModule & operator=(FacilityModule const &) = delete;

        /* FacilityModuleApi::ModuleInfo overrides: */
        std::string const & filename() const noexcept final override
        { return m_filename; }

        std::string const & configuration() const noexcept final override
        { return m_configuration; }

        std::string const & name() const noexcept final override
        { return m_moduleName; }

        FMA::ModuleVersion version() const noexcept final override
        { return m_moduleInfo->moduleVersion; }


        std::vector<FMA::ApiVersion> const & supportedApiVersions()
                const noexcept final override
        { return m_moduleSupportedVersions; }

        FMA::ApiVersion usedApiVersion() const noexcept final override
        { return m_usedApiVersion; }

    private: /* Fields: */

        std::string const m_filename;
        std::string const m_configuration;
        std::shared_ptr<void> const m_libraryHandle;
        SharemindFacilityModuleInfo const * const m_moduleInfo;
        std::string const m_moduleName;
        std::vector<FMA::ApiVersion> const m_moduleSupportedVersions;
        FMA::ApiVersion const m_usedApiVersion;

    }; /* class FacilityModule */

/* Methods: */

    InnerBase() = default;
    InnerBase(InnerBase &&) = delete;
    InnerBase(InnerBase const &) = delete;

    InnerBase & operator=(InnerBase &&) = delete;
    InnerBase & operator=(InnerBase const &) = delete;

    std::shared_ptr<FacilityModuleApi::ModuleInfo> addModule(
            std::string filename,
            std::string configuration)
    {
        auto module(std::make_shared<FacilityModule>(
                        *this,
                        std::move(filename),
                        std::move(configuration)));
        auto r(std::static_pointer_cast<FacilityModuleApi::ModuleInfo>(
                   module));
        m_modules.emplace_back(std::move(module));
        return r;
    }

    #define DEFINE_INNERBASE_REGISTER_METHODS(What,what,E,e) \
        void register ## What ## Facility(std::string facilityName, \
                                          FacilitySharedPtr facility) \
        { \
            using Factory = \
                    DummyFacilityFactory< \
                        AV::What ## FacilityFactoryContext>; \
            return register ## What ## FacilityFactory( \
                        std::move(facilityName), \
                        std::make_shared<Factory>(std::move(facility))); \
        } \
        void register ## What ## FacilityFactory( \
                std::string facilityName, \
                std::shared_ptr<AV::What ## FacilityFactory> factory) \
        { \
            auto & container = m_ ## what ## Facilities; \
            for (auto const & facilityInfo : container) \
                if (facilityInfo.name() == facilityName) \
                    throw FMA::DuplicateFacilityException( \
                                concat(E " facility or " e \
                                       " facility factory already " \
                                       "registered!")); \
            container.emplace_back(std::move(facilityName), \
                                   std::move(factory)); \
        }
    DEFINE_INNERBASE_REGISTER_METHODS(FacilityModule,
                                      facilityModule,
                                      "Facility module",
                                      "facility module")
    DEFINE_INNERBASE_REGISTER_METHODS(Module,module,"Module","module")
    DEFINE_INNERBASE_REGISTER_METHODS(Pd,pd,"PD", "PD")
    DEFINE_INNERBASE_REGISTER_METHODS(Pdpi,pdpi,"PDPI", "PDPI")
    DEFINE_INNERBASE_REGISTER_METHODS(Process,process,"Process","process")
    #undef DEFINE_INNERBASE_REGISTER_METHODS

/* Fields: */

    std::vector<std::shared_ptr<FacilityModule> > m_modules;
    std::vector<FacilityInfo<AV::FacilityModuleFacilityFactoryContext> >
            m_facilityModuleFacilities;
    std::vector<FacilityInfo<AV::ModuleFacilityFactoryContext> >
            m_moduleFacilities;
    std::vector<FacilityInfo<AV::PdFacilityFactoryContext> >
            m_pdFacilities;
    std::vector<FacilityInfo<AV::PdpiFacilityFactoryContext> >
            m_pdpiFacilities;
    std::vector<FacilityInfo<AV::ProcessFacilityFactoryContext> >
            m_processFacilities;

}; /* struct InnerBase { */

#define DEFINE_FINDER_CHAINING_MEMBER(Name, name, ...) \
    FacilitySharedPtr find ## Name ## Facility(__VA_ARGS__) \
            const noexcept final override \
    { \
        auto const & map = m_chainedFacilityFinder->name ## FacilityMap(); \
        auto const it(map.find(signature)); \
        return (it != map.end()) ? it->second : nullptr; \
    }
#define DEFINE_FINDER_CHAINING_MEMBERS(Name, name) \
    public: /* Methods: */ \
        DEFINE_FINDER_CHAINING_MEMBER(Name, name, const char * signature) \
        DEFINE_FINDER_CHAINING_MEMBER(Name, name, \
                                      std::string const & signature) \
        SimpleUnorderedStringMap<FacilitySharedPtr> & \
                name ## FacilityMap() noexcept final override \
        { return m_chainedFacilityFinder->name ## FacilityMap(); } \
        SimpleUnorderedStringMap<FacilitySharedPtr> const & \
                name ## FacilityMap() const noexcept final override \
        { return m_chainedFacilityFinder->name ## FacilityMap(); }

#define DEFINE_FACILITY_FINDER_IMPL(Name,name,...) \
    class Name ## FacilityFinderImpl final \
            : public FacilityModuleApi::Name ## FacilityFinder \
            , public AV::Name ## FacilityFactoryContext \
    { \
    public: /* Methods: */ \
        DEFINE_FINDER_IMPL_METHOD(Name, name, const char * signature) \
        DEFINE_FINDER_IMPL_METHOD(Name, name, std::string const & signature) \
        SimpleUnorderedStringMap<FacilitySharedPtr> & \
                name ## FacilityMap() noexcept final override \
        { return m_ ## name ## Facilities; } \
        SimpleUnorderedStringMap<FacilitySharedPtr> const & \
                name ## FacilityMap() const noexcept final override \
        { return m_ ## name ## Facilities; } \
    private: /* Fields: */ \
        std::shared_ptr<InnerBase> m_innerBase; \
        SimpleUnorderedStringMap<FacilitySharedPtr> m_ ## name ## Facilities; \
    __VA_ARGS__ \
    };
#define DEFINE_REGULAR_FACILITY_FINDER_IMPL(Name, name) \
    DEFINE_FACILITY_FINDER_IMPL(Name,name, \
        public: /* Methods: */ \
            Name ## FacilityFinderImpl(std::shared_ptr<InnerBase> innerBase) \
                : m_innerBase(std::move(innerBase)) \
                , FACILITY_FINDER_IMPL_INIT(Name, name, #name) \
            {} \
    )
#define DEFINE_CHAINED_FACILITY_FINDER_IMPL(Name,name,CName,cname,...) \
    DEFINE_FACILITY_FINDER_IMPL(Name, name, \
        public: /* Methods: */ \
            Name ## FacilityFinderImpl( \
                    std::shared_ptr<InnerBase> innerBase, \
                    std::shared_ptr<FMA::CName ## FacilityFinder> \
                            cname ## FacilityFinder) \
                : m_innerBase(std::move(innerBase)) \
                , FACILITY_FINDER_IMPL_INIT(Name, name, #name) \
                , m_chainedFacilityFinder(std::move(cname ## FacilityFinder)) \
            {} \
        private: /* Fields: */ \
            std::shared_ptr<FMA::CName ## FacilityFinder> \
                    m_chainedFacilityFinder; \
        __VA_ARGS__ \
    )

DEFINE_REGULAR_FACILITY_FINDER_IMPL(Module, module)
DEFINE_CHAINED_FACILITY_FINDER_IMPL(Pd, pd, Module, module,
        DEFINE_FINDER_CHAINING_MEMBERS(Module, module))
DEFINE_CHAINED_FACILITY_FINDER_IMPL(Pdpi, pdpi, Pd, pd,
        DEFINE_FINDER_CHAINING_MEMBERS(Module, module)
        DEFINE_FINDER_CHAINING_MEMBERS(Pd, pd))
DEFINE_REGULAR_FACILITY_FINDER_IMPL(Process,process)

} // anonymous namespace


class FacilityModuleApi::Inner final: public InnerBase {};


SHAREMIND_DEFINE_EXCEPTION_NOINLINE(sharemind::Exception,
                                    FacilityModuleApi::,
                                    Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        Exception,
        FacilityModuleApi::,
        FailedToCreateFacilityException);
SHAREMIND_DEFINE_EXCEPTION_NOINLINE(Exception,
                                    FacilityModuleApi::,
                                    ModuleLoadingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        FailedToDynamicallyLinkFacilityModuleException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        MissingSymbolException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        InvalidModuleNameException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        InvalidSupportedApiListInModuleException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        ApiNotSupportedException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        DuplicateFacilityException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        ApiErrorException);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        FailedToInitializeModuleException,
        "Failed to initialize module!");
SHAREMIND_DEFINE_EXCEPTION_NOINLINE(
        ModuleLoadingException,
        FacilityModuleApi::,
        ModuleInitializationException);


FacilityModuleApi::ModuleInfo::~ModuleInfo() noexcept = default;


FacilityModuleApi::ModuleFacilityFinder::~ModuleFacilityFinder() noexcept
        = default;


FacilityModuleApi::PdFacilityFinder::~PdFacilityFinder() noexcept = default;


FacilityModuleApi::PdpiFacilityFinder::~PdpiFacilityFinder() noexcept = default;


FacilityModuleApi::ProcessFacilityFinder::~ProcessFacilityFinder() noexcept
        = default;


FacilityModuleApi::FacilityModuleApi()
    : m_inner(std::make_shared<Inner>())
{}

FacilityModuleApi::FacilityModuleApi(FacilityModuleApi &&) noexcept = default;

FacilityModuleApi::~FacilityModuleApi() noexcept = default;

FacilityModuleApi & FacilityModuleApi::operator=(FacilityModuleApi &&) noexcept
        = default;

#define DEFINE_FMA_REGISTER_METHODS(What,what) \
    void FacilityModuleApi::register ## What ## Facility( \
                std::string facilityName, \
                FacilitySharedPtr facility) \
    { \
        return m_inner->register ## What ## Facility(std::move(facilityName), \
                                                     std::move(facility)); \
    } \
    void FacilityModuleApi::register ## What ## FacilityFactory( \
            std::string facilityName, \
            std::shared_ptr<AV::What ## FacilityFactory> factory) \
    { \
        return m_inner->register ## What ## FacilityFactory( \
                    std::move(facilityName), \
                    std::move(factory)); \
    }
DEFINE_FMA_REGISTER_METHODS(FacilityModule, facilityModule)
DEFINE_FMA_REGISTER_METHODS(Module,module)
DEFINE_FMA_REGISTER_METHODS(Pd,pd)
DEFINE_FMA_REGISTER_METHODS(Pdpi,pdpi)
DEFINE_FMA_REGISTER_METHODS(Process,process)
#undef DEFINE_FMA_REGISTER_METHODS

std::shared_ptr<FacilityModuleApi::ModuleInfo> FacilityModuleApi::addModule(
        std::string filename,
        std::string configuration)
{ return m_inner->addModule(std::move(filename), std::move(configuration)); }

std::shared_ptr<FacilityModuleApi::ModuleFacilityFinder>
FacilityModuleApi::createModuleFacilityFinder()
{ return std::make_shared<ModuleFacilityFinderImpl>(m_inner); }

std::shared_ptr<FacilityModuleApi::PdFacilityFinder>
FacilityModuleApi::createPdFacilityFinder(
        std::shared_ptr<ModuleFacilityFinder> moduleFacilityFinder)
{
    return std::make_shared<PdFacilityFinderImpl>(
                m_inner,
                std::move(moduleFacilityFinder));
}

std::shared_ptr<FacilityModuleApi::PdpiFacilityFinder>
FacilityModuleApi::createPdpiFacilityFinder(
        std::shared_ptr<PdFacilityFinder> pdFacilityFinder)
{
    return std::make_shared<PdpiFacilityFinderImpl>(
                m_inner,
                std::move(pdFacilityFinder));
}

std::shared_ptr<FacilityModuleApi::ProcessFacilityFinder>
FacilityModuleApi::createProcessFacilityFinder()
{ return std::make_shared<ProcessFacilityFinderImpl>(m_inner); }

} // namespace sharemind {
