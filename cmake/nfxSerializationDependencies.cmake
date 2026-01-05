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

set(NFX_DEPS_STRINGUTILS_VERSION   "0.4.0" )
set(NFX_DEPS_NLOHMANN_JSON_VERSION "3.11.0")

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

# --- nfx-stringutils ---
find_package(nfx-stringutils ${NFX_DEPS_STRINGUTILS_VERSION} QUIET)
if(NOT nfx-stringutils_FOUND)
	FetchContent_Declare(
		nfx-stringutils
		GIT_REPOSITORY https://github.com/nfx-libs/nfx-stringutils.git
		GIT_TAG        ${NFX_DEPS_STRINGUTILS_VERSION}
		GIT_SHALLOW    TRUE
	)
endif()

if(NOT nfx-stringutils_FOUND)
	FetchContent_MakeAvailable(nfx-stringutils)
	set(NFX_STRINGUTILS_INCLUDE_DIR "${nfx-stringutils_SOURCE_DIR}/include" CACHE INTERNAL "nfx-stringutils include directory")
endif()

# --- nlohmann/json ---
set(NFX_SERIALIZATION_NLOHMANN_JSON_MIN_VERSION "3.11.0" CACHE STRING "Minimum required nlohmann/json version")

if(NFX_SERIALIZATION_WITH_JSON)
	find_package(nlohmann_json ${NFX_SERIALIZATION_NLOHMANN_JSON_MIN_VERSION} QUIET)
	if(NOT nlohmann_json_FOUND)
		message(STATUS "nlohmann/json not found on system, using FetchContent")
	
		FetchContent_Declare(nlohmann_json
			URL https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip
			URL_HASH SHA256=b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
	else()
		message(STATUS "Using system-installed nlohmann/json version ${nlohmann_json_VERSION}")
	endif()
endif()

if(NFX_SERIALIZATION_WITH_JSON)
	if(NOT nlohmann_json_FOUND)
		FetchContent_MakeAvailable(nlohmann_json)
		set(NLOHMANN_JSON_INCLUDE_DIR "${nlohmann_json_SOURCE_DIR}/include" CACHE INTERNAL "nlohmann_json include directory")
	endif()
endif()

#----------------------------------------------
# Cleanup
#----------------------------------------------

set(CMAKE_REQUIRED_QUIET    ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY      ${_SAVED_CMAKE_FIND_QUIETLY})
