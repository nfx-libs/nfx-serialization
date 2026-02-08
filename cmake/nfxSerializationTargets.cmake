#==============================================================================
# nfx-serialization - CMake targets
#==============================================================================

#----------------------------------------------
# Header-only library
#----------------------------------------------

# nfx-serialization is header-only, no sources to compile
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Create ::static alias for compatibility with existing code
add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME})

#----------------------------------------------
# Target configuration
#----------------------------------------------

# Include directories
target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${NFX_SERIALIZATION_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Link to nfx-json dependency (if JSON support enabled)
if(NFX_SERIALIZATION_WITH_JSON)
    target_link_libraries(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:nfx-json::static>
    )
endif()

# C++20 requirement
target_compile_features(${PROJECT_NAME}
    INTERFACE
        cxx_std_20
)
