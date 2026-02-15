#==============================================================================
# nfx-serialization - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(nfx-serialization
    VERSION 0.9.3
    DESCRIPTION "Cross-platform C++ JSON serialization library with extensible trait capabilities"
    HOMEPAGE_URL "https://github.com/nfx-libs/nfx-serialization"
    LANGUAGES CXX
)

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

set(NFX_SERIALIZATION_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- JSON serialization support ---
option(NFX_SERIALIZATION_WITH_JSON             "Enable JSON serialization support"  OFF)

# --- Performance optimizations ---
option(NFX_SERIALIZATION_ENABLE_SIMD           "Enable SIMD CPU optimizations"      ON )

# --- Build components ---
option(NFX_SERIALIZATION_BUILD_TESTS           "Build tests"                        OFF)
option(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS "Build extension tests"              OFF)
option(NFX_SERIALIZATION_BUILD_SAMPLES         "Build samples"                      OFF)
option(NFX_SERIALIZATION_BUILD_BENCHMARKS      "Build benchmarks"                   OFF)
option(NFX_SERIALIZATION_BUILD_DOCUMENTATION   "Build Doxygen documentation"        OFF)

# --- Installation & packaging ---
option(NFX_SERIALIZATION_INSTALL_PROJECT       "Install project"                    OFF)
option(NFX_SERIALIZATION_PACKAGE_SOURCE        "Enable source package generation"   OFF)

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(nfxSerializationBuildConfig)
include(nfxSerializationDependencies)
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
