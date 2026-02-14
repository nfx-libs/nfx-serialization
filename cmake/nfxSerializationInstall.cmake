#==============================================================================
# nfx-serialization - Library installation
#==============================================================================

#----------------------------------------------
# Installation condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_INSTALL_PROJECT)
    return()
endif()

#----------------------------------------------
# Install headers
#----------------------------------------------

# Install nfx-serialization headers
install(
    DIRECTORY "${NFX_SERIALIZATION_INCLUDE_DIR}/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

# Install nfx-json headers (bundled dependency)
install(
    DIRECTORY "${nfx-json_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

#----------------------------------------------
# Install library targets
#----------------------------------------------

# Header-only interface library
set(install_targets ${PROJECT_NAME})

install(
    TARGETS ${install_targets}
    EXPORT nfx-serialization-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#----------------------------------------------
# Install CMake export files
#----------------------------------------------

install(
    EXPORT nfx-serialization-targets
    FILE nfx-serialization-targets.cmake
    NAMESPACE nfx-serialization::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-serialization
    COMPONENT Development
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/nfx-serialization-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfx-serialization-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nfx-serialization-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-serialization
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/nfx-serialization-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/nfx-serialization-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-serialization
    COMPONENT Development
)

#----------------------------------------------
# Install license files
#----------------------------------------------

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
    RENAME "LICENSE-${PROJECT_NAME}.txt"
)

file(GLOB license_files "${CMAKE_CURRENT_SOURCE_DIR}/licenses/LICENSE-*")
foreach(license_file ${license_files})
    get_filename_component(license_name ${license_file} NAME)
    install(
        FILES ${license_file}
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
        RENAME "${license_name}.txt"
    )
endforeach()

#----------------------------------------------
# Install documentation
#----------------------------------------------

if(NFX_SERIALIZATION_BUILD_DOCUMENTATION)
    install(
        DIRECTORY "${CMAKE_BINARY_DIR}/doc/html"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        OPTIONAL
        COMPONENT Documentation
    )

    if(WIN32)
        # Install Windows .cmd batch file
        install(
            FILES "${CMAKE_BINARY_DIR}/doc/index.html.cmd"
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            OPTIONAL
            COMPONENT Documentation
        )
    else()
        # Install Unix symlink
        install(
            FILES "${CMAKE_BINARY_DIR}/doc/index.html"
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            OPTIONAL
            COMPONENT Documentation
        )
    endif()
endif()

message(STATUS "Installation configured for targets: ${install_targets}")
