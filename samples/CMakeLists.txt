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
# Samples source files
#----------------------------------------------

set(sample_sources "")

if(NFX_SERIALIZATION_WITH_JSON)
    list(APPEND sample_sources
        Sample_JsonArrayIterator.cpp
        Sample_JsonDocument.cpp
        Sample_JsonObjectFieldIterator.cpp
        Sample_JsonPathView.cpp
        Sample_JsonPointer.cpp
        Sample_JsonSchemaGenerator.cpp
        Sample_JsonSchemaValidator.cpp
        Sample_JsonSerializer.cpp
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
                nfx-serialization::static
        )

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

#----------------------------------------------
# Copy testdata to output directory
#----------------------------------------------

if(EXISTS "${NFX_SERIALIZATION_TESTDATA_DIR}")
    add_custom_target(copy_testdata_samples ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${NFX_SERIALIZATION_TESTDATA_DIR}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples/testdata"
        COMMENT "Copying testdata to samples output directory"
    )
endif()
