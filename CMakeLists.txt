#==============================================================================
# nfx-serialization - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(nfx-serialization
	VERSION 0.2.0
	DESCRIPTION "Cross-platform C++ JSON serialization library with extensible trait capabilities"
	HOMEPAGE_URL "https://github.com/nfx-libs/nfx-serialization"
	LANGUAGES CXX
)

#----------------------------------------------
# Installation prefix configuration
#----------------------------------------------

include(GNUInstallDirs)

# Set default installation path to nfx
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if(WIN32)
		set(CMAKE_INSTALL_PREFIX "C:/Program Files/nfx" CACHE PATH "Installation directory" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX "/usr/local/nfx" CACHE PATH "Installation directory" FORCE)
	endif()
	message(STATUS "Setting install prefix to '${CMAKE_INSTALL_PREFIX}'")
endif()

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(NFX_SERIALIZATION_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "nfx-serialization major version")
set(NFX_SERIALIZATION_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "nfx-serialization minor version")
set(NFX_SERIALIZATION_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "nfx-serialization patch version")
set(NFX_SERIALIZATION_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "nfx-serialization version")

#----------------------------------------------
# Project metadata
#----------------------------------------------

set(NFX_SERIALIZATION_LICENSE "MIT")
set(NFX_SERIALIZATION_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- JSON serialization support ---
option(NFX_SERIALIZATION_WITH_JSON             "Enable JSON serialization support"  OFF)

# --- Library build types ---
option(NFX_SERIALIZATION_BUILD_STATIC          "Build static library"               OFF)
option(NFX_SERIALIZATION_BUILD_SHARED          "Build shared library"               OFF)

# --- Build components ---
option(NFX_SERIALIZATION_BUILD_TESTS           "Build tests"                        OFF)
option(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS "Build extension tests"              OFF)
option(NFX_SERIALIZATION_BUILD_SAMPLES         "Build samples"                      OFF)
option(NFX_SERIALIZATION_BUILD_BENCHMARKS      "Build benchmarks"                   OFF)
option(NFX_SERIALIZATION_BUILD_DOCUMENTATION   "Build Doxygen documentation"        OFF)

# --- Installation ---
option(NFX_SERIALIZATION_INSTALL_PROJECT       "Install project"                    OFF)

# --- Packaging ---
option(NFX_SERIALIZATION_PACKAGE_SOURCE        "Enable source package generation"   OFF)
option(NFX_SERIALIZATION_PACKAGE_ARCHIVE       "Enable TGZ/ZIP package generation"  OFF)
option(NFX_SERIALIZATION_PACKAGE_DEB           "Enable DEB package generation"      OFF)
option(NFX_SERIALIZATION_PACKAGE_RPM           "Enable RPM package generation"      OFF)
option(NFX_SERIALIZATION_PACKAGE_WIX           "Enable WiX Windows installer (MSI)" OFF)

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(nfxSerializationBuildConfig)
include(nfxSerializationDependencies)
include(nfxSerializationSources)
include(nfxSerializationTargets)

#----------------------------------------------
# Installation and packaging
#----------------------------------------------

include(nfxSerializationInstall)
include(nfxSerializationPackaging)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(test)
add_subdirectory(samples)
add_subdirectory(benchmark)
add_subdirectory(doc)
