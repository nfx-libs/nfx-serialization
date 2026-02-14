#==============================================================================
# nfx-serialization - Dependencies configuration
#==============================================================================

#----------------------------------------------
# Output configuration
#----------------------------------------------

set(_SAVED_CMAKE_REQUIRED_QUIET    ${CMAKE_REQUIRED_QUIET})
set(_SAVED_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(_SAVED_CMAKE_FIND_QUIETLY      ${CMAKE_FIND_QUIETLY})

set(CMAKE_REQUIRED_QUIET    ON     )
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE) # [ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG]
set(CMAKE_FIND_QUIETLY      ON     )

#----------------------------------------------
# Dependency versions
#----------------------------------------------

set(NFX_DEPS_NFX_JSON_VERSION "1.4.1")

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

if(DEFINED ENV{CI})
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
else()
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
set(FETCHCONTENT_QUIET OFF)

# --- nfx-json ---
if(NFX_SERIALIZATION_WITH_JSON)
    find_package(nfx-json ${NFX_DEPS_NFX_JSON_VERSION} QUIET)
    if(NOT nfx-json_FOUND)
        set(NFX_JSON_BUILD_STATIC        ON  CACHE BOOL "")
        set(NFX_JSON_BUILD_SHARED        OFF CACHE BOOL "")
        set(NFX_JSON_BUILD_TESTS         OFF CACHE BOOL "")
        set(NFX_JSON_BUILD_SAMPLES       OFF CACHE BOOL "")
        set(NFX_JSON_BUILD_BENCHMARKS    OFF CACHE BOOL "")
        set(NFX_JSON_BUILD_DOCUMENTATION OFF CACHE BOOL "")
        set(NFX_JSON_INSTALL_PROJECT     OFF CACHE BOOL "")
        set(NFX_JSON_PACKAGE_SOURCE      OFF CACHE BOOL "")
        set(NFX_JSON_PACKAGE_ARCHIVE     OFF CACHE BOOL "")
        set(NFX_JSON_PACKAGE_DEB         OFF CACHE BOOL "")
        set(NFX_JSON_PACKAGE_RPM         OFF CACHE BOOL "")

        FetchContent_Declare(
            nfx-json
                GIT_REPOSITORY https://github.com/nfx-libs/nfx-json.git
                GIT_TAG        ${NFX_DEPS_NFX_JSON_VERSION}
                GIT_SHALLOW    TRUE
                EXCLUDE_FROM_ALL
        )
    endif()

    if(NOT nfx-json_FOUND)
        FetchContent_MakeAvailable(nfx-json)
    endif()
endif()

#----------------------------------------------
# Cleanup
#----------------------------------------------

set(CMAKE_REQUIRED_QUIET    ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY      ${_SAVED_CMAKE_FIND_QUIETLY})
