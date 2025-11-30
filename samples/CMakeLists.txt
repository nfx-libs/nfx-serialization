#==============================================================================
# nfx-serialization - Samples
#==============================================================================

#----------------------------------------------
# Sample condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_BUILD_SAMPLES)
	message(STATUS "Samples disabled, skipping...")
	return()
endif()

#----------------------------------------------
# Samples source files
#----------------------------------------------

set(sample_sources)

if(NFX_SERIALIZATION_WITH_JSON)
	list(APPEND sample_sources
		Sample_JSONArrayIterator.cpp
		Sample_JSONDocument.cpp
		Sample_JSONObjectFieldIterator.cpp
		Sample_JSONPathView.cpp
		Sample_JSONPointer.cpp
		Sample_JSONSchemaGenerator.cpp
		Sample_JSONSchemaValidator.cpp
		Sample_JSONSerializer.cpp
	)
endif()

if(NOT sample_sources)
	message(STATUS "No sample sources available, skipping...")
	return()
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

		target_link_libraries(${sample_target_name} PRIVATE
			nfx-serialization::static
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${sample_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
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
