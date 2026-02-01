#==============================================================================
# nfx-serialization - Samples
#==============================================================================

#----------------------------------------------
# Sample condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_BUILD_SAMPLES)
    return()
endif()

#----------------------------------------------
# Fetch optional nfx extension libraries for nfx types sample
#----------------------------------------------

if(NFX_SERIALIZATION_WITH_JSON)
    include(FetchContent)

    # Try to find libraries first, then fetch if not found
    find_package(nfx-containers 0.3.1 QUIET)
    find_package(nfx-datatypes  0.2.0 QUIET)
    find_package(nfx-datetime   0.3.0 QUIET)

    # nfx-containers (header-only)
    if(NOT nfx-containers_FOUND AND NOT TARGET nfx-containers::nfx-containers)
        FetchContent_Declare(
            nfx-containers
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-containers.git
            GIT_TAG        main
        )
        list(APPEND nfx_libs_to_fetch nfx-containers)
    endif()

    # nfx-datatypes (requires static library)
    if(NOT nfx-datatypes_FOUND AND NOT TARGET nfx-datatypes::static)
        set(NFX_DATATYPES_BUILD_STATIC ON CACHE BOOL "" FORCE)
        FetchContent_Declare(
            nfx-datatypes
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-datatypes.git
            GIT_TAG        main
        )
        list(APPEND nfx_libs_to_fetch nfx-datatypes)
    endif()

    # nfx-datetime (requires static library)
    if(NOT nfx-datetime_FOUND AND NOT TARGET nfx-datetime::static)
        set(NFX_DATETIME_BUILD_STATIC ON CACHE BOOL "" FORCE)
        FetchContent_Declare(
            nfx-datetime
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-datetime.git
            GIT_TAG        main
        )
        list(APPEND nfx_libs_to_fetch nfx-datetime)
    endif()

    # Only fetch libraries that weren't found
    if(nfx_libs_to_fetch)
        FetchContent_MakeAvailable(${nfx_libs_to_fetch})
    endif()
endif()

#----------------------------------------------
# Samples source files
#----------------------------------------------

set(sample_sources "")

if(NFX_SERIALIZATION_WITH_JSON)
    list(APPEND sample_sources
        extensions/Sample_JsonSerializationNfxTypes.cpp
        Sample_JsonSerializationBasics.cpp
        Sample_JsonSerializationContainers.cpp
        Sample_JsonSerializationStlContainers.cpp
        Sample_JsonSerializationTraits.cpp
    )
endif()

#----------------------------------------------
# Configure samples executables
#----------------------------------------------

foreach(sample_source ${sample_sources})
    get_filename_component(sample_target_name ${sample_source} NAME_WE)

    if(NOT TARGET ${sample_target_name})
        add_executable(${sample_target_name} ${sample_source})

        #----------------------------------------------
        # Target linking
        #----------------------------------------------

        target_link_libraries(${sample_target_name}
            PRIVATE
                nfx-serialization::nfx-serialization
        )

        #----------------------------------------------
        # Additional linking for nfx types sample
        #----------------------------------------------

        if(${sample_target_name} STREQUAL "Sample_JsonSerializationNfxTypes")
            if(TARGET nfx-containers::nfx-containers)
                target_link_libraries(${sample_target_name}
                    PRIVATE
                        nfx-containers::nfx-containers
                )
            endif()

            if(TARGET nfx-datatypes::static)
                target_link_libraries(${sample_target_name}
                    PRIVATE
                        nfx-datatypes::static
                )
            endif()

            if(TARGET nfx-datetime::static)
                target_link_libraries(${sample_target_name}
                    PRIVATE
                        nfx-datetime::static
                )
            endif()
        endif()

        #----------------------------------------------
        # Enable specific CPU features
        #----------------------------------------------

        target_compile_options(${sample_target_name}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
        )

        #----------------------------------------------
        # Properties
        #----------------------------------------------

        set_target_properties(${sample_target_name}
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                POSITION_INDEPENDENT_CODE ON
                DEBUG_POSTFIX "-d"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endforeach()
