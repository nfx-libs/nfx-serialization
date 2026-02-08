#==============================================================================
# nfx-serialization - Extensions dependencies configuration
#==============================================================================

function(nfx_serialization_fetch_extensions)
    if(NOT NFX_SERIALIZATION_WITH_JSON)
        return()
    endif()

    include(FetchContent)

    #----------------------------------------------
    # Extension library versions
    #----------------------------------------------

    set(NFX_SERIALIZATION_DEPS_NFX_CONTAINERS_VERSION "0.4.1")
    set(NFX_SERIALIZATION_DEPS_NFX_DATATYPES_VERSION  "0.4.0")
    set(NFX_SERIALIZATION_DEPS_NFX_DATETIME_VERSION   "0.4.1")

    #----------------------------------------------
    # nfx-containers
    #----------------------------------------------

    if(NOT nfx-containers_FOUND)
        set(NFX_CONTAINERS_BUILD_TESTS         OFF CACHE BOOL "Build tests"                 FORCE)
        set(NFX_CONTAINERS_BUILD_SAMPLES       OFF CACHE BOOL "Build samples"               FORCE)
        set(NFX_CONTAINERS_BUILD_BENCHMARKS    OFF CACHE BOOL "Build benchmarks"            FORCE)
        set(NFX_CONTAINERS_BUILD_DOCUMENTATION OFF CACHE BOOL "Build Doxygen documentation" FORCE)
        set(NFX_CONTAINERS_INSTALL_PROJECT     OFF CACHE BOOL "Install project"             FORCE)
        set(NFX_CONTAINERS_PACKAGE_SOURCE      OFF CACHE BOOL "Enable source package"       FORCE)
        set(NFX_CONTAINERS_PACKAGE_ARCHIVE     OFF CACHE BOOL "Enable archive package"      FORCE)
        set(NFX_CONTAINERS_PACKAGE_DEB         OFF CACHE BOOL "Enable DEB package"          FORCE)
        set(NFX_CONTAINERS_PACKAGE_RPM         OFF CACHE BOOL "Enable RPM package"          FORCE)
        set(NFX_CONTAINERS_PACKAGE_WIX         OFF CACHE BOOL "Enable WIX package"          FORCE)

        FetchContent_Declare(
            nfx-containers
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-containers.git
            GIT_TAG        ${NFX_SERIALIZATION_DEPS_NFX_CONTAINERS_VERSION}
            EXCLUDE_FROM_ALL
        )
        list(APPEND nfx_libs_to_fetch nfx-containers)
    endif()

    #----------------------------------------------
    # nfx-datatypes (Int128, Decimal)
    #----------------------------------------------

    if(NOT nfx-datatypes_FOUND)
        set(NFX_DATATYPES_BUILD_STATIC        ON  CACHE BOOL "Build static library"              FORCE)
        set(NFX_DATATYPES_BUILD_SHARED        OFF CACHE BOOL "Build shared library"              FORCE)
        set(NFX_DATATYPES_BUILD_TESTS         OFF CACHE BOOL "Build tests"                       FORCE)
        set(NFX_DATATYPES_BUILD_SAMPLES       OFF CACHE BOOL "Build samples"                     FORCE)
        set(NFX_DATATYPES_BUILD_BENCHMARKS    OFF CACHE BOOL "Build benchmarks"                  FORCE)
        set(NFX_DATATYPES_BUILD_DOCUMENTATION OFF CACHE BOOL "Build Doxygen documentation"       FORCE)
        set(NFX_DATATYPES_INSTALL_PROJECT     OFF CACHE BOOL "Install project"                   FORCE)
        set(NFX_DATATYPES_PACKAGE_SOURCE      OFF CACHE BOOL "Enable source package generation"  FORCE)
        set(NFX_DATATYPES_PACKAGE_ARCHIVE     OFF CACHE BOOL "Enable TGZ/ZIP package generation" FORCE)
        set(NFX_DATATYPES_PACKAGE_DEB         OFF CACHE BOOL "Enable DEB package generation"     FORCE)
        set(NFX_DATATYPES_PACKAGE_RPM         OFF CACHE BOOL "Enable RPM package generation"     FORCE)
        set(NFX_DATATYPES_PACKAGE_WIX         OFF CACHE BOOL "Enable WiX MSI installer"          FORCE)

        FetchContent_Declare(
            nfx-datatypes
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-datatypes.git
            GIT_TAG        ${NFX_SERIALIZATION_DEPS_NFX_DATATYPES_VERSION}
            EXCLUDE_FROM_ALL
        )
        list(APPEND nfx_libs_to_fetch nfx-datatypes)
    endif()

    #----------------------------------------------
    # nfx-datetime (DateTime, DateTimeOffset, TimeSpan)
    #----------------------------------------------

    if(NOT nfx-datetime_FOUND)
        set(NFX_DATETIME_BUILD_STATIC        ON  CACHE BOOL "Build static library"              FORCE)
        set(NFX_DATETIME_BUILD_SHARED        OFF CACHE BOOL "Build shared library"              FORCE)
        set(NFX_DATETIME_BUILD_TESTS         OFF CACHE BOOL "Build tests"                       FORCE)
        set(NFX_DATETIME_BUILD_SAMPLES       OFF CACHE BOOL "Build samples"                     FORCE)
        set(NFX_DATETIME_BUILD_BENCHMARKS    OFF CACHE BOOL "Build benchmarks"                  FORCE)
        set(NFX_DATETIME_BUILD_DOCUMENTATION OFF CACHE BOOL "Build Doxygen documentation"       FORCE)
        set(NFX_DATETIME_INSTALL_PROJECT     OFF CACHE BOOL "Install project"                   FORCE)
        set(NFX_DATETIME_PACKAGE_SOURCE      OFF CACHE BOOL "Enable source package generation"  FORCE)
        set(NFX_DATETIME_PACKAGE_ARCHIVE     OFF CACHE BOOL "Enable TGZ/ZIP package generation" FORCE)
        set(NFX_DATETIME_PACKAGE_DEB         OFF CACHE BOOL "Enable DEB package generation"     FORCE)
        set(NFX_DATETIME_PACKAGE_RPM         OFF CACHE BOOL "Enable RPM package generation"     FORCE)
        set(NFX_DATETIME_PACKAGE_WIX         OFF CACHE BOOL "Enable WiX MSI installer"          FORCE)

        FetchContent_Declare(
            nfx-datetime
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-datetime.git
            GIT_TAG        ${NFX_SERIALIZATION_DEPS_NFX_DATETIME_VERSION}
            EXCLUDE_FROM_ALL
        )
        list(APPEND nfx_libs_to_fetch nfx-datetime)
    endif()

    #----------------------------------------------
    # Fetch all declared libraries
    #----------------------------------------------

    if(nfx_libs_to_fetch)
        FetchContent_MakeAvailable(${nfx_libs_to_fetch})
    endif()
endfunction()
